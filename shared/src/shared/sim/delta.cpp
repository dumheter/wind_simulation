// MIT License
//
// Copyright (c) 2020 Filip Björklund¨, Christoffer Gustafsson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "delta.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <dlog/dlog.hpp>
#include <vector>

#include "shared/math/math.hpp"
#include "shared/scene/component/cwind.hpp"

#include "thirdparty/dutil/file.hpp"

// ========================================================================== //
// DeltaField Implementation
// ========================================================================== //

namespace wind {

static void buildValueDifferenceDiagram(const VectorField *sim,
                                        const VectorField *baked,
                                        FieldBase::Dim dim,
                                        const std::string &path);

DeltaField::~DeltaField() {
  delete m_delta;
  delete m_sim;
  delete m_baked;
}

// -------------------------------------------------------------------------- //

void DeltaField::build(const HCSim &csim, const HCWind &cwind) {
  WindSimulation *sim = csim->getSim();
  const FieldBase::Dim dim = sim->getDim();
  m_delta =
      new VectorField(dim.width, dim.height, dim.depth, sim->getCellSize());
  m_sim = new VectorField(dim.width, dim.height, dim.depth, sim->getCellSize());
  m_baked =
      new VectorField(dim.width, dim.height, dim.depth, sim->getCellSize());

  // Construct delta
  for (u32 k = 0; k < dim.depth; k++) {
    for (u32 j = 0; j < dim.height; j++) {
      for (u32 i = 0; i < dim.width; i++) {
        Vec3F vSim = sim->V().get(i + 1, j + 1, k + 1);
        Vec3F vBake = cwind->getWindAtPoint(
            Vec3F(i + 1.0f, j + 1.0f, k + 1.0f) * sim->getCellSize());
        const bool obs = sim->O().get(i, j, k);
        m_delta->set(i, j, k, obs ? Vec3F::ZERO : vBake - vSim);
        m_sim->set(i, j, k, vSim);
        m_baked->set(i, j, k, vBake);
      }
    }
  }

  DLOG_INFO("[DELTA_FIELD] total error: {:.4f}", getError());
  const BoxPlot box = boxPlot();
  DLOG_INFO("[DELTA_FIELD] box plot: {:.4f} "
            "|{:.4f}---{:.4f}[{:.4f}]{:.4f}---{:.4f}| {:.4f}",
            box.minOutlier, box.minVal, box.perc25, box.median, box.perc75,
            box.maxVal, box.maxOutlier);

  // DLOG_INFO("[DELTA_FIELD] box plot 2: {:.4f},  {:.4f},  {:.4f},  {:.4f},  "
  //          "{:.4f},  {:.4f},  {:.4f}",
  //          box.minOutlier, box.minVal, box.perc25, box.median, box.perc75,
  //          box.maxVal, box.maxOutlier);

  constexpr bool kValueDifferenceDiagram = false;
  if constexpr (kValueDifferenceDiagram) {
    buildValueDifferenceDiagram(m_sim, m_baked, dim, "value_diff_diag.csv");
  }
}

// -------------------------------------------------------------------------- //

f32 DeltaField::getError() const {
  const FieldBase::Dim dim = m_delta->getDim();
  const u32 count = dim.width * dim.height * dim.depth;
  f32 error = 0.0f;
  for (u32 k = 0; k < dim.depth; k++) {
    for (u32 j = 0; j < dim.height; j++) {
      for (u32 i = 0; i < dim.width; i++) {
        error += m_delta->get(i, j, k).length();
      }
    }
  }
  if (count == 0) {
    return 0.0f;
  }
  return error / count;
}

DeltaField::BoxPlot DeltaField::boxPlot() {
  // Create sorted list of errors
  const FieldBase::Dim dim = m_delta->getDim();
  std::vector<f32> errors;
  for (u32 k = 0; k < dim.depth; k++) {
    for (u32 j = 0; j < dim.height; j++) {
      for (u32 i = 0; i < dim.width; i++) {
        errors.push_back(m_delta->get(i, j, k).length());
      }
    }
  }
  std::sort(errors.begin(), errors.end());

  // Box plit
  BoxPlot box;
  box.median = median(errors);
  box.perc25 = quartile1(errors);
  box.perc75 = quartile3(errors);
  const f32 iqr = box.perc75 - box.perc25; // Interquartile range

  // Find values
  box.minOutlier = errors[0];
  box.maxOutlier = errors[errors.size() - 1];
  box.minVal = box.minOutlier;
  box.maxVal = box.maxOutlier;
  for (size_t i = 0; i < errors.size(); i++) {
    if (errors[i] >= box.perc25 - 3 * iqr) {
      box.minVal = errors[i];
      break;
    }
  }
  for (size_t i = 0; i < errors.size(); i++) {
    const size_t i_ = errors.size() - i - 1;
    if (errors[i_] <= box.perc75 + 3 * iqr) {
      box.maxVal = errors[i_];
      break;
    }
  }

  return box;
}

// -------------------------------------------------------------------------- //

void DeltaField::paint(Painter &painter, const Vec3F &offset,
                       const Vec3F &padding) const {
  if (m_drawDelta) {
    m_delta->paintWithColor(painter, Color::green(), offset, padding);
  }
  if (m_drawSim) {
    m_sim->paintWithColor(painter, Color::red(), offset, padding);
  }
  if (m_drawBaked) {
    m_baked->paintWithColor(painter, Color::yellow(), offset, padding);
  }
}

// ============================================================ //

static void buildValueDifferenceDiagram(const VectorField *sim,
                                        const VectorField *bake,
                                        FieldBase::Dim dim,
                                        const std::string &path) {
  struct Entry {
    f32 vel;
    u32 simSamples;
    u32 bakeSamples;
  };
  std::vector<Entry> data{};

  constexpr f32 kResolution = 0.5f;
  constexpr f32 kMax = 100.0f;
  for (s32 i = 0; i * kResolution < kMax; ++i) {
    data.emplace_back(Entry{i * kResolution, 0, 0});
  }

  for (u32 z = 0; z < dim.depth; z++) {
    for (u32 y = 0; y < dim.height; y++) {
      for (u32 x = 0; x < dim.width; x++) {
        const Vec3F sv = sim->get(x, y, z);
        const Vec3F bv = bake->get(x, y, z);
        const s32 iSim =
            dclamp(s32(std::lroundf(sv.length() / kResolution)), s32(0),
                   s32(std::lroundf(kMax / kResolution) - 1));
        const s32 iBake =
            dclamp(s32(std::lroundf(bv.length() / kResolution)), s32(0),
                   s32(std::lroundf(kMax / kResolution) - 1));
        data[iSim].simSamples += 1;
        data[iBake].bakeSamples += 1;
      }
    }
  }

  std::string strOutput;
  // strOutput += "vel;simSamples;bakeSamples\n";
  // for (const auto &entry : data) {
  //   strOutput += std::to_string(entry.vel);
  //   strOutput += ";";
  //   strOutput += std::to_string(entry.simSamples);
  //   strOutput += ";";
  //   strOutput += std::to_string(entry.bakeSamples);
  //   strOutput += "\n";
  // }

  strOutput += "vel";
  for (const auto &entry : data) {
    strOutput += ";";
    strOutput += std::to_string(entry.vel);
  }
  strOutput += "\nsimSamples";
  for (const auto &entry : data) {
    strOutput += ";";
    strOutput += std::to_string(entry.simSamples);
  }
  strOutput += "\nbakeSamples";
  for (const auto &entry : data) {
    strOutput += ";";
    strOutput += std::to_string(entry.bakeSamples);
  }
  strOutput += "\n";

  dutil::File outFile{};
  outFile.Open(path, dutil::File::Mode::kWrite);
  const auto res = outFile.Write(strOutput);
  if (res != dutil::File::Result::kSuccess) {
    DLOG_ERROR("failed to write csv file: {}",
               dutil::File::ResultToString(res));
  } else {
    DLOG_INFO("successfully wrote csv file");
  }
}

} // namespace wind
