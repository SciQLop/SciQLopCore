#ifndef SCIQLOP_SPECTROGRAMTIMESERIE_H
#define SCIQLOP_SPECTROGRAMTIMESERIE_H

#include "CoreGlobal.h"

#include <TimeSeries.h>

class SCIQLOP_CORE_EXPORT SpectrogramTimeSerie
    : public TimeSeries::TimeSerie<double, SpectrogramTimeSerie, 2>
{
public:
  using item_t =
      decltype(std::declval<
               TimeSeries::TimeSerie<double, SpectrogramTimeSerie, 2>>()[0]);

  using iterator_t = decltype(
      std::declval<TimeSeries::TimeSerie<double, SpectrogramTimeSerie, 2>>()
          .begin());

  SpectrogramTimeSerie() {}
  SpectrogramTimeSerie(SpectrogramTimeSerie::axis_t& t,
                       SpectrogramTimeSerie::axis_t& y,
                       SpectrogramTimeSerie::container_type<
                           SpectrogramTimeSerie::raw_value_type>& values,
                       std::vector<std::size_t>& shape)
      : TimeSeries::TimeSerie<double, SpectrogramTimeSerie, 2>(t, values, shape)
  {
    _axes[1] = y;
  }

  ~SpectrogramTimeSerie() = default;
  using TimeSerie::TimeSerie;
};

#endif // SCIQLOP_SPECTROGRAMTIMESERIE_H
