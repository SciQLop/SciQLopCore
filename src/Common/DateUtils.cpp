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
#include "SciQLopCore/Common/DateUtils.hpp"

#include <cmath>

QDateTime DateUtils::dateTime(double secs, Qt::TimeSpec timeSpec) noexcept
{
  // Uses msecs to be Qt 4 compatible
  if(!std::isnan(secs))
    return QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(secs * 1000.),
                                          timeSpec);
  return QDateTime();
}

double DateUtils::secondsSinceEpoch(const QDateTime& dateTime) noexcept
{
  // Uses msecs to be Qt 4 compatible
  return dateTime.toMSecsSinceEpoch() / 1000.;
}
