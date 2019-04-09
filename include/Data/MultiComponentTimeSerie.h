#ifndef SCIQLOP_MULTICOMPONENTTIMESERIE_H
#define SCIQLOP_MULTICOMPONENTTIMESERIE_H

#include "CoreGlobal.h"

#include <TimeSeries.h>

class SCIQLOP_CORE_EXPORT MultiComponentTimeSerie
    : public TimeSeries::TimeSerie<double, MultiComponentTimeSerie, 2>
{
public:
  using item_t =
      decltype(std::declval<
               TimeSeries::TimeSerie<double, MultiComponentTimeSerie, 2>>()[0]);

  using iterator_t = decltype(
      std::declval<TimeSeries::TimeSerie<double, MultiComponentTimeSerie, 2>>()
          .begin());

  MultiComponentTimeSerie() {}
  ~MultiComponentTimeSerie() = default;
  using TimeSerie::TimeSerie;
};

#endif // SCIQLOP_MULTICOMPONENTTIMESERIE_H
