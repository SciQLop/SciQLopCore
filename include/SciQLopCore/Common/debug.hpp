/*------------------------------------------------------------------------------
-- This file is a part of the SciQLop Software
-- Copyright (C) 2022, Plasma Physics Laboratory - CNRS
--
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
-------------------------------------------------------------------------------*/
/*-- Author : Alexis Jeandet
-- Mail : alexis.jeandet@member.fsf.org
----------------------------------------------------------------------------*/
#pragma once
#include <assert.h>
#include <iostream>
#include <string>
#include <typeinfo>

template<class T, bool crash>
void SciQLopError(const std::string& message, int line, const std::string& file)
{
  std::cerr << "Error in " << file << ", class:" << typeid(T).name()
            << ", line: " << line << std::endl
            << "Message: " << message << std::endl;
  if constexpr(crash) throw;
}

#ifdef SCIQLOP_CRASH_ON_ERROR
#define SCIQLOP_ERROR(CLASS, MESSAGE)                                          \
  SciQLopError<CLASS, true>(MESSAGE, __LINE__, __FILE__)
#else
#define SCIQLOP_ERROR(CLASS, MESSAGE)                                          \
  SciQLopError<CLASS, false>(MESSAGE, __LINE__, __FILE__)
#endif