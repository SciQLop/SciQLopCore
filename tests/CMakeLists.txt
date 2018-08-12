add_definitions(-DCORE_TESTS_RESOURCES_DIR="${CMAKE_CURRENT_LIST_DIR}/../tests-resources")

FILE (GLOB_RECURSE  TestUtilsSources
    TestUtils/TestProviders.h
    TestUtils/TestProviders.cpp
)

add_library(TestUtils ${TestUtilsSources})
target_link_libraries(TestUtils sciqlopcore Qt5::Test)

declare_test(TestStringUtils TestStringUtils Common/TestStringUtils.cpp "sciqlopcore;Qt5::Test")

declare_test(TestContainers TestContainers Common/TestContainers.cpp "sciqlopcore;Qt5::Test")
declare_test(TestSyncGroup TestSyncGroup Variable/TestSyncGroup.cpp "sciqlopcore;Qt5::Test")


declare_test(TestDataSeriesUtils TestDataSeriesUtils Data/TestDataSeriesUtils.cpp "sciqlopcore;Qt5::Test")
declare_test(TestOptionalAxis TestOptionalAxis Data/TestOptionalAxis.cpp "sciqlopcore;Qt5::Test")
declare_test(TestSpectrogramSeries TestSpectrogramSeries
    "Data/TestSpectrogramSeries.cpp;Data/DataSeriesBuilders.h;Data/DataSeriesBuilders.cpp;Data/DataSeriesTestsUtils.h;Data/DataSeriesTestsUtils.cpp"
    "sciqlopcore;Qt5::Test")
declare_test(TestOneDimArrayData TestOneDimArrayData Data/TestOneDimArrayData.cpp "sciqlopcore;Qt5::Test")
declare_test(TestScalarSeries TestScalarSeries
    "Data/TestScalarSeries.cpp;Data/DataSeriesBuilders.h;Data/DataSeriesBuilders.cpp;Data/DataSeriesTestsUtils.h;Data/DataSeriesTestsUtils.cpp"
    "sciqlopcore;Qt5::Test")
declare_test(TestTwoDimArrayData TestTwoDimArrayData Data/TestTwoDimArrayData.cpp "sciqlopcore;Qt5::Test")
declare_test(TestVectorSeries TestVectorSeries
    "Data/TestVectorSeries.cpp;Data/DataSeriesBuilders.h;Data/DataSeriesBuilders.cpp;Data/DataSeriesTestsUtils.h;Data/DataSeriesTestsUtils.cpp"
    "sciqlopcore;Qt5::Test")

declare_test(TestDataSourceController TestDataSourceController
    "DataSource/TestDataSourceController.cpp;DataSource/DataSourceItemBuilder.cpp"
    "sciqlopcore;Qt5::Test")
declare_test(TestDataSourceItem TestDataSourceItem
    "DataSource/TestDataSourceItem.cpp;DataSource/DataSourceItemBuilder.cpp"
    "sciqlopcore;Qt5::Test")

declare_test(TestVariable TestVariable Variable/TestVariable.cpp "sciqlopcore;Qt5::Test")
declare_test(TestVariableCacheController TestVariableCacheController Variable/TestVariableCacheController.cpp "sciqlopcore;Qt5::Test")
declare_test(TestVariableController TestVariableController Variable/TestVariableController.cpp "sciqlopcore;Qt5::Test")
declare_test(TestVariableSync TestVariableSync Variable/TestVariableSync.cpp "sciqlopcore;Qt5::Test")

declare_test(TestDownloader TestDownloader Network/TestDownloader.cpp "sciqlopcore;Qt5::Test")


declare_test(TestVariableController2 TestVariableController2 Variable/TestVariableController2.cpp "sciqlopcore;TestUtils;Qt5::Test")
