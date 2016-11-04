// Vector base
// Author(s):       iFarbod <ifarbod@outlook.com>
//
// Copyright (c) 2015-2016 The San Andreas Online Open Source Project
//
// Distributed under the MIT license (See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT)

#include "PCH.hpp"

#include "VectorBase.hpp"

namespace Util
{

unsigned char* VectorBase::AllocateBuffer(unsigned size)
{
    return new unsigned char[size];
}

}