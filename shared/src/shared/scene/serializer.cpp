#include "serializer.hpp"
#include "shared/log.hpp"
#include "shared/scene/types.hpp"
#include "shared/wind/base_functions.hpp"
#include <BsCorePrerequisites.h>
#include <Components\BsCRigidbody.h>
#include <alflib/memory/raw_memory_reader.hpp>
#include <alflib/memory/raw_memory_writer.hpp>

namespace wind {

bool writeWindSource(alflib::RawMemoryWriter *mw,
                     const std::vector<BaseFn> &fns) {
  mw->Write(static_cast<std::underlying_type<ComponentType>::type>(
      ComponentType::kWind));
  bool res = mw->Write(fns.size());
  for (auto &fn : fns) {
    res = mw->Write(fn);
  }
  return res;
}

bool writeRigidbody(alflib::RawMemoryWriter *mw, f32 restitution, f32 mass) {
  mw->Write(static_cast<std::underlying_type<ComponentType>::type>(
      ComponentType::kRigidbody));
  mw->Write(restitution);
  return mw->Write(mass);
}

bool writeRotor(alflib::RawMemoryWriter *mw) { return false; }

void readComponent(alflib::RawMemoryReader &mr, bs::HSceneObject &so) {
  const auto type = static_cast<ComponentType>(
      mr.Read<std::underlying_type<ComponentType>::type>());
  if (type == ComponentType::kRigidbody) {
    const auto restitiution = mr.Read<f32>();
    const auto mass = mr.Read<f32>();
    auto rigid = so->addComponent<bs::CRigidbody>();
    // HPhysicsMaterial physicsMaterial =
    //     PhysicsMaterial::create(1.0f, 1.0f, restitution);
    // rigid->setPhysicsMaterial(physicsMaterial);
    // rigid->setMass(mass);
  } else {
    logWarning("[serializer] could not read component");
  }
}

} // namespace wind
