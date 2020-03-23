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

#include "dlog.hpp"
#if defined(_MSC_VER)
#define MEAN_AND_LEAN
#define NO_MIN_MAX
#include <Windows.h>
#endif
#include <vector>
#ifdef DLOG_LOG_TO_FILE
#include <chrono>
#include <ctime>
#include <dutil/assert.hpp>
#include <dutil/file.hpp>
#endif
#include <cstdio>

namespace dlog {

void Flush() { fflush(stdout); }

static inline void FixConsole() {
#if defined(_MSC_VER)
  // Set console encoding
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);

  // Enable virtual terminal processing
  const HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD mode;
  GetConsoleMode(out, &mode);
  SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}

void Init() { FixConsole(); }

void Demo() {
  Raw("== dlog demo ==\n");

  Raw("\n== how levels work ==");
  std::vector<dlog::Level> levels;
  levels.push_back(Level::kVerbose);
  levels.push_back(Level::kInfo);
  levels.push_back(Level::kWarning);
  levels.push_back(Level::kError);
  for (auto level : levels) {
    Raw("\n= level {} =\n", LevelToString(level));
    SetLevel(level);
    Verbose("verbose, char [{}]", 'X');
    Info("info, str [{}]", "im a string!");
    Warning("warning, float [{}]", 3.14f);
    Error("error, int [{}]", 20);
  }

  Raw("\n== log with tag ==\n");
  SetLevel(Level::kVerbose);
  dlog::TagVerbose("char [{}]", "demo", 'X');
  dlog::TagInfo("str [{}]", "demo", "im a string!");
  dlog::TagWarning("float [{}]", "demo", 3.14f);
  dlog::TagError("int [{}]", "demo", 20);

  Raw("\n== all colors ==\n");
  SetLevel(Level::kVerbose);
  Raw(Paint(Color::kGray, "gray\n"));
  Raw(Paint(Color::kBrightRed, "bright_red\n"));
  Raw(Paint(Color::kBrightGreen, "bright_green\n"));
  Raw(Paint(Color::kBrightYellow, "bright_yellow\n"));
  Raw(Paint(Color::kBrightBlue, "bright_blue\n"));
  Raw(Paint(Color::kMagenta, "magenta\n"));
  Raw(Paint(Color::kWhite, "white\n"));

  Raw(Paint(Color::kBlack, "black\n"));
  Raw(Paint(Color::kRed, "red\n"));
  Raw(Paint(Color::kGreen, "green\n"));
  Raw(Paint(Color::kYellow, "yellow\n"));
  Raw(Paint(Color::kDarkBlue, "dark_blue\n"));
  Raw(Paint(Color::kPurple, "purple\n"));
  Raw(Paint(Color::kBlue, "blue\n"));
  Raw(Paint(Color::kBrightGray, "bright_gray\n"));
}

// ============================================================ //

std::string LevelToString(Level level) {
  std::string str{};

  switch (level) {
  case Level::kVerbose: {
    str = "verbose";
    break;
  }
  case Level::kInfo: {
    str = "info";
    break;
  }
  case Level::kWarning: {
    str = "warning";
    break;
  }
  case Level::kError: {
    str = "error";
    break;
  }
  case Level::kNone: {
    str = "none";
    break;
  }
  }

  return str;
}

// ============================================================ //

class Dlog {
private:
  explicit Dlog(Level new_level);

public:
  ~Dlog();

  static void SetLevel(Level level);

  static Level GetLevel();

  static void WriteToFile(const std::string &string, Level log_level);

  static void SetLogCallback(void (*cb)(const std::string &log_msg,
                                        Level log_level, void *user_data),
                             void *user_data);

  DLOG_MT_MUTEX_FN

private:
  static Dlog &Instance();

  Level level_;
#ifdef DLOG_LOG_TO_FILE
  dutil::File log_file_{};
#endif
#ifdef DLOG_LOG_CALLBACK
  void (*cb_)(const std::string &log_msg, Level log_level, void *user_data);
  void *cb_user_data_;
#endif
  DLOG_MT_DECLARE_MUTEX
};

Dlog::Dlog(Level new_level) : level_(new_level) {
#ifdef DLOG_LOG_TO_FILE
  if (!log_file_.IsOpen()) {
    const auto now =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
#if defined(_WIN32)
    std::tm local_time_stack;
    std::tm *local_time = &local_time_stack;
    const errno_t error = localtime_s(&local_time_stack, &now);
#else
    const std::tm *local_time = std::localtime(&now);
#endif
    const std::string path_with_time{
        "log_" + ToString<4>(1900 + local_time->tm_year) + "-" +
        ToString<2>(1 + local_time->tm_mon) + "-" +
        ToString<2>(local_time->tm_mday) + "_" +
        ToString<2>(local_time->tm_hour) + ToString<2>(local_time->tm_min) +
        ToString<2>(local_time->tm_sec) + ".txt"};
    const std::string log_name{path_with_time};
    std::string log_dir = LogDirectory::Get();
    if (log_dir.size() > 0 && log_dir.back() != '/') {
      log_dir += "/";
    }
    log_dir += log_name;

    const auto result = log_file_.Open(log_dir, dutil::File::Mode::kAppend);
    if (result != dutil::File::Result::kSuccess) {
      std::string s{"Failed to open log file. NOTE: You must create the "
                    "director(y/ies) where the log file is: " +
                    log_dir};
      DUTIL_ASSERT(result == dutil::File::Result::kSuccess, s.c_str());
    }
  }
#endif
}

Dlog::~Dlog() {
#ifdef DLOG_LOG_TO_FILE
  log_file_.Close();
#endif
}

Dlog &Dlog::Instance() {
  static Dlog log{Level::kVerbose};
  return log;
}

void Dlog::SetLevel(Level level) {
  Dlog &log = Instance();
  log.level_ = level;
}

Level Dlog::GetLevel() {
  Dlog &log = Instance();
  return log.level_;
}

void Dlog::WriteToFile(const std::string &string, Level log_level) {
#ifdef DLOG_LOG_TO_FILE
  (void)log_level;
  Dlog &log = Instance();
  log.log_file_.Write(string);
#elif defined(DLOG_LOG_CALLBACK)
  Dlog &log = Instance();
  log.cb_(string, log_level, log.cb_user_data_);
#else
  (void)string;
  (void)log_level;
  DLOG_WARNING(
      "attempting to log to file, but DLOG_LOG_TO_FILE or DLOG_LOG_CALLBACK"
      "is not set, rendering this call a noop");
#endif
}

void Dlog::SetLogCallback(void (*cb)(const std::string &log_msg,
                                     Level log_level, void *user_data),
                          void *user_data) {
#ifdef DLOG_LOG_CALLBACK
  DLOG_MT_LOCK;
  Dlog &log = Instance();
  log.cb_ = cb;
  log.cb_user_data_ = user_data;
#else
  (void)cb;
  (void)user_data;
  DLOG_WARNING(
      "attemting to set log callback, buf DLOG_LOG_CALLBACK is not defined");
#endif
}

// ============================================================ //

void SetLevel(Level level) { Dlog::SetLevel(level); }

Level GetLevel() { return Dlog::GetLevel(); }

void LogToFile(const std::string &string, Level log_level) {
  Dlog::WriteToFile(string, log_level);
}

void SetLogCallback(void (*cb)(const std::string &log_msg, Level log_level,
                               void *user_data),
                    void *user_data) {
  Dlog::SetLogCallback(cb, user_data);
}

DLOG_MT_MUTEX_F_CPP

// ============================================================ //
// Log directory
// ============================================================ //

void LogDirectory::Set(const std::string &new_log_directory) {
  LogDirectory &log_dir = Instance();
  log_dir.path_ = new_log_directory;
}

std::string LogDirectory::Get() {
  LogDirectory &log_dir = Instance();
  std::string &path = log_dir.path();
  if (path.size() > 0 && path.at(path.size() - 1) == '/') {
    path.resize(path.size() - 1);
  }

  return log_dir.path_;
}

LogDirectory &LogDirectory::Instance() {
  static LogDirectory log_dir{};
  return log_dir;
}

} // namespace dlog
