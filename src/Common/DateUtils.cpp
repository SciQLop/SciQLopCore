#include "Common/DateUtils.h"

#include <cmath>

QDateTime DateUtils::dateTime(double secs, Qt::TimeSpec timeSpec) noexcept
{
  // Uses msecs to be Qt 4 compatible
  if(!std::isnan(secs))
    return QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(secs * 1000.),
                                          timeSpec);
  return QDateTime();
}

double DateUtils::secondsSinceEpoch(const QDateTime& dateTime) noexcept
{
  // Uses msecs to be Qt 4 compatible
  return dateTime.toMSecsSinceEpoch() / 1000.;
}
