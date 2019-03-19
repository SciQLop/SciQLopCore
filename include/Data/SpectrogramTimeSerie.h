#ifndef SCIQLOP_SPECTROGRAMTIMESERIE_H
#define SCIQLOP_SPECTROGRAMTIMESERIE_H

#include "CoreGlobal.h"

#include <TimeSeries.h>

class SCIQLOP_CORE_EXPORT SpectrogramTimeSerie
    : public TimeSeries::TimeSerie<double, SpectrogramTimeSerie, 2>
{
public:
  using item_t =
      decltype(TimeSeries::TimeSerie<double, SpectrogramTimeSerie, 2>()[0]);
  SpectrogramTimeSerie() {}
  ~SpectrogramTimeSerie() = default;
  using TimeSerie::TimeSerie;
};

#endif // SCIQLOP_SPECTROGRAMTIMESERIE_H
