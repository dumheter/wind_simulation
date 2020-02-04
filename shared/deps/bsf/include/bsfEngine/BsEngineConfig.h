#pragma once

#define BS_RENDERER_MODULE "bsfRenderBeast"
#define BS_RENDER_API_MODULE "bsfVulkanRenderAPI"
#define BS_AUDIO_MODULE "bsfOpenAudio"
#define BS_PHYSICS_MODULE "bsfPhysX"
#define BS_SCRIPTING_ENABLED 0

/** Path to the framework source directory. */
static constexpr const char* RAW_APP_ROOT = "C:/Users/Filip/Desktop/BuildBSF/bsf/";

/** Path to the framework build directory. */
static constexpr const char* BUILD_APP_ROOT = "C:/Users/Filip/Desktop/BuildBSF/bsf/build/";

/** Path to the binaries when files haven't been packaged yet (e.g. running from debugger). */
#if BS_CONFIG == BS_CONFIG_DEBUG
static constexpr const char* BINARIES_PATH = "C:/Users/Filip/Desktop/BuildBSF/bsf/build/bin/x64/Debug";
#elif BS_CONFIG == BS_CONFIG_RELWITHDEBINFO
static constexpr const char* BINARIES_PATH = "C:/Users/Filip/Desktop/BuildBSF/bsf/build/bin/x64/RelWithDebInfo";
#elif BS_CONFIG == BS_CONFIG_MINSIZEREL
static constexpr const char* BINARIES_PATH = "C:/Users/Filip/Desktop/BuildBSF/bsf/build/bin/x64/MinSizeRel";
#elif BS_CONFIG == BS_CONFIG_RELEASE
static constexpr const char* BINARIES_PATH = "C:/Users/Filip/Desktop/BuildBSF/bsf/build/bin/x64/Release";
#endif

/** Banshee 3D related version stuff*/
#if BS_IS_BANSHEE3D
#define BS_B3D_VERSION_MAJOR 
#define BS_B3D_VERSION_MINOR 
#define BS_B3D_VERSION_PATCH 
#define BS_B3D_CURRENT_RELEASE_YEAR 
#define BS_B3D_VERSION_STRING _MKSTR(BS_B3D_VERSION_MAJOR) "." _MKSTR(BS_B3D_VERSION_MINOR) "." _MKSTR(BS_B3D_VERSION_PATCH) ".0"
#endif
