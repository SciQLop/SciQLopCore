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
#include <TimeSeries.h>
#include <cmath>
#include <utility>

class  SpectrogramTimeSerie
    : public TimeSeries::TimeSerie<double, SpectrogramTimeSerie, 2>
{
public:
  double min_sampling = std::nan("");
  double max_sampling = std::nan("");
  bool y_is_log       = true;
  using item_t =
      decltype(std::declval<
               TimeSeries::TimeSerie<double, SpectrogramTimeSerie, 2>>()[0]);

  using iterator_t = decltype(
      std::declval<TimeSeries::TimeSerie<double, SpectrogramTimeSerie, 2>>()
          .begin());

  SpectrogramTimeSerie() {}
  SpectrogramTimeSerie(SpectrogramTimeSerie::axis_t&& t,
                       SpectrogramTimeSerie::axis_t&& y,
                       SpectrogramTimeSerie::data_t&& values,
                       std::vector<std::size_t>& shape, double min_sampling,
                       double max_sampling, bool y_is_log = true)
      : TimeSeries::TimeSerie<double, SpectrogramTimeSerie, 2>(t, values,
                                                               shape),
        min_sampling{min_sampling}, max_sampling{max_sampling}, y_is_log{
                                                                    y_is_log}
  {
    _axes[1] = y;
  }

  SpectrogramTimeSerie(SpectrogramTimeSerie::axis_t&& t,
                       SpectrogramTimeSerie::axis_t&& y,
                       SpectrogramTimeSerie::data_t&& values,
                       const std::initializer_list<std::size_t>& shape, double min_sampling,
                       double max_sampling, bool y_is_log = true)
      : TimeSeries::TimeSerie<double, SpectrogramTimeSerie, 2>(t, values,
                                                               shape),
        min_sampling{min_sampling}, max_sampling{max_sampling}, y_is_log{
                                                                    y_is_log}
  {
    _axes[1] = y;
  }

  ~SpectrogramTimeSerie() = default;
  using TimeSerie::TimeSerie;
};

