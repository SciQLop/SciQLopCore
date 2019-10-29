TestUtils = library('TestUtils', 'TestUtils/TestProviders.h', 'TestUtils/TestProviders.cpp',
                       dependencies : [sciqlop_core, qt5test]
                       )


TestUtils_dep = declare_dependency(link_with : TestUtils,
                                include_directories: include_directories('TestUtils'),
                                dependencies : [sciqlop_core, qt5test])

tests = [
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
        'name':'TestDataSourceController',
        'sources': [
                    'DataSource/TestDataSourceController.cpp',
                    'DataSource/DataSourceItemBuilder.cpp'
                   ],
        'deps': [sciqlop_core, qt5test]
    },
    {
        'name':'TestDataSources',
        'sources': [
                    'DataSource/TestDataSources.cpp'
                   ],
        'deps': [sciqlop_core, qt5test, TestUtils_dep]
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
  if get_option('teamcity')
    test_exe_args = ['-teamcity', '-o', '@0@.teamcity.txt'.format(unit_test['name'])]
  else
    test_exe_args = []
  endif
  test('Test-' + unit_test['name'], test_exe, args: test_exe_args)

endforeach

datasources_manual_test_exe = executable('datasources_manual_test', 'DataSource/DataSourcesManualTest.cpp', dependencies :[sciqlop_core,
TestUtils_dep, qt5gui, qt5widgets])
