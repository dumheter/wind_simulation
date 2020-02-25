#include "log.hpp"

#if defined(_MSC_VER)
#define MEAN_AND_LEAN
#define NO_MIN_MAX
#include <Windows.h>
#endif

namespace wind {

void fixConsole() {
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

} // namespace wind
