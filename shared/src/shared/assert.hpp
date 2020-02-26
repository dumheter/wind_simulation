#pragma once

#include <assert.h>

#define windAssert(condition, message) assert(!!(condition) && message)
