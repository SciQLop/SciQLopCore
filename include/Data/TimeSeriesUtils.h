#ifndef TIMESERIESUTILS_H
#define TIMESERIESUTILS_H
#include <TimeSeries.h>
#include <memory>
#include "ScalarTimeSerie.h"
#include "VectorTimeSerie.h"
#include "SpectrogramTimeSerie.h"

namespace TimeSeriesUtils
{

template <typename T>
TimeSeries::ITimeSerie* copy(T input_ts)
{
    if constexpr(std::is_base_of_v<TimeSeries::ITimeSerie,T>)
    {
        if (auto ts = dynamic_cast<VectorTimeSerie*>(input_ts))
        {
            return new VectorTimeSerie(*ts);
        }
        if (auto ts = dynamic_cast<ScalarTimeSerie*>(input_ts))
        {
            return new ScalarTimeSerie(*ts);
        }
        if (auto ts = dynamic_cast<SpectrogramTimeSerie*>(input_ts))
        {
            return new SpectrogramTimeSerie(*ts);
        }
    }
    else
    {
        if (auto ts = std::dynamic_pointer_cast<VectorTimeSerie>(input_ts))
        {
            return new VectorTimeSerie(*ts);
        }
        if (auto ts = std::dynamic_pointer_cast<ScalarTimeSerie>(input_ts))
        {
            return new ScalarTimeSerie(*ts);
        }
        if (auto ts = std::dynamic_pointer_cast<SpectrogramTimeSerie>(input_ts))
        {
            return new SpectrogramTimeSerie(*ts);
        }
    }
    return nullptr;
}
}

#endif
