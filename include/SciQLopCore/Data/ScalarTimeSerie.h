
#pragma once
#include <TimeSeries.h>

class ScalarTimeSerie
    : public TimeSeries::TimeSerie<double, ScalarTimeSerie>
{
public:
  ScalarTimeSerie() {}
  ~ScalarTimeSerie() = default;
  using TimeSerie::TimeSerie;
};
