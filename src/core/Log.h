#pragma once

#include <spdlog/spdlog.h>

class Log
{
public:
    inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger_; }

    static void Init();
    static void Shutdown();

#ifdef DEBUG_BUILD
    template<typename... Args>
    static void Trace(fmt::format_string<Args...> fmt, Args&&... args)
    {
        if (!s_CoreLogger_) return;
        s_CoreLogger_->trace(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void Debug(fmt::format_string<Args...> fmt, Args&&... args)
    {
        if (!s_CoreLogger_) return;
        s_CoreLogger_->debug(fmt, std::forward<Args>(args)...);
    }
#else
    template<typename... Args> static void Trace(const char*, Args&&...) {}
    template<typename... Args> static void Debug(const char*, Args&&...) {}
#endif

    template<typename... Args>
    static void Info(fmt::format_string<Args...> fmt, Args&&... args)
    {
        if (!s_CoreLogger_) return;
        s_CoreLogger_->info(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void Warn(fmt::format_string<Args...> fmt, Args&&... args)
    {
        if (!s_CoreLogger_) return;
        s_CoreLogger_->warn(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void Error(fmt::format_string<Args...> fmt, Args&&... args)
    {
        if (!s_CoreLogger_) return;
        s_CoreLogger_->error(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void Critical(fmt::format_string<Args...> fmt, Args&&... args)
    {
        if (!s_CoreLogger_) return;
        s_CoreLogger_->critical(fmt, std::forward<Args>(args)...);
    }

private:
    static std::shared_ptr<spdlog::logger> s_CoreLogger_;
};
