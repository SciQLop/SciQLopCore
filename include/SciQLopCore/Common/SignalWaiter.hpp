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

#include <QEventLoop>

/**
 * Class for synchronously waiting for the reception of a signal. The signal to wait is passed to
 * the construction of the object. When starting the wait, a timeout can be set to exit if the
 * signal has not been sent
 */
class SignalWaiter : public QObject {
    Q_OBJECT
public:
    /**
     * Ctor
     * @param object the sender of the signal
     * @param signal the signal to listen
     */
    explicit SignalWaiter(QObject &sender, const char *signal);

    /**
     * Starts the signal and leaves after the signal has been received, or after the timeout
     * @param timeout the timeout set (if 0, uses a default timeout)
     * @return true if the signal was sent, false if the timeout occured
     */
    bool wait(int timeout);

private:
    bool m_Timeout;
    QEventLoop m_EventLoop;

private slots:
    void timeout();
};

