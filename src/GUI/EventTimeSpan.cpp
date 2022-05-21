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
#include "SciQLopCore/GUI/EventTimeSpan.hpp"

#include "SciQLopCore/GUI/PlotWidget.hpp"

#include <iostream>

void EventTimeSpan::addTimeSpan(PlotWidget* plot, const DateTimeRange& range)
{
  using namespace SciQLopPlots;
  if(plot)
  {
    auto ts = new SciQLopPlots::TimeSpan{
        plot, axis::range{range.m_TStart, range.m_TEnd}};
    ts->set_deletable(false);
    timeSpans.push_back(ts);
    connect(plot, &PlotWidget::destroyed, this,
            [this, ts]() { timeSpans.removeAll(ts); });

    connect(ts, &SciQLopPlots::TimeSpan::range_changed, this,
            [this](const axis::range& range) {
              setTimeRange(range.first, range.second);
            });
  }
}

EventTimeSpan::EventTimeSpan(TimeSyncPanel* panel, const DateTimeRange& range)
    : QObject{}, SciQLopObject{this}, _range{range}
{
  for(auto p : panel->plots())
  {
    addTimeSpan(dynamic_cast<PlotWidget*>(p), range);
  }
  connect(panel, &TimeSyncPanel::plotAdded, this,
          [this](PlotWidget* plot) { this->addTimeSpan(plot, this->_range); });
}

EventTimeSpan::EventTimeSpan(TimeSyncPanel* panel, double start, double stop)
    : EventTimeSpan(panel, DateTimeRange{start, stop})
{}

EventTimeSpan::~EventTimeSpan()
{
  for(auto ts : timeSpans)
  {
    delete ts;
  }
}

void EventTimeSpan::setTimeRange(const DateTimeRange& range)
{
  using namespace SciQLopPlots;
  auto r = axis::range{range.m_TStart, range.m_TEnd};
  if(range != _range)
  {
    _range = range;
    for(auto ts : timeSpans)
    {
      std::cout << "EventTimeSpan::setTimeRange" << std::endl;
      ts->set_range(r);
    }
  }
}

void EventTimeSpan::setTimeRange(double start, double stop)
{
  setTimeRange(DateTimeRange{start, stop});
}
