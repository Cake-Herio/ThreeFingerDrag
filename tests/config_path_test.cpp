#include <cassert>
#include <iostream>
#include "../ThreeFingerDrag/application.h"

Logger& Logger::GetInstance() { static Logger logger("unused"); return logger; }
Logger::Logger(const std::string&) {}
Logger::~Logger() = default;
void Logger::Debug(const std::string&) {}
void Logger::Error(const std::string& message) { throw std::runtime_error(message); }

int main() {
    _set_error_mode(_OUT_TO_STDERR);
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    auto config = GlobalConfig::GetInstance();
    assert(config->GetGestureSpeed() == 45.0);
    assert(config->GetCancellationDelayMs() == 1500);
    Application::CheckPortableMode();
    const auto directory = std::filesystem::u8path(Application::GetConfigurationFolderPath());
    assert(directory == Application::ExePath().parent_path());
    assert(directory != std::filesystem::current_path());
    Application::ReadConfiguration();
    assert(config->GetGestureSpeed() == 45.0);
    assert(config->GetCancellationDelayMs() == 1500);
    config->SetGestureSpeed(72.0);
    config->SetCancellationDelayMs(1700);
    Application::WriteConfiguration();
    config->SetGestureSpeed(1.0);
    config->SetCancellationDelayMs(100);
    Application::ReadConfiguration();
    assert(config->GetGestureSpeed() == 72.0);
    assert(config->GetCancellationDelayMs() == 1700);
    assert(Application::IsInitialStartup());
    assert(!Application::IsInitialStartup());
    std::cout << "PASS: defaults, EXE-relative config, Unicode path, INI save/reload, version marker\n";
}
