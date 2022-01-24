#include "BindingSet.hpp"
#include "Instance.hpp"

namespace daxa {
	namespace gpu {
		BindingSet::BindingSet(std::shared_ptr<DeviceBackend> deviceBackend, VkDescriptorSet set, std::weak_ptr<BindingSetAllocatorBindingiSetPool> pool, std::shared_ptr<BindingSetInfo const> info)
			: deviceBackend{ std::move(deviceBackend) }
			, set { set }
			, pool{ pool }
			, info{ std::move(info) }
		{ }

		thread_local std::vector<VkDescriptorImageInfo> descImageInfoBuffer = {};

		void BindingSet::bindSamplers(u32 binding, std::span<SamplerHandle> samplers, u32 descriptorArrayOffset) {
			DAXA_ASSERT_M(usesOnGPU == 0, "can not update binding set while it is used on gpu");
			descImageInfoBuffer.reserve(samplers.size());

			for (auto& sampler : samplers) {
				DAXA_ASSERT_M(sampler, "invalid sampler handle");
				descImageInfoBuffer.push_back(VkDescriptorImageInfo{
					.sampler = sampler->getVkSampler(),
					.imageView = VK_NULL_HANDLE,
					.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				});
			}

			VkWriteDescriptorSet write{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = set,
				.dstBinding = binding,
				.dstArrayElement = descriptorArrayOffset,
				.descriptorCount = (u32)descImageInfoBuffer.size(),
				.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.pImageInfo = descImageInfoBuffer.data()
			};

			vkUpdateDescriptorSets(deviceBackend->device.device, 1, &write, 0, nullptr);

			descImageInfoBuffer.clear();
		}

		void BindingSet::bindSampler(u32 binding, SamplerHandle sampler, u32 dstArrayElement) {
			bindSamplers(binding, { &sampler,1 }, dstArrayElement);
		}

		thread_local std::vector<VkDescriptorBufferInfo> descBufferInfoBuffer = {};

		void BindingSet::bindBuffers(u32 binding, std::span<BufferHandle> buffers, u32 descriptorArrayOffset) {
			DAXA_ASSERT_M(usesOnGPU == 0, "can not update binding set, that is still in use on the gpu");
			for (auto& buffer : buffers) {
				DAXA_ASSERT_M(buffer, "invalid buffer handle");
				descBufferInfoBuffer.push_back(VkDescriptorBufferInfo{
					.buffer = buffer->getVkBuffer(),
					.offset = 0,									// TODO Unsure what to put here
					.range = buffer->getSize(),
				});
			}

			VkWriteDescriptorSet write{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = set,
				.dstBinding = binding,
				.dstArrayElement = descriptorArrayOffset,
				.descriptorCount = (u32)descBufferInfoBuffer.size(),
				.descriptorType = this->info->getDescription().bindings[binding].descriptorType,
				.pBufferInfo = descBufferInfoBuffer.data(),
			};

			vkUpdateDescriptorSets(deviceBackend->device.device, 1, &write, 0, nullptr);

			descBufferInfoBuffer.clear();
		}

		void BindingSet::bindBuffer(u32 binding, BufferHandle buffer, u32 dstArrayElement) {
			bindBuffers(binding, { &buffer, 1 }, dstArrayElement);
		}

		void BindingSet::bindImages(u32 binding, std::span<std::pair<ImageHandle, VkImageLayout>> images, u32 descriptorArrayOffset) {
			DAXA_ASSERT_M(usesOnGPU == 0, "can not update binding set while it is used on gpu");
			VkDescriptorType imageDescriptorType = info->getDescription().bindings[binding].descriptorType;
			bool bIsImage = imageDescriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER || 
				imageDescriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE || 
				imageDescriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			DAXA_ASSERT_M(bIsImage, "tried binding image to non image binding index");
			
			for (auto& [image, layout] : images) {
				DAXA_ASSERT_M(image, "invalid image handle");
				VkSampler sampler = VK_NULL_HANDLE;
				if (imageDescriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
					DAXA_ASSERT_M(image->getSampler().valid(), "can not bind image without sampler to a combined image sampler binding");
					sampler = image->getSampler()->getVkSampler();
				}

				descImageInfoBuffer.push_back(VkDescriptorImageInfo{
					.sampler = sampler,
					.imageView = image->getVkView(),
					.imageLayout = layout,
				});
			}

			VkWriteDescriptorSet write{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = set,
				.dstBinding = binding,
				.dstArrayElement = descriptorArrayOffset,
				.descriptorCount = (u32)descImageInfoBuffer.size(),
				.descriptorType = imageDescriptorType,
				.pImageInfo = descImageInfoBuffer.data()
			};

			vkUpdateDescriptorSets(deviceBackend->device.device, 1, &write, 0, nullptr);

			descImageInfoBuffer.clear();
		}

		void BindingSet::bindImage(u32 binding, ImageHandle image, VkImageLayout imgLayout, u32 dstArrayElement) {
			std::pair imgAndLayout = { image, imgLayout };
			bindImages(binding, { &imgAndLayout, 1 }, dstArrayElement);
		}
		
		BindingSetInfo::BindingSetInfo(std::shared_ptr<DeviceBackend> deviceBackend, BindingSetDescription const& description) 
			: deviceBackend{ std::move(deviceBackend) }
			, description{ description }
		{
			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCI{
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.bindingCount = (u32)description.bindingsCount,
				.pBindings = description.bindings.data(),
			};
			vkCreateDescriptorSetLayout(this->deviceBackend->device.device, &descriptorSetLayoutCI, nullptr, &layout);

			for (auto& binding : this->description.getBIndingsSpan()) {
				this->descriptorCount += binding.descriptorCount;
			}
		}
		void BindingSetInfo::cleanup() {
			if (deviceBackend) {
				vkDestroyDescriptorSetLayout(deviceBackend->device.device, layout, nullptr);
				deviceBackend = {};
			}
		}
		BindingSetInfo::~BindingSetInfo() {
			cleanup();
		}
		BindingSetInfo::BindingSetInfo(BindingSetInfo&& other) noexcept {
			this->deviceBackend = std::move(other.deviceBackend);
			this->description = std::move(other.description);
			this->layout = std::move(other.layout);
			this->descriptorCount = std::move(other.descriptorCount);
		}
		BindingSetInfo& BindingSetInfo::operator=(BindingSetInfo&& other) noexcept {
			cleanup();
			this->deviceBackend = std::move(other.deviceBackend);
			this->description = std::move(other.description);
			this->layout = std::move(other.layout);
			this->descriptorCount = std::move(other.descriptorCount);
			return *this;
		}
		
		BindingSetDescriptionCache::BindingSetDescriptionCache(std::shared_ptr<DeviceBackend> deviceBackend) 
			: deviceBackend{ std::move(deviceBackend) }
		{ }
		
		std::shared_ptr<BindingSetInfo const> BindingSetDescriptionCache::getInfoShared(BindingSetDescription const& description) {
			for (int i = 0; i < description.bindingsCount-1; i++) {	
				DAXA_ASSERT_M(description.bindings[i].binding < description.bindings[i+1].binding, "bindings in a description must be listed in ascending order");
			}
			if (!map.contains(description)) {
				map[description] = std::make_shared<BindingSetInfo>(deviceBackend, description);
			}
			return map[description];
		}
		
		BindingSetInfo const& BindingSetDescriptionCache::getInfo(BindingSetDescription const& description) {
			for (int i = 0; i < description.bindingsCount-1; i++) {	
				DAXA_ASSERT_M(description.bindings[i].binding < description.bindings[i+1].binding, "bindings in a description must be listed in ascending order");
			}
			if (!map.contains(description)) {
				map[description] = std::make_shared<BindingSetInfo>(deviceBackend, description);
			}
			return *map[description];
		}

		BindingSetAllocator::BindingSetAllocator(std::shared_ptr<DeviceBackend> deviceBackend, BindingSetAllocatorCreateInfo const& ci)
			: deviceBackend{ std::move(deviceBackend) }
			, setInfo{ std::move(ci.setInfo) }
			, setsPerPool{ ci.setPerPool }
		{
			if (instance->pfnSetDebugUtilsObjectNameEXT != nullptr && ci.debugName != nullptr) {
				this->debugName = ci.debugName;
			}

			initPoolSizes();
		}

		BindingSetAllocator::~BindingSetAllocator() {
			if (deviceBackend->device.device) {
				for (auto& pool : pools) {
					auto lock = std::unique_lock(pool->mut);
					DAXA_ASSERT_M(pool->allocatedSets == pool->zombies.size(), "At the time of the descruction of a BindingSetAllocator, there were still living bindingsets left. It is very likely that this bug is caused by not calling waitIdle on the queue before destroying a BindingSetAllocator!");
					vkResetDescriptorPool(deviceBackend->device.device, pool->pool, 0);
					vkDestroyDescriptorPool(deviceBackend->device.device, pool->pool, nullptr);
				}
			}
		}
		
		void BindingSet::setDebugName(char const* debugName) {
			if (instance->pfnSetDebugUtilsObjectNameEXT != nullptr && debugName != nullptr) {
				this->debugName = debugName;

				VkDebugUtilsObjectNameInfoEXT imageNameInfo {
					.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
					.pNext = NULL,
					.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET,
					.objectHandle = (uint64_t)this->set,
					.pObjectName = debugName,
				};
				instance->pfnSetDebugUtilsObjectNameEXT(deviceBackend->device.device, &imageNameInfo);
			}
		}

		BindingSetHandle BindingSetAllocator::getSet(char const* debugName) {
			std::optional<BindingSetHandle> handleOpt = std::nullopt;
			for (auto& pool : pools) {
				auto lock = std::unique_lock(pool->mut);
				if (!pool->zombies.empty()) {
					handleOpt = BindingSetHandle{ std::move(pool->zombies.back()) };
					pool->zombies.pop_back();
					break;
				}
				else if (pool->allocatedSets < setsPerPool) {
					handleOpt = getNewSet(pool);
				}
			}

			if (!handleOpt.has_value()) {
				pools.push_back(std::move(getNewPool()));
				// dont need mutex lock, as we are the only ones that have a ptr to the pool:
				handleOpt = getNewSet(pools.back());
			}

			if (
				instance->pfnSetDebugUtilsObjectNameEXT && 
				debugName != nullptr && 
				handleOpt.value()->getDebugName() != debugName
			) {
				handleOpt.value()->setDebugName(debugName);
			}

			return std::move(handleOpt.value());
		}

		void BindingSetAllocator::initPoolSizes() {
			for (auto const& binding : setInfo->getDescription().getBIndingsSpan()) {
				poolSizes.push_back(VkDescriptorPoolSize{
					.type = binding.descriptorType,
					.descriptorCount = (u32)(setsPerPool * binding.descriptorCount),
				});
			}
		}

		BindingSetHandle BindingSetAllocator::getNewSet(std::shared_ptr<BindingSetAllocatorBindingiSetPool>& pool) {
			VkDescriptorSetAllocateInfo descriptorSetAI{
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
				.pNext = nullptr,
				.descriptorPool = pool->pool,
				.descriptorSetCount = 1,
				.pSetLayouts = &setInfo->getVkDescriptorSetLayout(),
			};
			pool->allocatedSets += 1;
			VkDescriptorSet set;
			vkAllocateDescriptorSets(deviceBackend->device.device, &descriptorSetAI, &set);

			return BindingSetHandle{ std::make_shared<BindingSet>(deviceBackend, set, pool, setInfo) };
		}

		std::shared_ptr<BindingSetAllocatorBindingiSetPool> BindingSetAllocator::getNewPool() {
			VkDescriptorPoolCreateInfo descriptorPoolCI{
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.maxSets = (u32)setsPerPool,
				.poolSizeCount = (u32)poolSizes.size(),
				.pPoolSizes = poolSizes.data(),
			};

			VkDescriptorPool pool = VK_NULL_HANDLE;
			vkCreateDescriptorPool(deviceBackend->device.device, &descriptorPoolCI, nullptr, &pool);
			auto ret = std::make_shared<BindingSetAllocatorBindingiSetPool>();
			ret->pool = pool;
			ret->allocatedSets = 0;
			ret->zombies = {};

			if (instance->pfnSetDebugUtilsObjectNameEXT && !debugName.empty()) {
				poolNameBuffer.clear();
				poolNameBuffer = debugName;
				poolNameBuffer += " pool nr ";
				poolNameBuffer += std::to_string(pools.size());
				
				VkDebugUtilsObjectNameInfoEXT imageNameInfo{
					.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
					.pNext = NULL,
					.objectType = VK_OBJECT_TYPE_DESCRIPTOR_POOL,
					.objectHandle = (uint64_t)pool,
					.pObjectName = poolNameBuffer.c_str(),
				};
				instance->pfnSetDebugUtilsObjectNameEXT(deviceBackend->device.device, &imageNameInfo);
			}

			return std::move(ret);
		}
	}
}