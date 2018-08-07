#ifndef CONTAINERS_H
#define CONTAINERS_H
#include <algorithm>

namespace SciQLop::containers {

template <class T1,class T2>
bool contains(const T1 &container, const T2 &value)
{
    return std::find(container.begin(), container.end(), value) != container.end();
}

}


#endif // CONTAINERS_H
