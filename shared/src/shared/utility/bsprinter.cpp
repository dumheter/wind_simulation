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

#include "shared/utility/bsprinter.hpp"

// ========================================================================== //
// Functions
// ========================================================================== //

std::ostream &operator<<(std::ostream &out, const bs::Vector3 &vec) {
  out << fmt::format("({:.1f}, {:.1f}, {:.1f})", vec.x, vec.y, vec.z);
  return out;
}

// -------------------------------------------------------------------------- //

std::ostream &operator<<(std::ostream &out, const bs::Vector2 &vec) {
  out << fmt::format("({:.1f}, {:.1f})", vec.x, vec.y);
  return out;
}

// -------------------------------------------------------------------------- //

std::ostream &operator<<(std::ostream &out, wind::ObjectType type) {
  switch (type) {
  case wind::ObjectType::kEmpty: {
    out << "kEmpty";
    break;
  }
  case wind::ObjectType::kSkybox: {
    out << "kSkybox";
    break;
  }
  case wind::ObjectType::kPlane: {
    out << "kPlane";
    break;
  }
  case wind::ObjectType::kCube: {
    out << "kCube";
    break;
  }
  case wind::ObjectType::kBall: {
    out << "kBall";
    break;
  }
  case wind::ObjectType::kModel: {
    out << "kModel";
    break;
  }
  case wind::ObjectType::kPlayer: {
    out << "kPlayer";
    break;
  }
  case wind::ObjectType::kRotor: {
    out << "kRotor";
    break;
  }
  case wind::ObjectType::kWindSource: {
    out << "kWindSource";
    break;
  }
  case wind::ObjectType::kInvalid:
  default: {
    out << "kInvalid";
    break;
  }
  }
  return out;
}

// -------------------------------------------------------------------------- //

std::ostream &operator<<(std::ostream &out, const bs::HComponent &comp) {
  if (comp->getRTTI()->getRTTIId() == wind::TID_CTag) {
    wind::CTag *tag = static_cast<wind::CTag *>(comp.get());
    out << fmt::format("CTag {{ {} }}", tag->getType());
  } else {
    out << comp->getName();
  }
  return out;
}
