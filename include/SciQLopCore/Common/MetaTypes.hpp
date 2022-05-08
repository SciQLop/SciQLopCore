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

#include <QMetaType>

/**
 * Struct used to create an instance that registers a type in Qt for signals /
 * slots mechanism
 * @tparam T the type to register
 */
template<typename T> struct MetaTypeRegistry
{
  explicit MetaTypeRegistry() { qRegisterMetaType<T>(); }
};

/**
 * This macro can be used to :
 * - declare a type as a Qt meta type
 * - and register it (through a static instance) at the launch of SciQlop, so it
 * can be passed in Qt signals/slots
 *
 * It can be used both in .h or in .cpp files
 *
 * @param NAME name of the instance under which the type will be registered (in
 * uppercase)
 * @param TYPE type to register
 *
 * Example:
 * ~~~cpp
 * // The following macro :
 * // - declares std::shared_ptr<Variable> as a Qt meta type
 * // - registers it through an instance named VAR_SHARED_PTR
 * SCIQLOP_REGISTER_META_TYPE(VAR_SHARED_PTR, std::shared_ptr<Variable>)
 *
 * // The following macro :
 * // - declares a raw pointer of Variable as a Qt meta type
 * // - registers it through an instance named VAR_RAW_PTR
 * SCIQLOP_REGISTER_META_TYPE(VAR_RAW_PTR, Variable*)
 * ~~~
 *
 */
// clang-format off
#define SCIQLOP_REGISTER_META_TYPE(NAME, TYPE)  \
Q_DECLARE_METATYPE(TYPE)                        \
const auto NAME = MetaTypeRegistry<TYPE>{};     \
// clang-format on

