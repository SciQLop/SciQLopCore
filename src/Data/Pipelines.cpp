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
#include "SciQLopCore/Data/Pipelines.hpp"

#include "SciQLopCore/DataSource/DataProviderParameters.hpp"
#include "SciQLopCore/DataSource/DataSources.hpp"
#include "SciQLopCore/DataSource/IDataProvider.hpp"
#include "SciQLopCore/SciQLopCore.hpp"
#include "SciQLopPlots/Qt/Graph.hpp"

using data_t = std::pair<std::vector<double>, std::vector<double>>;

template<DataSeriesType ds_type> data_t to_data_t(TimeSeries::ITimeSerie* ts);

template<> data_t to_data_t<DataSeriesType::SCALAR>(TimeSeries::ITimeSerie* ts)
{
  std::cout << "to_data_t" << std::endl;
  auto scalar_ts = dynamic_cast<ScalarTimeSerie*>(ts);
  std::cout << "size: " << scalar_ts->size() << std::endl;
  std::vector<double> x(scalar_ts->size());
  std::vector<double> y(scalar_ts->size());
  for(auto i = 0UL; i < scalar_ts->size(); i++)
  {
    y[i] = scalar_ts->v(i);
    x[i] = scalar_ts->t(i);
  }
  return {x, y};
}

template<> data_t to_data_t<DataSeriesType::VECTOR>(TimeSeries::ITimeSerie* ts)
{
  if(ts)
  {
    auto vector_ts = dynamic_cast<VectorTimeSerie*>(ts);
    std::vector<double> x(vector_ts->size());
    std::vector<double> y(3 * vector_ts->size());
    return {x, y};
  }
  return {};
}

template<typename data_t, DataSeriesType ds_type>
class Pipeline : public IPipeline
{
  std::thread genThread;
  SciQLopPlots::Graph<data_t, SciQLopPlots::SciQLopPlot, int> graph;
  IDataProvider* provider;

public:
  Pipeline(SciQLopPlots::SciQLopPlot* plot, IDataProvider* provider,
           QVariantHash metaData, QColor color = Qt::blue)
      : graph{SciQLopPlots::add_graph<data_t>(plot, color)}, provider{provider}
  {
    std::cout << "Pipeline ctor" << std::endl;
    genThread = std::thread([&graph = graph, &in = graph.transformations_out,
                             metaData, provider]() {
      while(!in.closed())
      {
        if(auto maybeNewRange = in.take(); maybeNewRange)
        {
          std::cout << "Update" << std::endl;
          DataProviderParameters p{
              {maybeNewRange->first, maybeNewRange->second}, metaData};
          graph << to_data_t<ds_type>(provider->getData(p));
        }
      }
    });
  }
  inline ~Pipeline() override {}
};

Pipelines::Pipelines(QObject* parent) : QObject{parent} {}

void Pipelines::plot(const QStringList& products,
                     SciQLopPlots::SciQLopPlot* plot)
{
  for(const auto& product : products)
  {
    auto provider = SciQLopCore::dataSources().provider(product);
    auto ds_type  = SciQLopCore::dataSources().dataSeriesType(product);
    auto metaData = SciQLopCore::dataSources().nodeData(product);
    switch(ds_type)
    {
      case DataSeriesType::SCALAR: {
        std::cout << "Scalar TS" << std::endl;
        auto pipeline = new Pipeline<data_t, DataSeriesType::SCALAR>(
            plot, provider, metaData);
      }
      break;
      default: break;
    }
    // SciQLopPlots::add_graph<>(this);
    std::cout << provider << std::endl;
  }
}
