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

#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/types.hpp"

// ========================================================================== //
// RTTI Types
// ========================================================================== //

namespace wind {

/// RTTI Type ID of CRotor component
constexpr u32 TID_CRotor = 2000;
/// RTTI Type ID of CNetComponent component
constexpr u32 TID_CNetComponent = 2001;
/// RTTI Type ID of CMyPlayer component
constexpr u32 TID_CMyPlayer = 2002;
/// RTTI Type ID of FPSWalker component
constexpr u32 TID_FPSWalker = 2003;
/// RTTI Type ID of FPSCamera component
constexpr u32 TID_FPSCamera = 2004;
/// RTTI Type ID of CWind component
constexpr u32 TID_CWindSource = 2005;
/// RTTI Type ID of CTag component
constexpr u32 TID_CTag = 2006;
/// RTTI Type ID of CSpline component
constexpr u32 TID_CSpline = 2007;
/// RTTI Type ID of CSplineFollow component
constexpr u32 TID_CSplineFollow = 2008;
/// RTTI Type ID of CWindOccluder component
constexpr u32 TID_CWindOccluder = 2009;

} // namespace wind
