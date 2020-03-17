#include "base_functions.hpp"
#include <alflib/memory/raw_memory_reader.hpp>
#include <alflib/memory/raw_memory_writer.hpp>

#include "shared/utility/json_util.hpp"
#include <ThirdParty/json.hpp>

namespace wind {

namespace baseFunctions {

String typeToString(Type type) {
  switch (type) {
  case Type::kPolynomial:
    return "polynomial";
  case Type::kSpline:
    return "spline";
  case Type::kConstant:
    [[fallthrough]];
    default:
    return "constant";
  }
}

Type stringToType(const String &type) {
  if (type == "polynomial") {
    return Type::kPolynomial;
  }
  if (type == "spline") {
    return Type::kSpline;
  }
  return Type::kConstant;
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

void Polynomial::toJson(nlohmann::json &value) const {
  JsonUtil::setValue(value, "origo", origo);
  value["x0"] = x0;
  value["x1"] = x1;
  value["x2"] = x2;
  value["y0"] = y0;
  value["y1"] = y1;
  value["y2"] = y2;
  value["z0"] = z0;
  value["z1"] = z1;
  value["z2"] = z2;
}

Polynomial Polynomial::fromJson(const nlohmann::json &value) {
  const f32 x0 = value.value("x0", 0.0f);
  const f32 x1 = value.value("x1", 0.0f);
  const f32 x2 = value.value("x2", 0.0f);
  const f32 y0 = value.value("y0", 0.0f);
  const f32 y1 = value.value("y1", 0.0f);
  const f32 y2 = value.value("y2", 0.0f);
  const f32 z0 = value.value("z0", 0.0f);
  const f32 z1 = value.value("z1", 0.0f);
  const f32 z2 = value.value("z2", 0.0f);
  const Vec3F origo = JsonUtil::getVec3F(value, "origo", Vec3F::ZERO);
  return Polynomial{origo, x0, x1, x2, y0, y1, y2, z0, z1, z2};
}

bool Polynomial::ToBytes(alflib::RawMemoryWriter &mw) const {
  mw.Write(static_cast<std::underlying_type<Type>::type>(Type::kPolynomial));
  mw.Write(origo.x);
  mw.Write(origo.y);
  mw.Write(origo.z);
  mw.Write(x0);
  mw.Write(x1);
  mw.Write(x2);
  mw.Write(y0);
  mw.Write(y1);
  mw.Write(y2);
  mw.Write(z0);
  mw.Write(z1);
  return mw.Write(z2);
}

Polynomial Polynomial::FromBytes(alflib::RawMemoryReader &mr) {
  return Polynomial{bs::Vector3{mr.Read<f32>(), mr.Read<f32>(), mr.Read<f32>()},
                    mr.Read<f32>(),
                    mr.Read<f32>(),
                    mr.Read<f32>(),
                    mr.Read<f32>(),
                    mr.Read<f32>(),
                    mr.Read<f32>(),
                    mr.Read<f32>(),
                    mr.Read<f32>(),
                    mr.Read<f32>()};
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
  case baseFunctions::Type::kPolynomial: {
    return BaseFn{Polynomial::fromJson(value)};
  }
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
    case baseFunctions::Type::kPolynomial: {
      fn = BaseFn{Polynomial::FromBytes(mr)};
      break;
    }
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
  if (isType<Polynomial>()) {
    return baseFunctions::typeToString(baseFunctions::Type::kPolynomial);
  }
  if (isType<Spline>()) {
    return baseFunctions::typeToString(baseFunctions::Type::kSpline);
  }
  return baseFunctions::typeToString(baseFunctions::Type::kConstant);
}

} // namespace wind
