#include <Common/spimpl.h>
#include <memory>
#include <vector>
#include <QHash>
#include <Variable/Variable.h>
#include <Variable/VariableSynchronizationGroup.h>
#include <Variable/VariableModel.h>
#include <Data/IDataProvider.h>
#include "Data/DateTimeRange.h"
#include <QMutexLocker>
#include <QUuid>
#include <QItemSelectionModel>

class VariableController2Private;
class VariableController2
{
    spimpl::unique_impl_ptr<VariableController2Private> impl;

public:
    explicit VariableController2();
    std::shared_ptr<Variable> createVariable(const QString &name, const QVariantHash &metadata,
                                             std::shared_ptr<IDataProvider> provider, const DateTimeRange &range);
    void changeRange(QUuid variable, DateTimeRange r);
    void asyncChangeRange(QUuid variable, DateTimeRange r);

};
