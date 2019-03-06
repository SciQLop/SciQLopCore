#ifndef SCIQLOP_VECTORTIMESERIE_H
#define SCIQLOP_VECTORTIMESERIE_H

#include "CoreGlobal.h"

#include <TimeSeries.h>

struct Vector
{
  double x, y, z;
};

class SCIQLOP_CORE_EXPORT VectorTimeSerie
    : public TimeSeries::TimeSerie<Vector, VectorTimeSerie>
{
public:
  VectorTimeSerie() {}
  ~VectorTimeSerie() = default;
  using TimeSerie::TimeSerie;
};

#endif // SCIQLOP_VECTORTIMESERIE_H
