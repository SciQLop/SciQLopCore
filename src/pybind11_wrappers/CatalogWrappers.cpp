#include "pywrappers_common.h"

#include <Catalogue.hpp>
#include <Catalogue/CatalogueController.h>
#include <Event.hpp>
#include <QDate>
#include <QString>
#include <QTime>
#include <QUuid>
#include <QVector>
#include <Repository.hpp>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <sstream>
#include <string>

namespace py = pybind11;

// std::ostream& operator<<(std::ostream& os, const DBEvent& e)
//{
//    os << std::endl;
//    return os;
//}

PYBIND11_MODULE(pysciqlopcatalogs, m)
{
  py::class_<CatalogueController::Product_t>(m, "Product")
      .def_readwrite("name", &CatalogueController::Product_t::name)
      .def_readwrite("start_time", &CatalogueController::Product_t::startTime)
      .def_readwrite("stop_time", &CatalogueController::Product_t::stopTime);

  py::class_<CatalogueController::Event_t, CatalogueController::Event_ptr>(
      m, "Event")
      .def_readwrite("name", &CatalogueController::Event_t::name)
      .def_readwrite("tags", &CatalogueController::Event_t::tags)
      .def_readwrite("products", &CatalogueController::Event_t::products)
      .def_readonly("uuid", &CatalogueController::Event_t::uuid)
      .def_property_readonly("start_time",
                             &CatalogueController::Event_t::startTime)
      .def_property_readonly("stop_time",
                             &CatalogueController::Event_t::stopTime)
      .def(py::self == py::self)
      .def(py::self != py::self);

  py::class_<CatalogueController::Catalogue_t,
             CatalogueController::Catalogue_ptr>(m, "Catalogue")
      .def_readwrite("name", &CatalogueController::Catalogue_t::name)
      .def_readonly("uuid", &CatalogueController::Catalogue_t::uuid)
      .def_property_readonly("start_time",
                             &CatalogueController::Catalogue_t::startTime)
      .def_property_readonly("stop_time",
                             &CatalogueController::Catalogue_t::stopTime)
      .def("add", &CatalogueController::Catalogue_t::add)
      .def("remove", py::overload_cast<CatalogueController::Event_ptr&>(
                         &CatalogueController::Catalogue_t::remove))
      .def("remove", py::overload_cast<const CatalogueController::uuid_t&>(
                         &CatalogueController::Catalogue_t::remove))
      .def("event",
           [](const CatalogueController::Catalogue_t& catalogue,
              const CatalogueController::uuid_t& uuid) {
             return catalogue.event(uuid);
           })
      .def("__contains__", &CatalogueController::Catalogue_t::contains);

  py::class_<CatalogueController>(m, "CatalogueController")
      .def("load_repository", &CatalogueController::loadRepository)
      .def("save_repository", &CatalogueController::saveRepository)
      .def("events", py::overload_cast<>(&CatalogueController::events))
      .def("events",
           py::overload_cast<const QString&>(&CatalogueController::events))
      .def("events",
           py::overload_cast<const CatalogueController::Catalogue_ptr&>(
               &CatalogueController::events))
      .def("catalogues", py::overload_cast<>(&CatalogueController::catalogues))
      .def("catalogues",
           py::overload_cast<const QString&>(&CatalogueController::catalogues))
      .def("has_unsaved_changes", &CatalogueController::hasUnsavedChanges)
      .def("repository", py::overload_cast<CatalogueController::Event_ptr>(
                             &CatalogueController::repository))
      .def("repository", py::overload_cast<CatalogueController::Catalogue_ptr>(
                             &CatalogueController::repository))
      .def("save", py::overload_cast<CatalogueController::Event_ptr>(
                       &CatalogueController::save))
      .def("save", py::overload_cast<CatalogueController::Catalogue_ptr>(
                       &CatalogueController::save))
      .def("save",
           py::overload_cast<const QString&>(&CatalogueController::save))

      .def("add", py::overload_cast<const QString&>(&CatalogueController::add))
      .def("add", py::overload_cast<const QString&, const QString&>(
                      &CatalogueController::add))
      .def("add", py::overload_cast<CatalogueController::Event_ptr,
                                    CatalogueController::Catalogue_ptr>(
                      &CatalogueController::add))
      .def("add",
           py::overload_cast<CatalogueController::Event_ptr, const QString&>(
               &CatalogueController::add))
      .def("add", [](CatalogueController& ctrlr,
                     CatalogueController::Event_ptr event) {
        return ctrlr.add(event);
      });
}
