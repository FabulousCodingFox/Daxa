#include "Device.hpp"

#include <iostream>
#include <algorithm>

#include <VkBootstrap.hpp>

namespace daxa {
	namespace gpu {
		std::shared_ptr<vkb::Instance> instance;
		vk::DebugUtilsMessengerEXT debugMessenger;
		bool initialized = false;

		void initGlobals() {
			vkb::InstanceBuilder instanceBuilder;
			instanceBuilder
				.set_app_name("Daxa Application")
				.set_engine_name("Daxa")
				.request_validation_layers(true)
				.require_api_version(1, 2, 0)
				.use_default_debug_messenger();
			auto instanceBuildReturn = instanceBuilder.build();
			if (!instanceBuildReturn) {
				std::cerr << "could not create vulkan instance!\n";
			}

			instance = std::make_shared<vkb::Instance>(std::move(instanceBuildReturn.value()));
			debugMessenger = instance->debug_messenger;
		}


		std::shared_ptr<vkb::Instance> Device::getInstance() {
			if (!initialized) {
				initGlobals();
				initialized = true;
			}
			return instance;
		}

		std::shared_ptr<Device> Device::createNewDevice() {
			if (!initialized) {
				initGlobals();
				initialized = true;
			}
			vkb::PhysicalDeviceSelector selector{ *instance };
			vkb::PhysicalDevice physicalDevice = selector
				.set_minimum_version(1, 2)
				.defer_surface_initialization()
				.require_separate_compute_queue()
				.require_separate_transfer_queue()
				.add_desired_extension(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)
				.select()
				.value();

			auto physicslDevice = physicalDevice.physical_device;

			vk::PhysicalDeviceDescriptorIndexingFeatures descriptor_indexing_features{
				.shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
				.descriptorBindingPartiallyBound = VK_TRUE,
				.descriptorBindingVariableDescriptorCount = VK_TRUE,
				.runtimeDescriptorArray = VK_TRUE,
			};

			vkb::DeviceBuilder deviceBuilder{ physicalDevice };
			deviceBuilder.add_pNext(&descriptor_indexing_features);

			vkb::Device vkbDevice = deviceBuilder.build().value();

			auto device = vkbDevice.device;

			auto mainGraphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
			auto mainGraphicsQueueFamiltyIndex = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

			VmaAllocatorCreateInfo allocatorInfo = {};
			allocatorInfo.physicalDevice = physicslDevice;
			allocatorInfo.device = device;
			allocatorInfo.instance = instance->instance;
			VmaAllocator allocator;
			vmaCreateAllocator(&allocatorInfo, &allocator);

			auto fnPtrvkCmdBeginRenderingKHR = (void(*)(VkCommandBuffer, const VkRenderingInfoKHR*))vkGetDeviceProcAddr(device, "vkCmdBeginRenderingKHR");
			auto fnPtrvkCmdEndRenderingKHR = (void(*)(VkCommandBuffer))vkGetDeviceProcAddr(device, "vkCmdEndRenderingKHR");
			assert(fnPtrvkCmdBeginRenderingKHR != nullptr && fnPtrvkCmdEndRenderingKHR != nullptr);

			std::shared_ptr<Device> ret = std::make_shared<Device>();
			ret->device = device;
			ret->descriptorLayoutCache = { device };
			ret->physicalDevice = physicalDevice.physical_device;
			ret->graphicsQ = mainGraphicsQueue;
			ret->graphicsQFamilyIndex = mainGraphicsQueueFamiltyIndex;
			ret->allocator = allocator;
			ret->vkCmdBeginRenderingKHR = fnPtrvkCmdBeginRenderingKHR;
			ret->vkCmdEndRenderingKHR = fnPtrvkCmdEndRenderingKHR;
			ret->initFrameContexts();
			return std::move(ret);
		}

		Device::~Device() {
			for (int i = 0; i < 3; i++) {
				nextFrameContext();
			}
			printf("Device is now gone!\n");
		}

		ImageHandle Device::createImage2d(Image2dCreateInfo ci) { 
			return ImageHandle{ std::make_shared<Image>(std::move(Image::create2dImage(device, allocator, ci))) };
		}

		BufferHandle Device::createBuffer(BufferCreateInfo ci) {
			return BufferHandle{ std::make_shared<Buffer>(std::move(Buffer(device, graphicsQFamilyIndex, allocator, ci))) };
		}

		CommandList Device::getEmptyCommandList() {
			return std::move(getNextCommandList());
		}

		void Device::submit(CommandList&& cmdList, SubmitInfo const& submitInfo) {
			CommandList list{ std::move(cmdList) };
			assert(list.operationsInProgress == 0);
			auto& frame = *this->frameContexts.front();

			auto thisSubmitFence = getNextFence();

			vk::SubmitInfo si{};

			vk::PipelineStageFlags pipelineStages = vk::PipelineStageFlagBits::eAllCommands;
			si.pWaitDstStageMask = &pipelineStages;

			vk::CommandBuffer commandBuffers[] = { list.cmd };
			si.pCommandBuffers = commandBuffers;
			si.commandBufferCount = 1;
			si.pWaitSemaphores = submitInfo.waitOnSemaphores.data();
			si.waitSemaphoreCount = submitInfo.waitOnSemaphores.size();
			si.pSignalSemaphores = submitInfo.signalSemaphores.data();
			si.signalSemaphoreCount = submitInfo.signalSemaphores.size();

			graphicsQ.submit(si, thisSubmitFence);

			frame.usedCommandLists.push_back(std::move(list));
		}

		void Device::submit(std::vector<CommandList>& cmdLists, SubmitInfo const& submitInfo) {
			auto& frame = *this->frameContexts.front();
			auto thisSubmitFence = getNextFence();

			submitCommandBufferBuffer.clear();
			for (auto& cmdList : cmdLists) {
				assert(cmdList.operationsInProgress == 0);
				submitCommandBufferBuffer.push_back(cmdList.cmd);
			}

			vk::PipelineStageFlags pipelineStages = vk::PipelineStageFlagBits::eAllCommands;
			vk::SubmitInfo si{};
			si.pWaitDstStageMask = &pipelineStages;
			si.pCommandBuffers = submitCommandBufferBuffer.data();
			si.commandBufferCount = static_cast<u32>(submitCommandBufferBuffer.size());
			si.pWaitSemaphores = submitInfo.waitOnSemaphores.data();
			si.waitSemaphoreCount = submitInfo.waitOnSemaphores.size();
			si.pSignalSemaphores = submitInfo.signalSemaphores.data();
			si.signalSemaphoreCount = submitInfo.signalSemaphores.size();

			graphicsQ.submit(si, thisSubmitFence);

			while (!cmdLists.empty()) {
				auto list = std::move(cmdLists.back());
				cmdLists.pop_back();
				frame.usedCommandLists.push_back(std::move(list));
			}
		}

		void Device::present(SwapchainImage const& sImage, std::vector<vk::Semaphore> const& waitOn) {
			vk::PresentInfoKHR presentInfo{};
			presentInfo.pImageIndices = &sImage.imageIndex;
			presentInfo.pSwapchains = &sImage.swapchain;
			presentInfo.swapchainCount = 1;
			presentInfo.pWaitSemaphores = waitOn.data();
			presentInfo.waitSemaphoreCount = static_cast<u32>(waitOn.size());

			graphicsQ.presentKHR(presentInfo);
		}

		void Device::nextFrameContext() {
			// get new frame context
			auto lastFrame = std::move(frameContexts.front());
			frameContexts.pop_front();
			frameContexts.push_back(std::move(lastFrame));

			auto& frame = *frameContexts.front();

			// wait on frame context to finish on gpu:
			if (!frame.usedFences.empty())
				device.waitForFences(frame.usedFences, VK_TRUE, 100000000);

			// reset frame ressources:
			if (!frame.usedFences.empty())
				device.resetFences(frame.usedFences);
			while (!frame.usedFences.empty()) {
				auto back = frame.usedFences.back();
				frame.usedFences.pop_back();
				unusedFences.push_back(back);
			}
			// we reset the command buffers individually as there is generally no big performance cost to that
			//device.resetCommandPool(*frame.cmdPool, vk::CommandPoolResetFlagBits::eReleaseResources);
			// reset and recycle CommandLists
			while (!frame.usedCommandLists.empty()) {
				auto list = std::move(frame.usedCommandLists.back());
				frame.usedCommandLists.pop_back();
				list.reset();
				unusedCommandLists.push_back(std::move(list));
			}
		}

		void Device::initFrameContexts() {
			// The amount of contexts is currently hardcoded to two wich is fine for now.
			// A later extention to adapt the amount of contexts may be a nice feature.
			for (int i = 0; i < 3; i++) {
				auto fc = std::make_unique<FrameContext>();
				this->frameContexts.push_back(std::move(fc));
			}
		}

		CommandList Device::getNextCommandList() {
			auto& frame = *frameContexts.front();
			if (unusedCommandLists.empty()) {
				// we have no command lists left, we need to create new ones:
				CommandList list;
				list.device = device;
				vk::CommandPoolCreateInfo cpci{};
				cpci.queueFamilyIndex = graphicsQFamilyIndex;
				list.cmdPool = device.createCommandPool(cpci);
				vk::CommandBufferAllocateInfo cbai{};
				cbai.commandPool = list.cmdPool;
				cbai.commandBufferCount = 1;
				cbai.level = vk::CommandBufferLevel::ePrimary;
				list.cmd = std::move(device.allocateCommandBuffers(cbai).front());
				list.vkCmdBeginRenderingKHR = this->vkCmdBeginRenderingKHR;
				list.vkCmdEndRenderingKHR = this->vkCmdEndRenderingKHR;
				unusedCommandLists.push_back(std::move(list));
			} 
			auto ret = std::move(unusedCommandLists.back());
			unusedCommandLists.pop_back();
			return std::move(ret);
		}

		vk::Semaphore Device::getNextSemaphore() {
			auto& frame = *frameContexts.front();
			if (unusedSemaphores.empty()) {
				unusedSemaphores.push_back(device.createSemaphore({}));
			}
			auto ret = unusedSemaphores.back();
			unusedSemaphores.pop_back();
			frame.usedSemaphores.push_back(ret);
			return ret;
		}

		vk::Fence Device::getNextFence() {
			auto& frame = *frameContexts.front();
			if (unusedFences.empty()) {
				unusedFences.push_back(device.createFence({}));
			}
			auto ret = unusedFences.back();
			unusedFences.pop_back();
			frame.usedFences.push_back(ret);
			return ret;
		}

		std::optional<ShaderModuleHandle> Device::tryCreateShderModuleFromGLSL(std::string const& glslSource, vk::ShaderStageFlagBits stage, std::string const& name) {
			return ShaderModuleHandle::tryCreateDAXAShaderModule(device, glslSource, name, stage);
		}

		std::optional<ShaderModuleHandle> Device::tryCreateShderModuleFromGLSL(std::filesystem::path const& pathToGlsl, vk::ShaderStageFlagBits stage, std::string const& name) {
			return ShaderModuleHandle::tryCreateDAXAShaderModule(device, pathToGlsl, name, stage);
		}

		GraphicsPipelineHandle Device::createGraphicsPipeline(GraphicsPipelineBuilder const& pipelineBuilder) {
			return pipelineBuilder.build(device, *descriptorLayoutCache);
		}
	}
}