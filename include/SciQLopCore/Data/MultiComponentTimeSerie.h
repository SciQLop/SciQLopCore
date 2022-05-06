#pragma once


#include <TimeSeries.h>

class MultiComponentTimeSerie
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
