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
#include <QList>
#include <QPair>
#include <SciQLopCore/Data/DataSeriesType.hpp>
#include <SciQLopCore/DataSource/DataProviderParameters.hpp>
#include <SciQLopCore/DataSource/DataSourceItem.hpp>
#include <SciQLopCore/DataSource/DataSourceItemAction.hpp>
#include <SciQLopCore/DataSource/DataSources.hpp>
#include <SciQLopCore/DataSource/IDataProvider.hpp>
#include <SciQLopCore/SciQLopCore.hpp>
#include <TimeSeries.h>
// must be included last because of Python/Qt definition of slots
#include "numpy_wrappers.hpp"
namespace py
{
  struct Product
  {
    QString path;
    std::vector<std::string> components;
    QMap<QString, QString> metadata;
    Product() = default;
    explicit Product(const QString& path,
                     const std::vector<std::string>& components,
                     const QMap<QString, QString>& metadata)
        : path{path}, components{components}, metadata{metadata}
    {}
    ~Product() = default;
  };

  struct ITimeSerie
  {
    ITimeSerie();
    ITimeSerie(TimeSeries::ITimeSerie* ts);
    virtual ~ITimeSerie();
    inline TimeSeries::ITimeSerie* take()
    {
      auto result = ts;
      ts=nullptr;
      return result;
    }
  private:
    TimeSeries::ITimeSerie* ts;
  };

  struct ScalarTimeSerie : ITimeSerie
  {
    inline ScalarTimeSerie(NpArray time, NpArray values)
        : ITimeSerie{new ::ScalarTimeSerie{std::move(time.data),
                                           std::move(values.data)}}
    {}
  };

  struct VectorTimeSerie : ITimeSerie
  {
    inline VectorTimeSerie(NpArray time, NpArray values)
        : ITimeSerie{new ::VectorTimeSerie{std::move(time.data),
                                           values.to_std_vect_vect()}}
    {}
  };

  struct MultiComponentTimeSerie : ITimeSerie
  {
    inline MultiComponentTimeSerie(NpArray time, NpArray values)
        : ITimeSerie{new ::MultiComponentTimeSerie{
              std::move(time.data),
              std::move(values.data),
              {time.flat_size(), values.flat_size() / time.flat_size()}}}
    {}
  };

  struct SpectrogramTimeSerie : ITimeSerie
  {
    inline SpectrogramTimeSerie(NpArray time, NpArray y, NpArray values)
        : ITimeSerie{new ::SpectrogramTimeSerie{
              std::move(time.data),
              std::move(y.data),
              std::move(values.data),
              {time.flat_size(), values.flat_size() / time.flat_size()},
              std::nan("1"),
              std::nan("1")}}
    {}
  };

  class DataProvider : public IDataProvider
  {
  public:
    DataProvider();

    virtual ~DataProvider();

    virtual ITimeSerie* get_data(const QMap<QString, QString>& key,
                                 double start_time, double stop_time);

    virtual TimeSeries::ITimeSerie*
    getData(const DataProviderParameters& parameters) override;

    void set_icon(const QString& path, const QString& name);

    void register_products(const QVector<Product*>& products);
  };
} // namespace py
