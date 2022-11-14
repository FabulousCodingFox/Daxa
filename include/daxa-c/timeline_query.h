#pragma once

#include <daxa-c/core.h>

namespace daxa
{
    struct TimelineQueryPoolInfo
    {
        daxa_u32 query_count = {};
        std::string debug_name = {};
    };

    struct TimelineQueryPool : ManagedPtr
    {
        TimelineQueryPool() = default;

        auto info() const -> TimelineQueryPoolInfo const &;

        auto get_query_results(daxa_u32 start_index, daxa_u32 count) -> std::vector<daxa_u64>;

      private:
        friend struct Device;
        explicit TimelineQueryPool(ManagedPtr impl);
    };
} // namespace daxa
