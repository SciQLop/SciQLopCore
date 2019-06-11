#ifndef SCIQLOP_SPECTROGRAMTIMESERIE_H
#define SCIQLOP_SPECTROGRAMTIMESERIE_H

#include "CoreGlobal.h"

#include <TimeSeries.h>
#include <cmath>

class SCIQLOP_CORE_EXPORT SpectrogramTimeSerie
    : public TimeSeries::TimeSerie<double, SpectrogramTimeSerie, 2>
{
public:
  double min_sampling = std::nan("");
  double max_sampling = std::nan("");
  bool y_is_log       = true;
  using item_t =
      decltype(std::declval<
               TimeSeries::TimeSerie<double, SpectrogramTimeSerie, 2>>()[0]);

  using iterator_t = decltype(
      std::declval<TimeSeries::TimeSerie<double, SpectrogramTimeSerie, 2>>()
          .begin());

  SpectrogramTimeSerie() {}
  SpectrogramTimeSerie(SpectrogramTimeSerie::axis_t&& t,
                       SpectrogramTimeSerie::axis_t&& y,
                       SpectrogramTimeSerie::data_t&& values,
                       std::vector<std::size_t>& shape, double min_sampling,
                       double max_sampling, bool y_is_log = true)
      : TimeSeries::TimeSerie<double, SpectrogramTimeSerie, 2>(t, values,
                                                               shape),
        min_sampling{min_sampling}, max_sampling{max_sampling}, y_is_log{
                                                                    y_is_log}
  {
    _axes[1] = y;
  }

  ~SpectrogramTimeSerie() = default;
  using TimeSerie::TimeSerie;
};

#endif // SCIQLOP_SPECTROGRAMTIMESERIE_H
