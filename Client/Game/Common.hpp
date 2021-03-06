// Common stuff
// Author(s):       iFarbod <ifarbod@outlook.com>
//
// Copyright (c) 2015-2017 CtNorth Team
//
// Distributed under the MIT license (See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT)

#pragma once

#include <Hooking/Hook.hpp>

// Check size of a struct
// struc = struct/union/class to check
// size = size which it MUST be
#define VALIDATE_SIZE(struc, size) static_assert(sizeof(struc) == size, "The size of " #struc " is not " #size)

#define OFFSETOF(struc, member) ((size_t)&reinterpret_cast<const volatile char&>((((struc*)0)->member)))
//#define OFFSETOF(struc, member)                                                                                        \
//    (reinterpret_cast<size_t>(&reinterpret_cast<const volatile char&>(((reinterpret_cast<struc*>(0))->member))))
//#define OFFSETOF(struc, member)                                                                                        \
//    (reinterpret_cast<size_t>(&reinterpret_cast<const volatile char&>(((static_cast<struc*>(nullptr))->member))))

// Check offset of a member in a struct
// struc = struct/union/class to check
// member = a non-static member of the class/union/struct to check
// offset = offset which it MUST be
#define VALIDATE_OFFSET(struc, member, offset) static_assert(OFFSETOF(struc, member) == offset, "The offset of " #struc "::" #member " is not " #offset)

// Padding
#define PAD(prefix, name, size) u8 prefix##_##name##_[size]
