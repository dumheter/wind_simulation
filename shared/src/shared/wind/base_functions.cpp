#include "base_functions.hpp"

#include "shared/scene/builder.hpp"
#include "shared/utility/bsprinter.hpp"
#include "shared/utility/json_util.hpp"

#include <ThirdParty/json.hpp>
#include <alflib/memory/raw_memory_reader.hpp>
#include <alflib/memory/raw_memory_writer.hpp>
#include <dlog/dlog.hpp>

#include "shared/utility/util.hpp"

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

Vec3F SplineBase::operator()(const Vec3F point) const {
  return getForce(findClosestPoint(point).idx);
}

bool SplineBase::ToBytes(alflib::RawMemoryWriter &mw) const {
  mw.Write(static_cast<u32>(points.size()));
  mw.Write(static_cast<u32>(forces.size()));
  for (const auto point : points) {
    mw.Write(point.x);
    mw.Write(point.y);
    mw.Write(point.z);
  }
  for (const auto force : forces) {
    mw.Write(force);
    mw.Write(force);
    mw.Write(force);
  }
  mw.Write(degree);
  return mw.Write(samples);
}

SplineBase SplineBase::FromBytes(alflib::RawMemoryReader &mr) {
  SplineBase s{};
  const auto pcount = mr.Read<u32>();
  const auto fcount = mr.Read<u32>();
  s.points.reserve(pcount);
  s.forces.reserve(fcount);
  for (u32 i = 0; i < pcount; ++i) {
    s.points.emplace_back(mr.Read<f32>(), mr.Read<f32>(), mr.Read<f32>());
  }
  for (u32 i = 0; i < fcount; ++i) {
    s.forces.emplace_back(mr.Read<f32>());
  }
  s.degree = mr.Read<decltype(s.degree)>();
  s.samples = mr.Read<decltype(s.samples)>();
  return s;
}

void SplineBase::toJson(nlohmann::json &value) const {
  std::vector<nlohmann::json> jpoints{};
  for (const auto point : points) {
    nlohmann::json jpoint{};
    JsonUtil::setValue(jpoint, "point", point);
    jpoints.push_back(std::move(jpoint));
  }
  value["points"] = jpoints;
  value["forces"] = forces;
  value["degree"] = degree;
  value["samples"] = samples;
}

SplineBase SplineBase::fromJson(const nlohmann::json &value) {
  SplineBase s{};
  s.degree = value.value("degree", 2);
  s.samples = value.value("samples", ObjectBuilder::kSplineSamplesAuto);
  const nlohmann::json jpoints = value["points"];
  for (const auto jpoint : jpoints) {
    s.points.push_back(JsonUtil::getVec3F(jpoint, "point", Vec3F::ZERO));
  }

  const auto forcesIt = value.find("forces");
  if (forcesIt == value.end()) {
    DLOG_WARNING("Scene file is probably old and missing spline forces. Please "
                 "update with these!");
    for (size_t i = 0; i < s.points.size(); i++) {
      s.forces.push_back(1.0f);
    }
  } else {
    const nlohmann::json jforces = value["forces"];
    for (const auto force : jforces) {
      s.forces.push_back(force);
    }
  }

  return s;
}

SplineBase::ClosestPoint SplineBase::findClosestPoint(Vec3F point) const {
  u32 index = 0;
  f32 minDist = distance(points[0], point);
  for (u32 i = 1; i < points.size(); ++i) {
    if (f32 d = distance(points[i], point); d < minDist) {
      minDist = d;
      index = i;
    }
  }
  return {index, minDist};
}

Vec3F SplineBase::getForce(u32 index) const {
  // b can be the next point, an inperpolated point or first point wrapped
  // around.
  const Vec3F a = points[index];
  Vec3F b;
  if (index + 1 != points.size()) {
    b = points[index + 1];
  } else {
    if (distance(points.front(), points.back()) > 0.1f) {
      b = a + (a - points[index - 1]);
    } else {
      b = points.front();
    }
  }

  Vec3F dir = b - a;
  dir.normalize();
  const Vec3F force = dir * forces[index];
  // DLOG_INFO("force {} @ {} -> {}", force, a, b);
  if (std::abs(force.x) > 100.0f || std::abs(force.y) > 100.0f ||
      std::abs(force.z) > 100.0f) {
    int a = 0;
  }

  return force;
}

Vec3F Spline::operator()(const Vec3F point) const {
  struct SplineMeta {
    SplineBase::ClosestPoint cp;
    Vec3F force;
    f32 g;
  };
  std::vector<SplineMeta> splineMeta;
  splineMeta.reserve(splines.size());

  u32 closestSplineIdx = 0;
  f32 closestSplineDist = std::numeric_limits<f32>::max();

  for (u32 i = 0; i < splines.size(); ++i) {
    SplineMeta meta = {splines[i].findClosestPoint(point), Vec3F::ZERO, 0.0f};
    meta.force = splines[i].getForce(meta.cp.idx);
    if (meta.cp.dist < closestSplineDist) {
      closestSplineIdx = i;
      closestSplineDist = meta.cp.dist;
    }
    splineMeta.push_back(std::move(meta));
  }

  f32 gSum = 0.0f;
  for (u32 i = 0; i < splines.size(); ++i) {
    splineMeta[i].g =
        gaussian(splineMeta[i].cp.dist, 1.0f, 0.0f, closestSplineDist);
    gSum += splineMeta[i].g;
  }

  Vec3F force = Vec3F::ZERO;
  // DLOG_INFO("-----------");
  for (const auto &meta : splineMeta) {
    force += meta.force * (meta.g / gSum);
    // DLOG_RAW("{:.2f}/{:.2f}\n", meta.g, gSum);
  }
  // DLOG_INFO("force: {}", force);
  return force;
}

bool Spline::ToBytes(alflib::RawMemoryWriter &mw) const {
  mw.Write(static_cast<std::underlying_type<Type>::type>(Type::kSpline));
  bool res = mw.Write(static_cast<u32>(splines.size()));
  for (const auto spline : splines) {
    res = mw.Write(spline);
  }
  return res;
}

Spline Spline::FromBytes(alflib::RawMemoryReader &mr) {
  Spline s{};
  const auto count = mr.Read<u32>();
  s.splines.reserve(count);
  for (u32 i = 0; i < count; ++i) {
    s.splines.push_back(mr.Read<SplineBase>());
  }
  return s;
}

void Spline::toJson(nlohmann::json &value) const {
  std::vector<nlohmann::json> jsplines{};
  for (const auto spline : splines) {
    nlohmann::json jspline{};
    spline.toJson(jspline);
    jsplines.push_back(std::move(jspline));
  }
  value["splines"] = jsplines;
}

Spline Spline::fromJson(const nlohmann::json &value) {
  Spline s{};
  const nlohmann::json jsplines = value["splines"];
  for (const auto jspline : jsplines) {
    s.splines.push_back(SplineBase::fromJson(jspline));
  }
  return s;
}

} // namespace baseFunctions

// ============================================================ //

BaseFn BaseFn::fromJson(const nlohmann::json &value) {
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
  default: { return BaseFn{Constant::fromJson(value)}; }
  }
}

void BaseFn::toJson(nlohmann::json &value) const {
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
  default: { fn = BaseFn{Constant::FromBytes(mr)}; }
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
