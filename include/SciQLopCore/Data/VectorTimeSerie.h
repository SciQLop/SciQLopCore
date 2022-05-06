#pragma once

#include <TimeSeries.h>

struct Vector
{
  double x, y, z;
};

class VectorTimeSerie
    : public TimeSeries::TimeSerie<Vector, VectorTimeSerie>
{
public:
  VectorTimeSerie() {}
  ~VectorTimeSerie() = default;
  using TimeSerie::TimeSerie;
};
