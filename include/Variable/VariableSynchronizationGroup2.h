#ifndef SCIQLOP_VARIABLESYNCHRONIZATIONGROUP2_H
#define SCIQLOP_VARIABLESYNCHRONIZATIONGROUP2_H

#include <QUuid>
#include <set>

#include "CoreGlobal.h"
#include <Common/spimpl.h>
#include <containers/algorithms.hpp>

/**
 * @brief The VariableSynchronizationGroup2 class holds a list of Variables uuid which are synchronized
 * @note This class is part of SciQLop internals, as a normal user you shouldn't have to care about it
 */
class SCIQLOP_CORE_EXPORT VariableSynchronizationGroup2
{

public:
    explicit VariableSynchronizationGroup2()=default;
    /**
     * @brief VariableSynchronizationGroup2 is a convenience ctor to build a group with a default variable
     * @param variable
     */
    explicit VariableSynchronizationGroup2(QUuid variable)
        :_variables{{variable}}
    {}

    /**
     * @brief addVariable adds the given variable to the group, does nothing if the varaible is alredy in the group
     * @param variable
     * @sa removeVariable
     */
    void addVariable(QUuid variable) noexcept
    {
        this->_variables.insert(variable);
    }

    /**
     * @brief removeVariable removes the given variable from the group, does nothing if the varaible is not in the group
     * @param variable
     * @sa addVariable
     */
    void removeVariable(QUuid variable) noexcept
    {
        this->_variables.erase(variable);
    }

    /**
     * @brief contains checks if the given variable is in the group
     * @param variable
     * @return true if the variable is in the group
     */
    bool contains(QUuid variable) const noexcept
    {
        return cpp_utils::containers::contains(this->_variables,variable);
    }

    /**
     * @brief variables
     * @return the list of synchronized variables in this group as a std::set
     */
    const std::set<QUuid> &variables() const noexcept
    {
        return this->_variables;
    }

    inline bool isEmpty()
    {
        return _variables.size()==0;
    }

    inline QUuid ID(){return _ID;}

    operator QUuid() {return _ID;}
private:
    std::set<QUuid> _variables;
    QUuid _ID = QUuid::createUuid();
};

#endif // SCIQLOP_VARIABLESYNCHRONIZATIONGROUP2_H
