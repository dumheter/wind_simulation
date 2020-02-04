#pragma once

#define BS_VERSION_MAJOR 1
#define BS_VERSION_MINOR 1
#define BS_VERSION_PATCH 0
#define BS_CURRENT_RELEASE_YEAR 2019


#ifndef _STR
#define _STR(m_x) #m_x
/**
 * This macro converts the preprocessor defined macro into textual form.
 * @note Currently used internally for binary information in Win32 resource files
 */
#define _MKSTR(m_x) _STR(m_x)
#endif

#define BS_VERSION_STRING _MKSTR(BS_VERSION_MAJOR) "." _MKSTR(BS_VERSION_MINOR) "." _MKSTR(BS_VERSION_PATCH) ".0"

#define BS_IS_BANSHEE3D 0
