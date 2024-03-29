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

#include "SciQLopCore/Common/MetaTypes.hpp"
#include "cpp_utils/types/detectors.hpp"

#include <QString>
#include <QMap>
#include <QSet>


HAS_METHOD(has_metaObject, metaObject)

#include <array>
#include <string>
#include <string_view>
#include <utility>

// taken here
// https://bitwizeshift.github.io/posts/2021/03/09/getting-an-unmangled-type-name-at-compile-time/

template<std::size_t... Idxs>
constexpr auto substring_as_array(std::string_view str,
                                  std::index_sequence<Idxs...>)
{
  return std::array{str[Idxs]..., '\n'};
}

template<typename T> constexpr auto type_name_array()
{
#if defined(__clang__)
  constexpr auto prefix   = std::string_view{"[T = "};
  constexpr auto suffix   = std::string_view{"]"};
  constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(__GNUC__)
  constexpr auto prefix   = std::string_view{"with T = "};
  constexpr auto suffix   = std::string_view{"]"};
  constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(_MSC_VER)
  constexpr auto prefix   = std::string_view{"type_name_array<"};
  constexpr auto suffix   = std::string_view{">(void)"};
  constexpr auto function = std::string_view{__FUNCSIG__};
#else
#error Unsupported compiler
#endif

  constexpr auto start = function.find(prefix) + prefix.size();
  constexpr auto end   = function.rfind(suffix);

  static_assert(start < end);

  constexpr auto name = function.substr(start, (end - start));
  return substring_as_array(name, std::make_index_sequence<name.size()>{});
}

template<typename T> struct type_name_holder
{
  static inline constexpr auto value = type_name_array<T>();
};

template<typename T> constexpr auto type_name() -> std::string_view
{
  constexpr auto& value = type_name_holder<T>::value;
  return std::string_view{value.data(), value.size()};
}

class SciQLopObject
{
  static QMap<QString, QSet<int>> used_names;

  static QString generate_unique_name(const QString& prefix);

  QString _className;
  QString _uniqueName;

  SciQLopObject(const QString& className);

public:

  template<typename T> SciQLopObject(T* self) : SciQLopObject{className(self)} {}

  virtual ~SciQLopObject();

  QString name()const;

  template<typename T> static QString className(const T*)
  {
    if constexpr(has_metaObject_v<T>)
    {
      return T::staticMetaObject.className();
    }
    else { return QString::fromStdString(std::string{type_name<T>()}); }
  }
};
