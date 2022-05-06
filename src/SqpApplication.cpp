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
#include "SciQLopCore/SqpApplication.hpp"

#include <SciQLopCore/Data/IDataProvider.hpp>
#include <SciQLopCore/DataSource/DataSources.hpp>
#include <QThread>


Q_LOGGING_CATEGORY(LOG_SqpApplication, "SqpApplication")

class SqpApplication::SqpApplicationPrivate
{
public:
    SqpApplicationPrivate()
    {
        // /////////////////////////////// //
        // Connections between controllers //
        // /////////////////////////////// //

    }

    virtual ~SqpApplicationPrivate()
    {

    }

    DataSources m_DataSources;

};


SqpApplication::SqpApplication(int& argc, char** argv)
        : QApplication { argc, argv }, impl { std::make_unique<SqpApplicationPrivate>() }
{
    this->setStyle(new MyProxyStyle(this->style()));
    qCDebug(LOG_SqpApplication()) << tr("SqpApplication construction") << QThread::currentThread();

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

}

SqpApplication::~SqpApplication() {}

void SqpApplication::initialize() {}


DataSources& SqpApplication::dataSources() noexcept
{
    return impl->m_DataSources;
}


