#include <memory>
#include <vector>
#include <QHash>
#include <QObject>
#include <QMutexLocker>
#include <QUuid>
#include <QItemSelectionModel>
#include <Common/spimpl.h>
#include <Variable/Variable.h>
#include <Variable/VariableSynchronizationGroup.h>
#include <Variable/VariableModel.h>
#include <Data/IDataProvider.h>
#include "Data/DateTimeRange.h"

class VariableController2: public QObject
{
    class VariableController2Private;
    Q_OBJECT

    spimpl::unique_impl_ptr<VariableController2Private> impl;

public:
    explicit VariableController2();
    std::shared_ptr<Variable> createVariable(const QString &name, const QVariantHash &metadata,
                                             const std::shared_ptr<IDataProvider>& provider,
                                             const DateTimeRange &range);

    void deleteVariable(const std::shared_ptr<Variable>& variable);
    void changeRange(const std::shared_ptr<Variable>& variable, const DateTimeRange& r);
    void asyncChangeRange(const std::shared_ptr<Variable>& variable, const DateTimeRange& r);
    const std::set<std::shared_ptr<Variable>> variables();

    void synchronize(const std::shared_ptr<Variable>& var, const std::shared_ptr<Variable>& with);


signals:
    void variableAdded(const std::shared_ptr<Variable>&);
    void variableDeleted(const std::shared_ptr<Variable>&);

};
