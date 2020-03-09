#pragma once

#include "shared/types.hpp"
#include <Scene/BsSceneObject.h>
#include <vector>

namespace alflib {
class RawMemoryWriter;
class RawMemoryReader;
} // namespace alflib

namespace wind {

struct BaseFn;

// ============================================================ //

void writeBall(alflib::RawMemoryWriter &mw);

// ============================================================ //

bool writeWindSource(alflib::RawMemoryWriter *mw,
                     const std::vector<BaseFn> &fns);

bool writeRigidbody(alflib::RawMemoryWriter *mw, f32 restitution, f32 mass);

bool writeRotor(alflib::RawMemoryWriter *mw);

void readComponent(alflib::RawMemoryReader &mr, bs::HSceneObject &so);

} // namespace wind
