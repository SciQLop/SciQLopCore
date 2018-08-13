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
                                             std::shared_ptr<IDataProvider> provider, const DateTimeRange &range);

    void deleteVariable(std::shared_ptr<Variable> variable);
    void changeRange(std::shared_ptr<Variable> variable, DateTimeRange r);
    void asyncChangeRange(std::shared_ptr<Variable> variable, DateTimeRange r);
    const std::set<std::shared_ptr<Variable> > &variables();

    void synchronize(std::shared_ptr<Variable> var, std::shared_ptr<Variable> with);


signals:
    void variableAdded(std::shared_ptr<Variable>);
    void variableDeleted(std::shared_ptr<Variable>);

};
