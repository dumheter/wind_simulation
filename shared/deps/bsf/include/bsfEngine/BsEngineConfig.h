#pragma once

#define BS_RENDERER_MODULE "bsfRenderBeast"
#define BS_RENDER_API_MODULE "bsfD3D11RenderAPI"
#define BS_AUDIO_MODULE "bsfOpenAudio"
#define BS_PHYSICS_MODULE "bsfPhysX"

/** Path to the framework root when files haven't been packaged yet (e.g. running from debugger). */
static constexpr const char* RAW_APP_ROOT = "C:/Projects/bsf/"; 

/** Secondary search path to use when looking for built-in assets (after the RAW_APP_ROOT). */
static constexpr const char* SECONDARY_APP_ROOT = "/"; 

/** Path to the binaries when files haven't been packaged yet (e.g. running from debugger). */
#if BS_CONFIG == BS_CONFIG_DEBUG
static constexpr const char* BINARIES_PATH = "C:/Projects/bsf/Build/bin/x64/Debug";
#elif BS_CONFIG == BS_CONFIG_RELWITHDEBINFO
static constexpr const char* BINARIES_PATH = "C:/Projects/bsf/Build/bin/x64/RelWithDebInfo";
#elif BS_CONFIG == BS_CONFIG_MINSIZEREL
static constexpr const char* BINARIES_PATH = "C:/Projects/bsf/Build/bin/x64/MinSizeRel";
#elif BS_CONFIG == BS_CONFIG_RELEASE
static constexpr const char* BINARIES_PATH = "C:/Projects/bsf/Build/bin/x64/Release";
#endif
