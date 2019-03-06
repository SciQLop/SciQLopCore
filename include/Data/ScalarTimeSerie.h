#ifndef SCIQLOP_SCALARTIMESERIE_H
#define SCIQLOP_SCALARTIMESERIE_H

#include "CoreGlobal.h"

#include <TimeSeries.h>

class SCIQLOP_CORE_EXPORT ScalarTimeSerie
    : public TimeSeries::TimeSerie<double, ScalarTimeSerie>
{
public:
  ScalarTimeSerie() {}
  ~ScalarTimeSerie() = default;
  using TimeSerie::TimeSerie;
};

#endif // SCIQLOP_SCALARSERIES_H
