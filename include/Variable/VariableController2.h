#ifndef VARIABLECONTROLLER2_H
#define VARIABLECONTROLLER2_H
#include <memory>
#include <vector>
#include <set>
#include <QHash>
#include <QObject>
#include <QMutexLocker>
#include <QUuid>
#include <QByteArray>
#include <QItemSelectionModel>
#include <Common/spimpl.h>
#include <Variable/Variable.h>
#include <Data/IDataProvider.h>
#include "Data/DateTimeRange.h"

class SCIQLOP_CORE_EXPORT VariableController2: public QObject
{
    class VariableController2Private;
    Q_OBJECT

    spimpl::unique_impl_ptr<VariableController2Private> impl;

public:
    explicit VariableController2();
    std::shared_ptr<Variable> createVariable(const QString &name, const QVariantHash &metadata,
                                             const std::shared_ptr<IDataProvider>& provider,
                                             const DateTimeRange &range);

    std::shared_ptr<Variable> cloneVariable(const std::shared_ptr<Variable>& variable);
    void deleteVariable(const std::shared_ptr<Variable>& variable);
    void changeRange(const std::shared_ptr<Variable>& variable, const DateTimeRange& r);
    void asyncChangeRange(const std::shared_ptr<Variable>& variable, const DateTimeRange& r);
    const std::vector<std::shared_ptr<Variable>> variables();

    bool isReady(const std::shared_ptr<Variable>& variable);
    bool isReady();


    void synchronize(const std::shared_ptr<Variable>& var, const std::shared_ptr<Variable>& with);

    const std::vector<std::shared_ptr<Variable>> variables(const std::vector<QUuid>& ids);


signals:
    void variableAdded(const std::shared_ptr<Variable>&);
    void variableDeleted(const std::shared_ptr<Variable>&);

};

#endif //VARIABLECONTROLLER2_H
