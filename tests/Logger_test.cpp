#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/ostream_sink.h>
#include <sstream>

TEST(LoggerCaptureTest, InfoGoesToSink)
{
    std::ostringstream buf;
    auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(buf);
    auto logger = std::make_shared<spdlog::logger>("test_logger", sink);
    spdlog::register_logger(logger);
    logger->set_level(spdlog::level::info);

    logger->info("hello {}", "world");

    std::string out = buf.str();
    EXPECT_NE(out.find("hello world"), std::string::npos);

    spdlog::drop("test_logger");
}