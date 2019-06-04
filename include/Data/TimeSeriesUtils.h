#ifndef TIMESERIESUTILS_H
#define TIMESERIESUTILS_H
#include "MultiComponentTimeSerie.h"
#include "ScalarTimeSerie.h"
#include "SpectrogramTimeSerie.h"
#include "VectorTimeSerie.h"

#include <TimeSeries.h>
#include <cmath>
#include <memory>

namespace TimeSeriesUtils
{
  template<typename T> TimeSeries::ITimeSerie* copy(T input_ts)
  {
    if constexpr(std::is_base_of_v<TimeSeries::ITimeSerie, T>)
    {
      if(auto ts = dynamic_cast<VectorTimeSerie*>(input_ts))
      { return new VectorTimeSerie(*ts); }
      if(auto ts = dynamic_cast<ScalarTimeSerie*>(input_ts))
      { return new ScalarTimeSerie(*ts); }
      if(auto ts = dynamic_cast<MultiComponentTimeSerie*>(input_ts))
      { return new MultiComponentTimeSerie(*ts); }
      if(auto ts = dynamic_cast<SpectrogramTimeSerie*>(input_ts))
      { return new SpectrogramTimeSerie(*ts); }
    }
    else
    {
      if(auto ts = std::dynamic_pointer_cast<VectorTimeSerie>(input_ts))
      { return new VectorTimeSerie(*ts); }
      if(auto ts = std::dynamic_pointer_cast<ScalarTimeSerie>(input_ts))
      { return new ScalarTimeSerie(*ts); }
      if(auto ts = std::dynamic_pointer_cast<SpectrogramTimeSerie>(input_ts))
      { return new SpectrogramTimeSerie(*ts); }
      if(auto ts = std::dynamic_pointer_cast<MultiComponentTimeSerie>(input_ts))
      { return new MultiComponentTimeSerie(*ts); }
    }
    return nullptr;
  }

  struct axis_properties
  {
    double range;
    double max_resolution;
    bool is_log;
    double min;
    double max;
  };

  constexpr auto IsLog    = true;
  constexpr auto IsLinear = false;

  constexpr auto CheckMedian     = true;
  constexpr auto DontCheckMedian = false;

  template<bool is_log, bool check_median>
  axis_properties axis_analysis(
      typename std::conditional<is_log, std::vector<double>&,
                                const std::vector<double>&>::type axis,
      double given_max_resolution = std::nan(""))
  {
    std::vector<double> axis_diff(axis.size());
    if constexpr(is_log)
    {
      std::transform(std::cbegin(axis), std::cend(axis), std::begin(axis),
                     [](const auto v) { return std::log10(v); });
    }
    auto min      = *std::begin(axis);
    auto max      = *(std::cend(axis) - 1);
    auto range    = max - min;
    auto min_diff = given_max_resolution;
    if(std::isnan(min_diff))
    {
      std::adjacent_difference(std::cbegin(axis), std::cend(axis),
                               std::begin(axis_diff));
      min_diff =
          (*std::min_element(std::cbegin(axis_diff) + 1, std::cend(axis_diff)));
      if constexpr(check_median)
      {
        std::nth_element(std::begin(axis_diff) + 1,
                         std::begin(axis_diff) + axis_diff.size() / 2,
                         std::end(axis_diff));
        auto median_diff = *(std::begin(axis_diff) + axis_diff.size() / 2);
        if(median_diff > (4 * min_diff)) min_diff = median_diff;
      }
    }

    return {range, min_diff, is_log, min, max};
  }

} // namespace TimeSeriesUtils

#endif
