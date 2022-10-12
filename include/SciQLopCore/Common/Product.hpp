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
#include <vector>
#include <string>
#include <QPair>
#include <QMap>
#include <QString>
#include "SciQLopCore/Data/DataSeriesType.hpp"


struct Product
{
  QString path;
  std::vector<std::string> components;
  QMap<QString, QString> metadata;
  DataSeriesType ds_type;
  Product() = default;
  explicit Product(const QString& path,
                   const std::vector<std::string>& components,
                   DataSeriesType ds_type,
                   const QMap<QString, QString>& metadata)
      : path{path}, components{components}, metadata{metadata},ds_type{ds_type}
  {}
  ~Product() = default;
};
