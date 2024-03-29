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
#include "MultiComponentTimeSerie.hpp"
#include "ScalarTimeSerie.hpp"
#include "SpectrogramTimeSerie.hpp"
#include "VectorTimeSerie.hpp"

#include <QString>

enum class DataSeriesType
{
  NONE,
  SCALAR,
  VECTOR,
  MULTICOMPONENT,
  SPECTROGRAM
};

struct DataSeriesTypeUtils
{
  static DataSeriesType fromString(const QString& type)
  {
    if(type.toLower() == QStringLiteral("scalar"))
    {
      return DataSeriesType::SCALAR;
    }
    else if(type.toLower() == QStringLiteral("spectrogram"))
    {
      return DataSeriesType::SPECTROGRAM;
    }
    else if(type.toLower() == QStringLiteral("vector"))
    {
      return DataSeriesType::VECTOR;
    }
    else if(type.toLower() == QStringLiteral("multicomponent"))
    {
      return DataSeriesType::MULTICOMPONENT;
    }
    else { return DataSeriesType::NONE; }
  }
  static DataSeriesType type(TimeSeries::ITimeSerie* ts)
  {
    if(!ts) return DataSeriesType::NONE;
    if(dynamic_cast<ScalarTimeSerie*>(ts)) return DataSeriesType::SCALAR;
    if(dynamic_cast<VectorTimeSerie*>(ts)) return DataSeriesType::VECTOR;
    if(dynamic_cast<MultiComponentTimeSerie*>(ts))
      return DataSeriesType::MULTICOMPONENT;
    if(dynamic_cast<SpectrogramTimeSerie*>(ts))
      return DataSeriesType::SPECTROGRAM;
    return DataSeriesType::NONE;
  }
};
