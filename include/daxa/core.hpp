#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <array>
#include <filesystem>
#include <functional>
#include <chrono>

#include <daxa/types.hpp>

#if !defined(NDEBUG)
#define DAXA_DEBUG
#endif

#if defined(DAXA_DEBUG)
#include <iostream>
#include <cstdlib>

#define DAXA_DBG_ASSERT_FAIL_STRING "[[DAXA ASSERT FAILURE]]"

#define DAXA_DBG_ASSERT_TRUE_M(x, m)                                              \
    [&] {                                                                         \
        if (!(x))                                                                 \
        {                                                                         \
            std::cerr << DAXA_DBG_ASSERT_FAIL_STRING << ": " << (m) << std::endl; \
            std::exit(-1);                                                        \
        }                                                                         \
    }()
#else

#define DAXA_DBG_ASSERT_TRUE_M(x, m) \
    [&] {                            \
        (x);                         \
    }()

#endif

#if defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace daxa
{
    using NativeWindowHandle = HWND;
} // namespace daxa
#endif

#if defined(__linux__)
#define VK_USE_PLATFORM_XLIB_KHR

#include <X11/Xlib.h>

namespace daxa
{
    using NativeWindowHandle = ::Window;
} // namespace daxa
#endif

namespace daxa
{

    struct Handle
    {
        Handle(std::shared_ptr<void> impl);

      protected:
        std::shared_ptr<void> impl = {};
    };

    struct ResultErr
    {
        std::string message = {};
    };

    template <typename T>
    struct Result
    {
        std::optional<T> v = {};
        std::string m = {};

      public:
        Result(T && value)
            : v{std::move(value)}, m{""}
        {
        }

        Result(T const & value)
            : v{value}, m{""}
        {
        }

        Result(std::optional<T> && opt)
            : v{std::move(opt)}, m{opt.has_value() ? "" : "default error message"}
        {
        }

        Result(std::optional<T> const & opt)
            : v{opt}, m{opt.has_value() ? "" : "default error message"}
        {
        }

        Result(ResultErr const & err)
            : v{std::nullopt}, m{err.message}
        {
        }

        Result(std::string_view message)
            : v{std::nullopt}, m{message}
        {
        }

        bool isOk() const
        {
            return v.has_value();
        }

        bool isErr() const
        {
            return !v.has_value();
        }

        auto value() const -> T const &
        {
            return v.value();
        }

        auto value() -> T &
        {
            DAXA_DBG_ASSERT_TRUE_M(v.has_value(), (m != "" ? m : "tried getting value of empty Result"));
            return v.value();
        }

        auto message() const -> std::string const &
        {
            return m;
        }

        operator bool() const
        {
            return v.has_value();
        }

        auto operator!() const -> bool
        {
            return !v.has_value();
        }
    };
} // namespace daxa