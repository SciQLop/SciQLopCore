#ifndef TIMESERIESUTILS_H
#define TIMESERIESUTILS_H
#include "MultiComponentTimeSerie.h"
#include "ScalarTimeSerie.h"
#include "SpectrogramTimeSerie.h"
#include "VectorTimeSerie.h"

#include <TimeSeries.h>
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
} // namespace TimeSeriesUtils

#endif
