// MIT License
//
// Copyright (c) 2020 Filip Björklund, Christoffer Gustafsson
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

#include "cspline_follow.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "cspline.hpp"

// ========================================================================== //
// CSpline Implementation
// ========================================================================== //

namespace wind {

CSplineFollow::CSplineFollow(const bs::HSceneObject &parent, f32 speed,
                             WrapMode wrapMode)
    : Component(parent), m_wrapMode(wrapMode), m_pos(0.0f), m_speed(speed) {}

// -------------------------------------------------------------------------- //

void CSplineFollow::fixedUpdate() {
  HCSpline splineComp = SO()->getComponent<CSpline>();
  if (splineComp) {
    Spline *spline = splineComp->getSpline();
    m_pos += m_speed;
    clampPos();
    const Vec3F pos = spline->sample(m_pos);
    SO()->setPosition(pos);

    bs::HSceneObject splineObj = SO()->findChild("spline_waypoints");
    splineObj->setPosition(-pos);
  }
}

// -------------------------------------------------------------------------- //

void CSplineFollow::clampPos() {
  if (m_pos > 1.0f) {
    switch (m_wrapMode) {
    case WrapMode::kWrap: {
      m_pos = 0.0f;
      break;
    }
    case WrapMode::kOscillate: {
      m_speed = -m_speed;
      m_pos = 1.0f;
      break;
    }
    case WrapMode::kStop: {
      m_pos = 1.0f;
      break;
    }
    default: { break; }
    }
  } else if (m_pos < 0.0f) {
    switch (m_wrapMode) {
    case WrapMode::kWrap: {
      m_pos = 1.0f;
      break;
    }
    case WrapMode::kOscillate: {
      m_pos = 0.0f;
      m_speed = -m_speed;
      break;
    }
    case WrapMode::kStop: {
      m_pos = 0.0f;
      break;
    }
    default: { break; }
    }
  }
}

// -------------------------------------------------------------------------- //

bs::RTTITypeBase *CSplineFollow::getRTTIStatic() {
  return CSplineFollowRTTI::instance();
}

// -------------------------------------------------------------------------- //

CSplineFollow::WrapMode CSplineFollow::wrapModeFromString(const String &str) {
  if (str == "oscillate") {
    return WrapMode::kOscillate;
  }
  if (str == "stop") {
    return WrapMode::kStop;
  }
  return WrapMode::kWrap;
}
} // namespace wind