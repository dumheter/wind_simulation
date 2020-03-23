/**
 * MIT License
 *
 * Copyright (c) 2019 Christoffer Gustafsson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef DLOG_LOG_HPP_
#define DLOG_LOG_HPP_

/**
 * Quick Example:
 *   int main() {
 *     DLOG_INIT();
 *     DLOG_SET_LEVEL(dlog::Level::kVerbose);
 *
 *     DLOG_INFO("This is my first log using dlog!");
 *     DLOG_WARNING("This is a warning, be aware.");
 *     DLOG_ERROR("Some error could have happened!");
 *     DLOG_VERBOSE("Lowest level of log").
 *
 *     DLOG_INFO("using fmt's {}.", "formatting");
 *   }
 *
 * Init:
 *   You should call DLOG_INIT() at the beginning of your program,
 *   this is only required when targeting the Windows platform, but is
 *   good practice anyway.
 *
 * Set level:
 *   You can set the verbosity level with DLOG_SET_LEVEL(...), the available
 *   levels are:
 *      dlog::Level::kVerbose
 *      dlog::Level::kInfo
 *      dlog::Level::kWarning
 *      dlog::Level::kError
 *
 * Log with color:
 *   std::string name{"Christoffer Gustafsson"};
 *   DLOG_INFO("I, {}, made this for you!",
 *             dlog::Paint(dlog::Color::kBrightGreen, name));
 *
 * Set log directory:
 *   DLOG_SET_LOG_DIRECTORY("my/log/directory/");
 *   Note, it will not create directories
 *
 * Log without macro:
 *   dlog::Info(...);
 *   or with tag:
 *   dlog::TagInfo(...);
 *
 * Formatting:
 *   The underlying fmt::format is exposed via dlog::Format(...) -> std::string.
 *   Example:
 *     std::string value = dlog::Format("value: {:.2f}", 13.37f);
 *
 * Setupt a log callback:
 *   Call this on startup, before using any other log functions.
 *   dlog::SetLogCallback(MyCbFn, (void*)&my_user_data);
 *
 * Settings:
 *   Here are a few settings you can enable by defining them at compile time.
 *   DLOG_MT - To log from multiple threads, define DLOG_MT.
 *   DLOG_TIMESTAMP - Make every log call be prepended with a timestamp
 *   formatted like this, [hh:mm:ss].
 *   DLOG_DATESTAMP - Make every log call be prepended with a datestamp
 *   formatted like this, [yyyy-mm-dd].
 *   DLOG_FILESTAMP - Have the filename and line number of the log call appear.
 *   DLOG_LOG_TO_FILE - Log to file.
 *   DLOG_LOG_CALLBACK - On each log, have a callback function get the log
 * message. DLOG_DISSABLE - Will remove all code generated from the macros, such
 * as DLOG_INFO(...) and DLOG_INIT(). Useful if you only want to log in debug
 * builds.
 *
 * Why Macro?:
 *   Use the macros, it will allow you to remove all log code by
 *   defining DLOG_DISSABLE at compile time.
 */

#include <cstdio>
#include <fmt/format.h>
#include <string>
#ifdef DLOG_MT
#include <mutex>
#endif
#if defined(DLOG_TIMESTAMP) || defined(DLOG_DATESTAMP)
#include <chrono>
#endif
#if defined(DLOG_FILESTAMP)
#include <cstring>
#endif

// ============================================================ //
// Macros
// ============================================================ //

#ifndef DLOG_DISSABLE
#define DLOG_MACRO_ENABLE
#endif

#if defined(DLOG_MACRO_ENABLE)
// print with function tag
#if defined(DLOG_FILESTAMP)
/**
 * make this: "/path/to/file.x" into this: "file.x"
 */
#if defined(_WIN32)
#define DLOG_FILENAME                                                          \
  (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define DLOG_FILENAME                                                          \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif
#define DLOG_FILE_LINE_FUNC                                                    \
  dlog::Format("{} @ {}:{}", __func__, DLOG_FILENAME, __LINE__)
#define DLOG_VERBOSE(...) dlog::TagVerbose(DLOG_FILE_LINE_FUNC, __VA_ARGS__)
#define DLOG_INFO(...) dlog::TagInfo(DLOG_FILE_LINE_FUNC, __VA_ARGS__)
#define DLOG_WARNING(...) dlog::TagWarning(DLOG_FILE_LINE_FUNC, __VA_ARGS__)
#define DLOG_ERROR(...) dlog::TagError(DLOG_FILE_LINE_FUNC, __VA_ARGS__)
#define DLOG_RAW(...) dlog::Raw(__VA_ARGS__)
#else
#define DLOG_VERBOSE(...) dlog::TagVerbose(__func__, __VA_ARGS__)
#define DLOG_INFO(...) dlog::TagInfo(__func__, __VA_ARGS__)
#define DLOG_WARNING(...) dlog::TagWarning(__func__, __VA_ARGS__)
#define DLOG_ERROR(...) dlog::TagError(__func__, __VA_ARGS__)
#define DLOG_RAW(...) dlog::Raw(__VA_ARGS__)
#endif
// misc
#define DLOG_INIT() dlog::Init()
#define DLOG_SET_LEVEL(level) dlog::SetLevel(level)
#define DLOG_SET_LOG_PATH(path) dlog::LogDirectory::Set(path)
#else
#define DLOG_VERBOSE(...)
#define DLOG_INFO(...)
#define DLOG_WARNING(...)
#define DLOG_ERROR(...)
#define DLOG_RAW(...)
#define DLOG_INIT()
#define DLOG_SET_LEVEL(level)
#define DLOG_SET_LOG_PATH(path)
#endif

// try to use snprintf_s over snprintf
#ifdef __STDC_LIB_EXT1__
#define __STDC_WANT_LIB_EXT1__ 1
#define DLOG_SPRINTF(buffer, size, format, ...)                                \
  snprintf_s(buffer, size, format, __VA_ARGS__)
#else
#define DLOG_SPRINTF(buffer, size, format, ...)                                \
  snprintf(buffer, size, format, __VA_ARGS__)
#endif

// ============================================================ //
// Thread Safe logging
// ============================================================ //

#ifdef DLOG_MT
#define DLOG_MT_DECLARE_MUTEX                                                  \
  std::shared_ptr<std::mutex> mutex_ = std::make_shared<std::mutex>();
#define DLOG_MT_MUTEX_FN                                                       \
  static std::shared_ptr<std::mutex> mutex() {                                 \
    Dlog &log = Instance();                                                    \
    return log.mutex_;                                                         \
  }
#define DLOG_MT_MUTEX_F_CPP                                                    \
  std::shared_ptr<std::mutex> get_mutex() { return Dlog::mutex(); }
#define DLOG_MT_MUTEX_F_HPP std::shared_ptr<std::mutex> get_mutex();
#define DLOG_MT_LOCK std::lock_guard<std::mutex> lock(*get_mutex());
#else
#define DLOG_MT_DECLARE_MUTEX
#define DLOG_MT_MUTEX_FN
#define DLOG_MT_MUTEX_F_CPP
#define DLOG_MT_MUTEX_F_HPP
#define DLOG_MT_LOCK
#endif

namespace dlog {

// ============================================================ //
// Settings
// ============================================================ //
// Set flags according to library user's settings.

#ifdef DLOG_TIMESTAMP
constexpr bool kTimestamp = true;
#else
constexpr bool kTimestamp = false;
#endif

#ifdef DLOG_DATESTAMP
constexpr bool kDatestamp = true;
#else
constexpr bool kDatestamp = false;
#endif

#if defined(DLOG_LOG_TO_FILE) || defined(DLOG_LOG_CALLBACK)
constexpr bool kSideLog = true;
#else
constexpr bool kSideLog = false;
#endif

#ifdef DLOG_FILESTAMP
constexpr bool kFilestamp = true;
#else
constexpr bool kFilestamp = false;
#endif

// ============================================================ //
// Misc
// ============================================================ //

/**
 * For now, only used on windows platform where it changes the
 * console to use utf8 encoding.
 */
void Init();

/**
 * Show off the log features.
 */
void Demo();

// ============================================================ //

/**
 * Integer to string, formatted with zeros.
 * Examples:
 * ToString<2>(4) -> "04"
 * ToString<4>(101) -> "0101"
 */
template <std::size_t char_count> inline std::string ToString(int value);
template <> inline std::string ToString<2>(int value) {
  std::string str{};
  str.resize(2);
  DLOG_SPRINTF(str.data(), str.capacity(), "%.2i", value);
  return str;
}
template <> inline std::string ToString<4>(int value) {
  std::string str{};
  str.resize(4);
  DLOG_SPRINTF(str.data(), str.capacity(), "%.4i", value);
  return str;
}

inline std::string MakeTimestamp() {
#ifdef DLOG_TIMESTAMP
  const auto now =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
#if defined(_WIN32)
  tm local_time_win;
  localtime_s(&local_time_win, &now);
  tm *local_time = &local_time_win;
#else
  const auto local_time = std::localtime(&now);
#endif
  const std::string formatted_time{dlog::ToString<2>(local_time->tm_hour) +
                                   ":" + dlog::ToString<2>(local_time->tm_min) +
                                   ":" + dlog::ToString<2>(local_time->tm_sec)};
  return formatted_time;
#else
  return "dlog internal error";
#endif
}

inline std::string MakeDatestamp() {
#ifdef DLOG_DATESTAMP
  const auto now =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
#if defined(_WIN32)
  tm local_time_win;
  localtime_s(&local_time_win, &now);
  tm *local_time = &local_time_win;
#else
  const auto local_time = std::localtime(&now);
#endif

  const std::string formatted_time{
      dlog::ToString<4>(1900 + local_time->tm_year) + "-" +
      dlog::ToString<2>(1 + local_time->tm_mon) + "-" +
      dlog::ToString<2>(local_time->tm_mday)};
  return formatted_time;
#else
  return "dlog internal error";
#endif
}

// ============================================================ //
// Level
// ============================================================ //

enum class Level : int {
  kVerbose = 0,
  kInfo = 1,
  kWarning = 2,
  kError = 3,
  kNone = 4
};

void SetLevel(Level level);

Level GetLevel();

std::string LevelToString(Level level);

DLOG_MT_MUTEX_F_HPP

// ============================================================ //
// Log to file
// ============================================================ //

void LogToFile(const std::string &string, Level level);

// ============================================================ //
// Log callback
// ============================================================ //

/**
 * Set a callback, containing the content of the log message, with any set user
 * data.
 */
void SetLogCallback(void (*cb)(const std::string &log_msg, Level log_level,
                               void *user_data),
                    void *user_data);

// ============================================================ //
// Color
// ============================================================ //

/**
 * Note, it's common to "theme" the terminal, changing the presented color of
 * these following colors. Thus, the color "kYellow" might appear as some
 * other color, for some users.
 */
using ColorType = int;
enum class Color : ColorType {
  kGray = 90,
  kBrightRed = 91,
  kBrightGreen = 92,
  kBrightYellow = 93,
  kBrightBlue = 94,
  kMagenta = 95,
  kTeal = 96,
  kWhite = 97,

  kBlack = 30,
  kRed = 31,
  kGreen = 32,
  kYellow = 33,
  kDarkBlue = 34,
  kPurple = 35,
  kBlue = 36,
  kBrightGray = 37
};

template <typename T> inline std::string Paint(Color color, T item) {
  std::string out =
      fmt::format("\033[{}m{}\033[0m", static_cast<ColorType>(color), item);
  return out;
}

// TODO have themes
constexpr Color kVerboseColor = Color::kGray;
constexpr Color kInfoColor = Color::kBlue;
constexpr Color kWarningColor = Color::kYellow;
constexpr Color kErrorColor = Color::kRed;
constexpr Color kTimestampColor = Color::kBrightYellow;
constexpr Color kDatestampColor = Color::kBrightYellow;
constexpr Color kTagColor = Color::kPurple;

// ============================================================ //
// Log functions
// ============================================================ //

/**
 * Force any unwritten data in stdout buffer to be written.
 */
void Flush();

template <typename TString, typename... Args>
inline void Generic(Level level, const Color level_color, const TString &format,
                    Args... args) {
  DLOG_MT_LOCK;

  const std::string level_str = LevelToString(level);

  if constexpr (dlog::kDatestamp) {
    const std::string datestamp = MakeDatestamp();
    fmt::print("[{:<19}] ", Paint(kDatestampColor, datestamp));
  }
  if constexpr (dlog::kTimestamp) {
    const std::string timestamp = dlog::MakeTimestamp();
    fmt::print("[{:<17}] ", Paint(kTimestampColor, timestamp));
  }
  fmt::print("{:<18} ", "[" + Paint(level_color, level_str) + "]");
  fmt::print(format, args...);
  fmt::print("\n");

  if constexpr (kSideLog) {
    const std::string a = [&]() {
      if constexpr (kTimestamp && kDatestamp) {
        const std::string timestamp = MakeTimestamp();
        const std::string datestamp = MakeDatestamp();
        return fmt::format("[{:<10}] [{:<8}] {:<9} ", datestamp, timestamp,
                           "[" + level_str + "]");
      } else if constexpr (kTimestamp) {
        const std::string timestamp = MakeTimestamp();
        return fmt::format("[{:<8}] {:<9} ", timestamp, "[" + level_str + "]");
      } else if constexpr (kDatestamp) {
        const std::string datestamp = MakeDatestamp();
        return fmt::format("[{:<10}] {:<9} ", datestamp, "[" + level_str + "]");
      } else {
        return fmt::format("{:<9} ", "[" + level_str + "]");
      }
    }();
    const std::string b = fmt::format(format, args...);
    const std::string c = "\n";
    LogToFile(a + b + c, level);
  }
}

/**
 * @tparam TString should be std::string or follow its 'api'.
 */
template <typename TString, typename... Args>
inline void Verbose(const TString &format, Args... args) {
  const Level level = GetLevel();
  if (level <= Level::kVerbose) {
    Generic(Level::kVerbose, kVerboseColor, format, args...);
  }
}

template <typename TString, typename... Args>
inline void Info(const TString &format, Args... args) {
  const Level level = GetLevel();
  if (level <= Level::kInfo) {
    Generic(Level::kInfo, kInfoColor, format, args...);
  }
}

template <typename TString, typename... Args>
inline void Warning(const TString &format, Args... args) {
  const Level level = GetLevel();
  if (level <= Level::kWarning) {
    Generic(Level::kWarning, kWarningColor, format, args...);
  }
}
template <typename TString, typename... Args>
inline void Error(const TString &format, Args... args) {
  const Level level = GetLevel();
  if (level <= Level::kError) {
    Generic(Level::kError, kErrorColor, format, args...);
  }
}

template <typename TString, typename... Args>
inline void Raw(const TString &format, Args... args) {
  DLOG_MT_LOCK;
  fmt::print(format, args...);
  if constexpr (kSideLog) {
    LogToFile(fmt::format(format, args...), Level::kNone);
  }
}

template <typename TString, typename... Args>
inline std::string Format(const TString &format, Args... args) {
  return fmt::format(format, args...);
}

// ============================================================ //
// Log with Tag
// ============================================================ //

template <typename TString, typename... Args>
inline void TagGeneric(Level level, const Color level_color,
                       const std::string &tag, const TString &format,
                       Args... args) {
  DLOG_MT_LOCK;

  const std::string level_str = LevelToString(level);

  if constexpr (kDatestamp) {
    const std::string datestamp = MakeDatestamp();
    fmt::print("[{:<19}] ", Paint(kDatestampColor, datestamp));
  }
  if constexpr (kTimestamp) {
    const std::string timestamp = MakeTimestamp();
    fmt::print("[{:<17}] ", Paint(kTimestampColor, timestamp));
  }

  if constexpr (kFilestamp) {
    fmt::print("{:<18} {:<45} ", "[" + Paint(level_color, level_str) + "]",
               "[" + Paint(kTagColor, tag) + "]");
  } else {
    fmt::print("{:<18} {:<25} ", "[" + Paint(level_color, level_str) + "]",
               "[" + Paint(kTagColor, tag) + "]");
  }
  fmt::print(format, args...);
  fmt::print("\n");

  if constexpr (kSideLog) {
    // no color for side logging
    const std::string a = [&]() {
      if constexpr (kTimestamp && kDatestamp) {
        const std::string timestamp = MakeTimestamp();
        const std::string datestamp = MakeDatestamp();
        return fmt::format("[{:<10}] [{:<8}] {:<9} {:<16} ", datestamp,
                           timestamp, "[" + level_str + "]", "[" + tag + "]");
      } else if constexpr (kTimestamp) {
        const std::string timestamp = MakeTimestamp();
        return fmt::format("[{:<8}] {:<9} {:<16} ", timestamp,
                           "[" + level_str + "]", "[" + tag + "]");
      } else if constexpr (kDatestamp) {
        const std::string datestamp = MakeDatestamp();
        return fmt::format("[{:<10}] {:<9} {:<16} ", datestamp,
                           "[" + level_str + "]", "[" + tag + "]");
      } else {
        return fmt::format("{:<9} {:<16} ", "[" + level_str + "]",
                           "[" + tag + "]");
      }
    }();
    const std::string b = fmt::format(format, args...);
    const std::string c = "\n";
    LogToFile(a + b + c, level);
  }
}

/**
 * @tparam TString should be std::string or follow its 'api'.
 */
template <typename TString, typename... Args>
inline void TagVerbose(const std::string &tag, const TString &format,
                       Args... args) {
  const Level level = GetLevel();
  if (level <= Level::kVerbose) {
    TagGeneric(Level::kVerbose, kVerboseColor, tag, format, args...);
  }
}

template <typename TString, typename... Args>
inline void TagInfo(const std::string &tag, const TString &format,
                    Args... args) {
  const Level level = GetLevel();
  if (level <= Level::kInfo) {
    TagGeneric(Level::kInfo, kInfoColor, tag, format, args...);
  }
}

template <typename TString, typename... Args>
inline void TagWarning(const std::string &tag, const TString &format,
                       Args... args) {
  const Level level = GetLevel();
  if (level <= Level::kWarning) {
    TagGeneric(Level::kWarning, kWarningColor, tag, format, args...);
  }
}

template <typename TString, typename... Args>
inline void TagError(const std::string &tag, const TString &format,
                     Args... args) {
  const Level level = GetLevel();
  if (level <= Level::kError) {
    TagGeneric(Level::kError, kErrorColor, tag, format, args...);
  }
}

// ============================================================ //
// Log directory
// ============================================================ //

/**
 * Singleton log path storage class. Used by dlog once at construction.
 * Make sure to call Set before dlog is constructed, else it will have
 * no effect.
 */
class LogDirectory {
public:
  static void Set(const std::string &new_log_directory);

  static std::string Get();

private:
  std::string path_{};

  LogDirectory() = default;
  LogDirectory(const LogDirectory &other) = delete;
  LogDirectory &operator=(const LogDirectory &other) = delete;

  static LogDirectory &Instance();

  const std::string &path() const { return path_; }
  std::string &path() { return path_; }
};

} // namespace dlog

#endif // DLOG_LOG_HPP_
