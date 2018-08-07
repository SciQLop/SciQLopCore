#ifndef SCIQLOP_VARIABLESYNCHRONIZATIONGROUP2_H
#define SCIQLOP_VARIABLESYNCHRONIZATIONGROUP2_H

#include <QUuid>
#include <set>

#include "CoreGlobal.h"
#include <Common/spimpl.h>
#include <Common/containers.h>

class SCIQLOP_CORE_EXPORT VariableSynchronizationGroup2
{

public:
    explicit VariableSynchronizationGroup2()=default;
    // Convenience ctor to build a group with a default variable
    explicit VariableSynchronizationGroup2(QUuid variable)
        :_variables{{variable}}
    {}

    void addVariable(QUuid variable);
    void removeVariable(QUuid variable);

    bool contains(QUuid variable)
    {
        return SciQLop::containers::contains(this->_variables,variable);
    }

    const std::set<QUuid> &variables() const noexcept;

private:
    std::set<QUuid> _variables;
};

#endif // SCIQLOP_VARIABLESYNCHRONIZATIONGROUP2_H
