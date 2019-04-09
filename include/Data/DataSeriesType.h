#ifndef SCIQLOP_DATASERIESTYPE_H
#define SCIQLOP_DATASERIESTYPE_H

#include <Data/MultiComponentTimeSerie.h>
#include <Data/ScalarTimeSerie.h>
#include <Data/SpectrogramTimeSerie.h>
#include <Data/VectorTimeSerie.h>
#include <QString>

enum class DataSeriesType
{
  NONE,
  SCALAR,
  VECTOR,
  MULTICOMPONENT,
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
    else if(type.toLower() == QStringLiteral("multicomponent"))
    {
      return DataSeriesType::MULTICOMPONENT;
    }
    else
    {
      return DataSeriesType::NONE;
    }
  }
  static DataSeriesType type(TimeSeries::ITimeSerie* ts)
  {
    if(!ts) return DataSeriesType::NONE;
    if(dynamic_cast<ScalarTimeSerie*>(ts)) return DataSeriesType::SCALAR;
    if(dynamic_cast<VectorTimeSerie*>(ts)) return DataSeriesType::VECTOR;
    if(dynamic_cast<MultiComponentTimeSerie*>(ts))
      return DataSeriesType::MULTICOMPONENT;
    if(dynamic_cast<SpectrogramTimeSerie*>(ts))
      return DataSeriesType::SPECTROGRAM;
    return DataSeriesType::NONE;
  }
};

#endif // SCIQLOP_DATASERIESTYPE_H
