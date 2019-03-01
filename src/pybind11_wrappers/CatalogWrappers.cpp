#include "pywrappers_common.h"
#include <QDate>
#include <QString>
#include <QTime>
#include <QUuid>
#include <QVector>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <sstream>
#include <string>

#include <Catalogue/CatalogueController.h>
//#include <DBCatalogue.h>
//#include <DBEvent.h>
//#include <DBEventProduct.h>


namespace py = pybind11;

//std::ostream& operator<<(std::ostream& os, const DBEvent& e)
//{
//    os << std::endl;
//    return os;
//}

#define JAVA_LIKE_PROPERTY(name, className)                                                        \
    .def_property("##name", &className::get##name, &className::set##name)

PYBIND11_MODULE(pysciqlopcatalogs, m)
{

    //    py::class_<DBEventProduct, std::shared_ptr<DBEventProduct>>(m,"DBEventProduct")
    //            JAVA_LIKE_PROPERTY(TStart, DBEventProduct)
    //            JAVA_LIKE_PROPERTY(TEnd, DBEventProduct)
    //            JAVA_LIKE_PROPERTY(ProductId, DBEventProduct)
    //            JAVA_LIKE_PROPERTY(CreationDateTime, DBEventProduct)
    //            JAVA_LIKE_PROPERTY(ModificationDateTime, DBEventProduct)
    //            JAVA_LIKE_PROPERTY(Event, DBEventProduct)
    //            ;

    //    py::class_<DBEvent, std::shared_ptr<DBEvent>>(m, "DBEvent")
    //            JAVA_LIKE_PROPERTY(Name,DBEvent)
    //            JAVA_LIKE_PROPERTY(UniqId,DBEvent)
    //            JAVA_LIKE_PROPERTY(Author,DBEvent)
    //            JAVA_LIKE_PROPERTY(Repository,DBEvent)
    //            JAVA_LIKE_PROPERTY(CreationDateTime,DBEvent)
    //            JAVA_LIKE_PROPERTY(ModificationDateTime,DBEvent)
    //            JAVA_LIKE_PROPERTY(EventProducts,DBEvent)
    //            .def_property_readonly("TStart", &DBEvent::getTStart)
    //            .def_property_readonly("TEnd", &DBEvent::getTEnd)
    //            .def("__repr__",__repr__<DBEvent>);

    //    py::class_<DBCatalogue, std::shared_ptr<DBCatalogue>>(m,"DBEventProduct")
    //            JAVA_LIKE_PROPERTY(CatalogueId, DBCatalogue)
    //            JAVA_LIKE_PROPERTY(UniqId, DBCatalogue)
    //            JAVA_LIKE_PROPERTY(Name, DBCatalogue)
    //            JAVA_LIKE_PROPERTY(Author, DBCatalogue)
    //            JAVA_LIKE_PROPERTY(Repository, DBCatalogue)
    //            JAVA_LIKE_PROPERTY(CreationDateTime, DBCatalogue)
    //            JAVA_LIKE_PROPERTY(ModificationDateTime, DBCatalogue)
    //            ;

    //    py::class_<CatalogueController>(m, "CatalogueController")
    //            .def("addDB", &CatalogueController::addDB)
    //            .def("saveDB", &CatalogueController::saveDB)
    //            .def("addEvent", &CatalogueController::addEvent)
    //            .def("updateEvent", &CatalogueController::updateEvent)
    //            .def("updateEventProduct", &CatalogueController::updateEventProduct)
    //            .def("removeEvent", &CatalogueController::removeEvent)
    //            .def("saveEvent", &CatalogueController::saveEvent)
    //            .def("discardEvent", &CatalogueController::discardEvent)
    //            .def("eventHasChanges", &CatalogueController::eventHasChanges)
    //            .def("addCatalogue", &CatalogueController::addCatalogue)
    //            .def("updateCatalogue", &CatalogueController::updateCatalogue)
    //            .def("removeCatalogue", &CatalogueController::removeCatalogue)
    //            .def("saveCatalogue", &CatalogueController::saveCatalogue)
    //            .def("discardCatalogue", &CatalogueController::discardCatalogue)
    //            .def("saveAll", &CatalogueController::saveAll)
    //            .def("hasChanges", &CatalogueController::hasChanges)
    //            ;
}
