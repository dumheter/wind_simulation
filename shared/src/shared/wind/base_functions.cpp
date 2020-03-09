#include "base_functions.hpp"
#include <alflib/memory/raw_memory_reader.hpp>
#include <alflib/memory/raw_memory_writer.hpp>

namespace wind {

namespace baseFunctions {

bool Constant::ToBytes(alflib::RawMemoryWriter &mw) const {
  mw.Write(static_cast<std::underlying_type<Type>::type>(Type::kConstant));
  mw.Write(dir.x);
  mw.Write(dir.y);
  mw.Write(dir.z);
  return mw.Write(magnitude);
}

Constant Constant::FromBytes(alflib::RawMemoryReader &mr) {
  Constant c{bs::Vector3{mr.Read<f32>(), mr.Read<f32>(), mr.Read<f32>()},
             mr.Read<f32>()};
  return c;
}

bool Spline::ToBytes(alflib::RawMemoryWriter &mw) const { return false; }
Spline Spline::FromBytes(alflib::RawMemoryReader &mr) { return Spline{}; }

} // namespace baseFunctions

// ============================================================ //

bool BaseFn::ToBytes(alflib::RawMemoryWriter &mw) const {
  return std::visit([&mw](auto &&arg) { return arg.ToBytes(mw); }, fn);
}

BaseFn BaseFn::FromBytes(alflib::RawMemoryReader &mr) {
  const auto type = static_cast<baseFunctions::Type>(
      mr.Read<std::underlying_type<baseFunctions::Type>::type>());
  BaseFn fn;
  switch (type) {
  case baseFunctions::Type::kConstant:
    fn = BaseFn{Constant::FromBytes(mr)};
    break;
  case baseFunctions::Type::kSpline:
    fn = BaseFn{Spline::FromBytes(mr)};
    break;
  }
  return fn;
}

String BaseFn::toString(baseFunctions::Type fn) const {
  switch (fn) {
  case baseFunctions::Type::kConstant:
    return "constant";
  case baseFunctions::Type::kSpline:
    return "spline";
  }
}

BaseFn BaseFn::fromString(const String &fn) {
  if (fn == "constant") {
    return BaseFn::fnConstant(Vec3F::ZERO, 100.0f);
  } else /*if (fn == "spline") */ {
    // return BaseFn::fn
    return BaseFn();
  }
}

} // namespace wind
