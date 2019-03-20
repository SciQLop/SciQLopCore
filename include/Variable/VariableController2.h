#ifndef VARIABLECONTROLLER2_H
#define VARIABLECONTROLLER2_H
#include "Data/DateTimeRange.h"

#include <Common/spimpl.h>
#include <Data/IDataProvider.h>
#include <QByteArray>
#include <QHash>
#include <QItemSelectionModel>
#include <QMutexLocker>
#include <QObject>
#include <QUuid>
#include <Variable/Variable2.h>
#include <memory>
#include <set>
#include <vector>

class SCIQLOP_CORE_EXPORT VariableController2 : public QObject
{
  class VariableController2Private;
  Q_OBJECT

  spimpl::unique_impl_ptr<VariableController2Private> impl;

public:
  explicit VariableController2();
  std::shared_ptr<Variable2>
  createVariable(const QString& name, const QVariantHash& metadata,
                 const std::shared_ptr<IDataProvider>& provider,
                 const DateTimeRange& range);

  std::shared_ptr<Variable2>
  cloneVariable(const std::shared_ptr<Variable2>& variable);
  void deleteVariable(const std::shared_ptr<Variable2>& variable);
  void changeRange(const std::shared_ptr<Variable2>& variable,
                   const DateTimeRange& r);
  void asyncChangeRange(const std::shared_ptr<Variable2>& variable,
                        const DateTimeRange& r);
  const std::vector<std::shared_ptr<Variable2>> variables();

  bool isReady(const std::shared_ptr<Variable2>& variable);
  bool isReady();

  void synchronize(const std::shared_ptr<Variable2>& var,
                   const std::shared_ptr<Variable2>& with);

  const std::vector<std::shared_ptr<Variable2>>
  variables(const std::vector<QUuid>& ids);

signals:
  void variableAdded(const std::shared_ptr<Variable2>&);
  void variableDeleted(const std::shared_ptr<Variable2>&);
};

#endif // VARIABLECONTROLLER2_H
