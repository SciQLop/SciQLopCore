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
#include "SciQLopCore/Common/SignalWaiter.hpp"

#include <QTimer>

namespace
{

  const auto DEFAULT_TIMEOUT = 30000;

} // namespace

SignalWaiter::SignalWaiter(QObject& sender, const char* signal)
    : m_Timeout{false}
{
  connect(&sender, signal, &m_EventLoop, SLOT(quit()));
}

bool SignalWaiter::wait(int timeout)
{
  if(timeout == 0) { timeout = DEFAULT_TIMEOUT; }

  QTimer timer{};
  timer.setInterval(timeout);
  timer.start();
  connect(&timer, &QTimer::timeout, this, &SignalWaiter::timeout);

  m_EventLoop.exec();

  return !m_Timeout;
}

void SignalWaiter::timeout()
{
  m_Timeout = true;
  m_EventLoop.quit();
}
