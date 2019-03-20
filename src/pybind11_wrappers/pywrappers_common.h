#ifndef PYWRAPPERS_COMMON_H
#define PYWRAPPERS_COMMON_H
#include <QString>
#include <QUuid>
#include <pybind11/pybind11.h>
#include <sstream>
#include <string>

template<typename T> std::string __repr__(T& obj)
{
  std::stringstream sstr;
  sstr << obj;
  return sstr.str();
}

#endif // PYWRAPPERS_COMMON_H
