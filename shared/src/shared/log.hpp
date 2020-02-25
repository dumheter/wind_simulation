#pragma once

#include <BsApplication.h>
#include <Debug/BsDebug.h>
#include <fmt/format.h>
#include <string>

namespace wind {

void fixConsole();

template <typename... Args>
void logInfo(const bs::String &format, Args... args) {
  bs::gDebug().log(fmt::format(format, args...).c_str(),
                   bs::LogVerbosity::Info);
}
template <typename... Args>
void logAny(const bs::String &format, Args... args) {
  bs::gDebug().log(fmt::format(format, args...).c_str(), bs::LogVerbosity::Any);
}
template <typename... Args>
void logFatal(const bs::String &format, Args... args) {
  bs::gDebug().log(fmt::format(format, args...).c_str(),
                   bs::LogVerbosity::Fatal);
}
template <typename... Args>
void logError(const bs::String &format, Args... args) {
  bs::gDebug().log(fmt::format(format, args...).c_str(),
                   bs::LogVerbosity::Error);
}
template <typename... Args>
void logWarning(const bs::String &format, Args... args) {
  bs::gDebug().log(fmt::format(format, args...).c_str(),
                   bs::LogVerbosity::Warning);
}
template <typename... Args>
void logLog(const bs::String &format, Args... args) {
  bs::gDebug().log(fmt::format(format, args...).c_str(), bs::LogVerbosity::Log);
}
template <typename... Args>
void logVerbose(const bs::String &format, Args... args) {
  bs::gDebug().log(fmt::format(format, args...).c_str(),
                   bs::LogVerbosity::Verbose);
}
template <typename... Args>
void logVeryVerbose(const bs::String &format, Args... args) {
  bs::gDebug().log(fmt::format(format, args...).c_str(),
                   bs::LogVerbosity::VeryVerbose);
}

} // namespace wind
