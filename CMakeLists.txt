cmake_minimum_required(VERSION 3.6)
project(SciQLOPCore CXX)

OPTION (CPPCHECK "Analyzes the source code with cppcheck" OFF)
OPTION (CLANG_TIDY "Analyzes the source code with Clang Tidy" OFF)
OPTION (IWYU "Analyzes the source code with Include What You Use" OFF)

OPTION (Catalog "builds catalog API" ON)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
#https://gitlab.kitware.com/cmake/cmake/issues/15227
#set(CMAKE_AUTOUIC ON)
if(POLICY CMP0071)
    cmake_policy(SET CMP0071 OLD)
endif()
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Widgets Network PrintSupport Svg Test REQUIRED)

find_package(pybind11 CONFIG QUIET)
if (NOT pybind11_FOUND)
    execute_process(COMMAND git submodule init external/pybind11 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    execute_process(COMMAND git submodule update external/pybind11 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    add_subdirectory(external/pybind11)
endif()

find_package(libcatalogs CONFIG QUIET)
if (NOT catalogs_FOUND)
    execute_process(COMMAND git submodule init external/libcatalogs WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    execute_process(COMMAND git submodule update external/libcatalogs WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    add_subdirectory(external/libcatalogs)
endif()

macro(declare_test testname testexe sources libraries)
    add_executable(${testexe} ${sources})
    target_link_libraries(${testexe} ${libraries})
    add_test(NAME ${testname} COMMAND ${testexe})
endmacro(declare_test)

enable_testing()

FILE (GLOB_RECURSE core_SRCS
    ./include/DataSource/DataSourceItemMergeHelper.h
    ./include/DataSource/DataSourceItemAction.h
    ./include/DataSource/DataSourceItem.h
    ./include/DataSource/DataSourceController.h
    ./include/Common/SortUtils.h
    ./include/Common/spimpl.h
    ./include/Common/MimeTypesDef.h
    ./include/Common/MetaTypes.h
    ./include/Common/StringUtils.h
    ./include/Common/SignalWaiter.h
    ./include/Common/DateUtils.h
    ./include/Common/Numeric.h
    ./include/Common/deprecate.h
    ./include/Common/containers.h
    ./include/Common/debug.h
    ./include/Plugin/IPlugin.h
    ./include/Data/ArrayDataIterator.h
    ./include/Data/VariableRequest.h
    ./include/Data/VectorSeries.h
    ./include/Data/DateTimeRange.h
    ./include/Data/DateTimeRangeHelper.h
    ./include/Data/ScalarSeries.h
    ./include/Data/DataSeriesMergeHelper.h
    ./include/Data/DataSeries.h
    ./include/Data/AcquisitionDataPacket.h
    ./include/Data/DataSeriesType.h
    ./include/Data/AcquisitionRequest.h
    ./include/Data/SqpIterator.h
    ./include/Data/ArrayData.h
    ./include/Data/DataSeriesIterator.h
    ./include/Data/DataSeriesUtils.h
    ./include/Data/SpectrogramSeries.h
    ./include/Data/Unit.h
    ./include/Data/DataProviderParameters.h
    ./include/Data/OptionalAxis.h
    ./include/Data/IDataProvider.h
    ./include/Data/IDataSeries.h
    ./include/Network/NetworkController.h
    ./include/Network/Downloader.h
    ./include/Version.h
    ./include/CoreGlobal.h
    ./include/Visualization/VisualizationController.h
    ./include/PluginManager/PluginManager.h
    ./include/Variable/VariableModel2.h
    ./include/Variable/VariableCacheStrategy.h
    ./include/Variable/VariableSynchronizationGroup2.h
    ./include/Variable/ProportionalCacheStrategy.h
    ./include/Variable/SingleThresholdCacheStrategy.h
    ./include/Variable/VariableCacheStrategyFactory.h
    ./include/Variable/Variable.h
    ./include/Variable/VariableController2.h
    ./include/Variable/private/VCTransaction.h
    ./include/Time/TimeController.h
    ./include/Settings/ISqpSettingsBindable.h
    ./include/Settings/SqpSettingsDefs.h

    ./src/DataSource/DataSourceItem.cpp
    ./src/DataSource/DataSourceItemAction.cpp
    ./src/DataSource/DataSourceItemMergeHelper.cpp
    ./src/DataSource/DataSourceController.cpp
    ./src/Common/DateUtils.cpp
    ./src/Common/MimeTypesDef.cpp
    ./src/Common/StringUtils.cpp
    ./src/Common/SignalWaiter.cpp
    ./src/Data/ScalarSeries.cpp
    ./src/Data/DataSeriesIterator.cpp
    ./src/Data/OptionalAxis.cpp
    ./src/Data/ArrayDataIterator.cpp
    ./src/Data/SpectrogramSeries.cpp
    ./src/Data/DataSeriesUtils.cpp
    ./src/Data/VectorSeries.cpp
    ./src/Network/NetworkController.cpp
    ./src/Network/Downloader.cpp
    ./src/Visualization/VisualizationController.cpp
    ./src/PluginManager/PluginManager.cpp
    ./src/Variable/VariableController2.cpp
    ./src/Variable/VariableModel2.cpp
    ./src/Variable/VariableSynchronizationGroup2.cpp
    ./src/Variable/Variable.cpp
    ./src/Version.cpp
    ./src/Time/TimeController.cpp
    ./src/Settings/SqpSettingsDefs.cpp

        )


IF(Catalog)
    FILE (GLOB_RECURSE core_catalog_SRCS
        ./src/Catalogue/CatalogueController.cpp
        ./include/Catalogue/CatalogueController.h
        )
ELSE()
FILE (GLOB_RECURSE core_catalog_SRCS
    )
ENDIF(Catalog)

add_definitions(-DCORE_STATIC)
#add_definitions(-DHIDE_DEPRECATED)
add_definitions(-DSCIQLOP_CRASH_ON_ERROR)

add_library(sciqlopcore ${core_SRCS} ${core_catalog_SRCS})
SET_TARGET_PROPERTIES(sciqlopcore PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

target_include_directories(sciqlopcore PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/SciQLOP>
)

target_link_libraries(sciqlopcore PUBLIC
  Qt5::Core
  Qt5::Network
)

if(Catalog)
    target_link_libraries(sciqlopcore PUBLIC
      catalogs
    )
endif()


pybind11_add_module(sciqlopqt src/pybind11_wrappers/QtWrappers.cpp)
target_link_libraries(sciqlopqt PUBLIC Qt5::Core)

pybind11_add_module(pysciqlopcore src/pybind11_wrappers/CoreWrappers.cpp)
target_link_libraries(pysciqlopcore PUBLIC sciqlopcore)

add_library(pysciqlop src/pybind11_wrappers/pywrappers_common.h)
target_link_libraries(pysciqlop PUBLIC Qt5::Core)
target_include_directories(pysciqlop PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/pybind11_wrappers/>
    $<INSTALL_INTERFACE:include/SciQLOP/py_wrappers>
)

SET_PROPERTY(GLOBAL PROPERTY CORE_PYTHON_PATH  ${CMAKE_CURRENT_BINARY_DIR})


install(TARGETS sciqlopcore EXPORT SciQLOPCoreConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SciQLOP)
install(EXPORT SciQLOPCoreConfig DESTINATION share/SciQLOPCore/cmake)
export(TARGETS sciqlopcore FILE SciQLOPCoreConfig.cmake)

add_subdirectory(tests)
