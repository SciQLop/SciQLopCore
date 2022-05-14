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
#include "PyDataProvider.hpp"

py::DataProvider::DataProvider(QObject* parent): IDataProvider(parent){}

py::DataProvider::~DataProvider()
{
  std::cout << "py::DataProvider::~DataProvider()" << std::endl;
}

py::ITimeSerie* py::DataProvider::get_data(const QMap<QString, QString>& key,
                                           double start_time, double stop_time)
{
  (void)key, (void)start_time, (void)stop_time;
  return nullptr;
}

TimeSeries::ITimeSerie*
py::DataProvider::getData(const DataProviderParameters& parameters)
{
  if(parameters.m_Data.contains("name"))
  {
    QMap<QString, QString> metadata;
    std::for_each(parameters.m_Data.constKeyValueBegin(),
                  parameters.m_Data.constKeyValueEnd(),
                  [&metadata](const auto& item) {
                    metadata[item.first] = item.second.toString();
                  });
    auto result = get_data(metadata, parameters.m_Range.m_TStart,
                           parameters.m_Range.m_TEnd);
    std::cout << "Before take" << std::endl;
    auto ts     = result->take();
    delete result;
    return ts;
  }
  return nullptr;
}

void py::DataProvider::set_icon(const QString& path, const QString& name)
{
  SciQLopCore::dataSources().setIcon(path, name);
}

void py::DataProvider::register_products(const QVector<Product*>& products)
{
  auto& dataSources     = SciQLopCore::dataSources();
  auto id               = this->id();
  auto data_source_name = this->name();
  std::for_each(std::cbegin(products), std::cend(products),
                [&id, &dataSources](const Product* product) {
                  dataSources.addDataSourceItem(
                      id, product->path, product->ds_type, product->metadata);
                });
}

py::ITimeSerie::ITimeSerie() : ts{nullptr} {}

py::ITimeSerie::ITimeSerie(TimeSeries::ITimeSerie* ts) : ts{ts} {}

py::ITimeSerie::~ITimeSerie()
{
  std::cout << "py::ITimeSerie::~ITimeSerie()" << std::endl;
  if(ts) delete ts;
}
