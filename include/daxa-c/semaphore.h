#pragma once

#include <daxa-c/core.h>

namespace daxa
{
    struct BinarySemaphoreInfo
    {
        std::string debug_name = {};
    };

    struct BinarySemaphore : ManagedPtr
    {
        BinarySemaphore() = default;

        auto info() const -> BinarySemaphoreInfo const &;

      private:
        friend struct Device;
        friend struct ImplSwapchain;
        explicit BinarySemaphore(ManagedPtr impl);
    };

    struct TimelineSemaphoreInfo
    {
        daxa_u64 initial_value = {};
        std::string debug_name = {};
    };

    struct TimelineSemaphore : ManagedPtr
    {
        TimelineSemaphore() = default;

        auto info() const -> TimelineSemaphoreInfo const &;

        auto value() const -> daxa_u64;
        void set_value(daxa_u64 value);
        auto wait_for_value(daxa_u64 value, daxa_u64 timeout_nanos = ~0ull) -> bool;

      private:
        friend struct Device;
        friend struct ImplSwapchain;
        explicit TimelineSemaphore(ManagedPtr impl);
    };
} // namespace daxa
