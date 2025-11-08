#include <gtest/gtest.h>
#include "parser/GameSpecLoader.h"
#include "parser/GameSpec.h"
<<<<<<< HEAD
#include <filesystem>

=======
#include <spdlog/spdlog.h>
#include <spdlog/sinks/sink.h>
#include <spdlog/pattern_formatter.h>
#include <mutex>
#include <spdlog/sinks/basic_file_sink.h>
#include <filesystem>
#include <fstream>
#include <chrono>
>>>>>>> 00e507b (added some more tests for GameSpecLoader.cpp)

class GameSpecLoaderTest : public ::testing::Test {
protected:
    GameSpecLoader loader;
};

// Memory sink to capture spdlog output for tests
class MemorySink : public spdlog::sinks::sink {
public:
    std::vector<std::string> logs;
    std::mutex m;

    void log(const spdlog::details::log_msg &msg) override {
        std::lock_guard<std::mutex> lock(m);
        spdlog::memory_buf_t formatted;
        if (formatter_) formatter_->format(msg, formatted);
        logs.emplace_back(fmt::to_string(formatted));
    }
    void flush() override {}
    void set_pattern(const std::string &pattern) override { set_formatter(std::make_unique<spdlog::pattern_formatter>(pattern)); }
    void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override { formatter_ = std::move(sink_formatter); }

private:
    std::unique_ptr<spdlog::formatter> formatter_{std::make_unique<spdlog::pattern_formatter>()};
};

TEST_F(GameSpecLoaderTest, LoadGameName) {
    std::filesystem::path filePath = std::filesystem::path(GAMES_DIR) / "hello-test.game";
    GameSpec spec = loader.loadFile(filePath.string().c_str());
    EXPECT_EQ(spec.name, "Hello Test");
}

TEST_F(GameSpecLoaderTest, LoadRockPaperScissors) {
    std::filesystem::path filePath = std::filesystem::path(GAMES_DIR) / "rock-paper-scissors.game";
    GameSpec spec = loader.loadFile(filePath.string().c_str());
    EXPECT_EQ(spec.name, "Rock, Paper, Scissors");
}

TEST_F(GameSpecLoaderTest, LoadFromString) {
    std::string gameText = R"(
configuration {
  name: "Test Game"
  player range: (2, 4)
  audience: false
  setup: { }
}
constants { }
variables { }
per-player { }
per-audience { }
rules { }
)";

    GameSpec spec = loader.loadString(gameText);
    EXPECT_EQ(spec.name, "Test Game");
}

TEST_F(GameSpecLoaderTest, LogsWhenLoadingString) {
    // Test that error logging works via HelloWorldSmokeTest when file is missing.
    auto mem = std::make_shared<MemorySink>();
    auto &sinks = spdlog::default_logger()->sinks();
    size_t old_size = sinks.size();
    sinks.push_back(mem);

    // call HelloWorldSmokeTest with a filename that does not exist to trigger error logging
    bool result = loader.HelloWorldSmokeTest("this_file_does_not_exist_for_log_test.game");
    EXPECT_FALSE(result);

    spdlog::default_logger()->flush();
    bool found_error = false;
    for (auto &s : mem->logs) {
        if (s.find("HelloWorldSmokeTest error") != std::string::npos || s.find("Cannot open file") != std::string::npos || s.find("Parse failed") != std::string::npos) {
            found_error = true;
            break;
        }
    }
    EXPECT_TRUE(found_error) << "Expected HelloWorldSmokeTest to emit an error log when file missing";

    // restore sinks
    sinks.resize(old_size);
}

TEST_F(GameSpecLoaderTest, FileSinkProducesExpectedLog) {
    namespace fs = std::filesystem;
    // create a unique temp file path
    auto stamp = std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    fs::path tmp = fs::temp_directory_path() / ("gamespec_loader_error_log_" + stamp + ".log");

    // attach a file sink to the default logger
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(tmp.string(), true);
    auto &sinks = spdlog::default_logger()->sinks();
    size_t old_size = sinks.size();
    sinks.push_back(file_sink);

    // Trigger an error log by calling HelloWorldSmokeTest on a missing file
    bool result = loader.HelloWorldSmokeTest("another_missing_file_for_error_log.game");
    EXPECT_FALSE(result);

    // flush and read file
    spdlog::default_logger()->flush();
    std::ifstream in(tmp);
    ASSERT_TRUE(in.is_open());
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();

    bool found_error = (content.find("HelloWorldSmokeTest error") != std::string::npos) || (content.find("Cannot open file") != std::string::npos) || (content.find("Parse failed") != std::string::npos);
    EXPECT_TRUE(found_error) << "Expected file log to contain error output, got:\n" << content;

    // cleanup: restore sinks and remove temp file
    sinks.resize(old_size);
    std::error_code ec;
    fs::remove(tmp, ec);
}

TEST_F(GameSpecLoaderTest, HelloWorldSmokeTestFileNotFound) {
    auto mem = std::make_shared<MemorySink>();
    auto &sinks = spdlog::default_logger()->sinks();
    size_t old_size = sinks.size();
    sinks.push_back(mem);

    bool result = loader.HelloWorldSmokeTest("this_file_does_not_exist.game");
    EXPECT_FALSE(result);

    bool found_error = false;
    spdlog::default_logger()->flush();
    for (auto &s : mem->logs) {
        if (s.find("HelloWorldSmokeTest error") != std::string::npos || s.find("Cannot open file") != std::string::npos) {
            found_error = true; break;
        }
    }
    EXPECT_TRUE(found_error) << "Expected HelloWorldSmokeTest to log an error when file missing";

    sinks.resize(old_size);
}
TEST_F(GameSpecLoaderTest, HelloWorldSmokeTestParsedOk) {
    namespace fs = std::filesystem;
    auto mem = std::make_shared<MemorySink>();
    auto &sinks = spdlog::default_logger()->sinks();
    size_t old_size = sinks.size();
    sinks.push_back(mem);

    // create temp file with a minimal valid config
    auto stamp = std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    fs::path tmp = fs::temp_directory_path() / ("hello_test_" + stamp + ".game");
    std::ofstream out(tmp);
    out << "configuration {\n  name: \"Tmp Hello\"\n  player range: (1, 2)\n  audience: false\n  setup: { }\n}\n";
    out.close();

    bool result = loader.HelloWorldSmokeTest(tmp.string().c_str());
    EXPECT_TRUE(result);

    spdlog::default_logger()->flush();
    bool found_parsed = false;
    bool found_snippet = false;
    for (auto &s : mem->logs) {
        if (s.find("Parsed OK") != std::string::npos) found_parsed = true;
        if (s.find("Configuration snippet") != std::string::npos) found_snippet = true;
    }
    EXPECT_TRUE(found_parsed) << "Expected 'Parsed OK' log";
    EXPECT_TRUE(found_snippet) << "Expected 'Configuration snippet' log";

    sinks.resize(old_size);
    std::error_code ec;
    fs::remove(tmp, ec);
}

TEST_F(GameSpecLoaderTest, LoadFileNotFoundThrows) {
    EXPECT_THROW(loader.loadFile("this_file_definitely_does_not_exist_12345.game"), std::runtime_error);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
