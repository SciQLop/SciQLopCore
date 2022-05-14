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
#include "SciQLopCore/Common/SciQLopObject.hpp"
#include "SciQLopCore/Data/DateTimeRange.hpp"

#include <QObject>
#include <QUuid>
#include <TimeSeries.h>
#include <functional>
#include <memory>

class DataProviderParameters;
class IDataSeries;

/**
 * @brief The IDataProvider interface aims to declare a data provider.
 *
 * A data provider is an entity that generates data and returns it according to
 * various parameters (time interval, product to retrieve the data, etc.) Since
 * its client mihgt use it from different threads it has to be either stateless
 * and/or thread safe
 *
 * @sa IDataSeries
 */
class IDataProvider : public QObject, public SciQLopObject
{
  Q_OBJECT

public:
  IDataProvider();

  virtual ~IDataProvider();

  inline virtual TimeSeries::ITimeSerie*
  getData(const DataProviderParameters& parameters)
  {
    throw std::runtime_error{
        "You must implement IDataProvider::getData method"};
  }

signals:

  void progress(QUuid requestID, double progress);
};

// Required for using shared_ptr in signals/slots
SCIQLOP_REGISTER_META_TYPE(IDATAPROVIDER_PTR_REGISTRY,
                           std::shared_ptr<IDataProvider>)
