#ifndef SCIQLOP_DATASERIESTYPE_H
#define SCIQLOP_DATASERIESTYPE_H

#include <QString>

enum class DataSeriesType
{
  NONE,
  SCALAR,
  VECTOR,
  SPECTROGRAM
};

struct DataSeriesTypeUtils
{
  static DataSeriesType fromString(const QString& type)
  {
    if(type.toLower() == QStringLiteral("scalar"))
    { return DataSeriesType::SCALAR; }
    else if(type.toLower() == QStringLiteral("spectrogram"))
    {
      return DataSeriesType::SPECTROGRAM;
    }
    else if(type.toLower() == QStringLiteral("vector"))
    {
      return DataSeriesType::VECTOR;
    }
    else
    {
      return DataSeriesType::NONE;
    }
  }
};

#endif // SCIQLOP_DATASERIESTYPE_H
