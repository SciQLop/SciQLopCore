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
#include "SciQLopCore/Common/SciQLopObject.hpp"

#include <iostream>

QMap<QString, QSet<int>> SciQLopObject::used_names = {};

QString SciQLopObject::generate_unique_name(const QString& prefix)
{
  auto& indexes = SciQLopObject::used_names[prefix];
  int i         = 0;
  while(indexes.contains(i))
    i++;
  indexes.insert(i);
  return QString("%1-%2").arg(prefix).arg(i);
}

SciQLopObject::SciQLopObject(const QString& className)
    : _className{className}, _uniqueName{
                                 SciQLopObject::generate_unique_name(className)}
{}

SciQLopObject::~SciQLopObject()
{
  auto parts = name().split('-');
  if(std::size(parts) == 2)
  {
    bool convSuccess = false;
    auto index       = parts[1].toInt(&convSuccess);
    if(convSuccess) { SciQLopObject::used_names[parts[0]].remove(index); }
  }
}

QString SciQLopObject::name() const { return _uniqueName; }
