TestUtils = library('TestUtils', 'TestUtils/TestProviders.h', 'TestUtils/TestProviders.cpp',
                       dependencies : [sciqlop_core, qt5test]
                       )


TestUtils_dep = declare_dependency(link_with : TestUtils,
                                dependencies : [sciqlop_core, qt5test])


DataSeriesTestsUtils = library('DataSeriesTestsUtils', 'Data/DataSeriesBuilders.h', 'Data/DataSeriesBuilders.cpp',
                    'Data/DataSeriesTestsUtils.h',
                    'Data/DataSeriesTestsUtils.cpp',
                       dependencies : [sciqlop_core, qt5test]
                       )


DataSeriesTestsUtils_dep = declare_dependency(link_with : DataSeriesTestsUtils,
                                dependencies : [sciqlop_core, qt5test])



tests = [
    {
        'name':'TestStringUtils',
        'sources': ['Common/TestStringUtils.cpp'],
        'deps': [sciqlop_core, qt5test]
    },
    {
        'name':'TestContainers',
        'sources': ['Common/TestContainers.cpp'],
        'deps': [sciqlop_core, qt5test]
    },
    {
        'name':'TestSyncGroup',
        'sources': ['Variable/TestSyncGroup.cpp'],
        'deps': [sciqlop_core, qt5test]
    },
    {
        'name':'TestDateTimeRange',
        'sources': ['Data/TestDateTimeRange.cpp'],
        'deps': [sciqlop_core, qt5test]
    },
    {
        'name':'TestDataSeriesUtils',
        'sources': ['Data/TestDataSeriesUtils.cpp'],
        'deps': [sciqlop_core, qt5test]
    },
    {
        'name':'TestOptionalAxis',
        'sources': ['Data/TestOptionalAxis.cpp'],
        'deps': [sciqlop_core, qt5test]
    },
    {
        'name':'TestSpectrogramSeries',
        'sources': [
                    'Data/TestSpectrogramSeries.cpp',
                    ],
        'deps': [sciqlop_core, qt5test, DataSeriesTestsUtils_dep]
    },
    {
        'name':'TestOneDimArrayData',
        'sources': ['Data/TestOneDimArrayData.cpp'],
        'deps': [sciqlop_core, qt5test]
    },
    {
        'name':'TestScalarSeries',
        'sources': [
                    'Data/TestScalarSeries.cpp',
                   ],
        'deps': [sciqlop_core, qt5test, DataSeriesTestsUtils_dep]
    },
    {
        'name':'TestTwoDimArrayData',
        'sources': ['Data/TestTwoDimArrayData.cpp'],
        'deps': [sciqlop_core, qt5test]
    },
    {
        'name':'TestVectorSeries',
        'sources': [
                    'Data/TestVectorSeries.cpp',
                   ],
        'deps': [sciqlop_core, qt5test, DataSeriesTestsUtils_dep]
    },
    {
        'name':'TestDataSourceController',
        'sources': [
                    'DataSource/TestDataSourceController.cpp',
                    'DataSource/DataSourceItemBuilder.cpp'
                   ],
        'deps': [sciqlop_core, qt5test]
    },
    {
        'name':'TestDataSourceItem',
        'sources': [
                    'DataSource/TestDataSourceItem.cpp',
                    'DataSource/DataSourceItemBuilder.cpp'
                   ],
        'deps': [sciqlop_core, qt5test]
    },
    {
        'name':'TestVariable',
        'sources': ['Variable/TestVariable.cpp'],
        'deps': [sciqlop_core, qt5test]
    },
    {
        'name':'TestDownloader',
        'sources': ['Network/TestDownloader.cpp'],
        'deps': [sciqlop_core, qt5test, qt5Concurrent]
    },
    {
        'name':'TestVariableController2',
        'sources': ['Variable/TestVariableController2.cpp'],
        'deps': [sciqlop_core, qt5test, qt5Concurrent, TestUtils_dep]
    },
    {
        'name':'TestVariableController2Async',
        'sources': ['Variable/TestVariableController2Async.cpp'],
        'deps': [sciqlop_core, qt5test, qt5Concurrent, TestUtils_dep]
    },
    {
        'name':'TestVariableController2WithSync',
        'sources': ['Variable/TestVariableController2WithSync.cpp'],
        'deps': [sciqlop_core, qt5test, qt5Concurrent, TestUtils_dep]
    }
]

foreach unit_test : tests
  test_moc_files = qt5.preprocess(moc_sources : unit_test['sources'])
  test_exe = executable(unit_test['name'],unit_test['sources'] , test_moc_files,
                          dependencies : unit_test['deps'],
						  cpp_args : ['-DCORE_TESTS_RESOURCES_DIR="'+meson.current_source_dir()+'/../tests-resources"']
						  )
  test('Test-' + unit_test['name'], test_exe, args: ['-teamcity', '-o', '@0@.teamcity.txt'.format(unit_test['name'])])
endforeach

