#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

std::shared_ptr<spdlog::logger> Log::s_CoreLogger_;

void Log::Init()
{
    try
    {
        const auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        const auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("engine.log", 1024 * 1024, 1);

        consoleSink->set_pattern("[%H:%M:%S.%e] [%^%l%$] [thread %t] %v");
        fileSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [thread %t] %v");

        std::array<spdlog::sink_ptr, 2> sinks = { consoleSink, fileSink };

        s_CoreLogger_ = std::make_shared<spdlog::logger>("ENGINE", sinks.begin(), sinks.end());
        spdlog::register_logger(s_CoreLogger_);

        s_CoreLogger_->set_level(spdlog::level::debug);

        s_CoreLogger_->flush_on(spdlog::level::err);
    } catch (const spdlog::spdlog_ex& ex)
    {
        fprintf(stderr, "Log initialization failed: %s\n", ex.what());
    }
}

void Log::Shutdown()
{
    spdlog::drop_all();
}
