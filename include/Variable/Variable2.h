#ifndef SCIQLOP_VARIABLE2_H
#define SCIQLOP_VARIABLE2_H

#include "CoreGlobal.h"

#include <Common/MetaTypes.h>
#include <Common/deprecate.h>
#include <Common/spimpl.h>
#include <Data/DataSeriesType.h>
#include <Data/DateTimeRange.h>
#include <Data/ScalarTimeSerie.h>
#include <Data/SpectrogramTimeSerie.h>
#include <Data/VectorTimeSerie.h>
#include <QDataStream>
#include <QObject>
#include <QReadWriteLock>
#include <QUuid>
#include <TimeSeries.h>
#include <optional>

class SCIQLOP_CORE_EXPORT Variable2 : public QObject
{
  Q_OBJECT

public:
  explicit Variable2(const QString& name, const QVariantHash& metadata = {});

  /// Copy ctor
  explicit Variable2(const Variable2& other);

  std::shared_ptr<Variable2> clone() const;

  QString name();
  void setName(const QString& name);
  DateTimeRange range();
  void setRange(const DateTimeRange& range);
  std::optional<DateTimeRange> realRange();

  std::size_t nbPoints();

  /// @return the data of the variable, nullptr if there is no data
  std::shared_ptr<TimeSeries::ITimeSerie> data();

  /// @return the type of data that the variable holds
  DataSeriesType type();

  QVariantHash metadata() const noexcept;

  //  void setData(const std::vector<AnyTimeSerie*>& dataSeries,
  //               const DateTimeRange& range, bool notify = true);

  void setData(const std::vector<TimeSeries::ITimeSerie*>& dataSeries,
               const DateTimeRange& range, bool notify = true);

  static QByteArray
  mimeData(const std::vector<std::shared_ptr<Variable2>>& variables)
  {
    auto encodedData = QByteArray{};
    QDataStream stream{&encodedData, QIODevice::WriteOnly};
    for(auto& var : variables)
    {
      stream << var->ID().toByteArray();
    }
    return encodedData;
  }

  static std::vector<QUuid> IDs(QByteArray mimeData)
  {
    std::vector<QUuid> variables;
    QDataStream stream{mimeData};

    QVariantList ids;
    stream >> ids;
    std::transform(std::cbegin(ids), std::cend(ids),
                   std::back_inserter(variables),
                   [](const auto& id) { return id.toByteArray(); });
    return variables;
  }

  operator QUuid() { return _uuid; }
  QUuid ID() { return _uuid; }
signals:
  void updated(QUuid ID);

private:
  struct VariablePrivate;
  spimpl::unique_impl_ptr<VariablePrivate> impl;
  QUuid _uuid;
  QReadWriteLock m_lock;
};

// Required for using shared_ptr in signals/slots
// SCIQLOP_REGISTER_META_TYPE(VARIABLE_PTR_REGISTRY, std::shared_ptr<Variable2>)
// SCIQLOP_REGISTER_META_TYPE(VARIABLE_PTR_VECTOR_REGISTRY,
//                           QVector<std::shared_ptr<Variable2>>)

#endif // SCIQLOP_VARIABLE2_H
