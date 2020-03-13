#include "base_functions.hpp"
#include <alflib/memory/raw_memory_reader.hpp>
#include <alflib/memory/raw_memory_writer.hpp>

#include "shared/utility/json_util.hpp"
#include <ThirdParty/json.hpp>

namespace wind {

namespace baseFunctions {

String typeToString(Type type) {
  switch (type) {
  case Type::kSpline:
    return "spline";
  case Type::kConstant:
    [[fallthrough]];
    default:
    return "constant";
  }
}

Type stringToType(const String &type) {
  if (type == "spline") {
    return Type::kSpline;
  } else /* if (type == "constant") */ {
    return Type::kConstant;
  }
}

// ============================================================ //

void Constant::toJson(nlohmann::json &value) const {
  value["magnitude"] = magnitude;
  JsonUtil::setValue(value, "direction", dir);
}

Constant Constant::fromJson(const nlohmann::json &value) {
  const f32 mag = value.value("magnitude", 0.0f);
  const Vec3F dir = JsonUtil::getVec3F(value, "direction", Vec3F::ZERO);
  return Constant{dir, mag};
}

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
void Spline::toJson(nlohmann::json &value) const {}
Spline Spline::fromJson(const nlohmann::json &value) { return Spline{}; }

} // namespace baseFunctions

// ============================================================ //

BaseFn BaseFn::fromJson(const nlohmann::json& value) {
  const String windTypeStr = JsonUtil::getString(value, "type", "constant");
  baseFunctions::Type windType = baseFunctions::stringToType(windTypeStr);

  switch (windType) {
  case baseFunctions::Type::kSpline: {
    return BaseFn{Spline::fromJson(value)};
  }
  case baseFunctions::Type::kConstant:
    [[fallthrough]];
  default: {
      return BaseFn{Constant::fromJson(value)};
    }
  }
}

void BaseFn::toJson(nlohmann::json& value) const {
  value["type"] = typeToString();
 std::visit([&value](auto &&f) { return f.toJson(value); }, fn);
}

bool BaseFn::ToBytes(alflib::RawMemoryWriter &mw) const {
  return std::visit([&mw](auto &&arg) { return arg.ToBytes(mw); }, fn);
}

BaseFn BaseFn::FromBytes(alflib::RawMemoryReader &mr) {
  const auto type = static_cast<baseFunctions::Type>(
      mr.Read<std::underlying_type<baseFunctions::Type>::type>());
  BaseFn fn;
  switch (type) {
    case baseFunctions::Type::kSpline: {
      fn = BaseFn{Spline::FromBytes(mr)};
      break;
    }
    case baseFunctions::Type::kConstant:
      [[fallthrough]];
    default: {
      fn = BaseFn{Constant::FromBytes(mr)};
    }
  }
  return fn;
}

String BaseFn::typeToString() const {
  if (isType<Spline>()) {
    return baseFunctions::typeToString(baseFunctions::Type::kSpline);
  }
  else /* if (isType<Constant>()) */ {
    return baseFunctions::typeToString(baseFunctions::Type::kConstant);
  }
}

} // namespace wind
