sciqlopcore_moc_headers = files(
    '../include/SciQLopCore/Data/IDataProvider.h',
    '../include/SciQLopCore/Common/SignalWaiter.h',
    '../include/SciQLopCore/DataSource/DataSourceItemAction.h',
    '../include/SciQLopCore/DataSource/datasources.h',
    '../include/SciQLopCore/GUI/MainWindow.h',
    '../include/SciQLopCore/SqpApplication.h'
)

sciqlopcore_moc_sources = files(
    'Common/SignalWaiter.cpp',
    'DataSource/DataSourceItemAction.cpp',
    'DataSource/datasources.cpp',
    'GUI/MainWindow.cpp',
    'SqpApplication.cpp'
)

sciqlopcore_moc_gen = qtmod.compile_moc(
                                        sources: sciqlopcore_moc_sources,
                                        headers: sciqlopcore_moc_headers)

sciqlopcore_headers = files(
    '../include/SciQLopCore/Data/DataProviderParameters.h',
    '../include/SciQLopCore/Data/DataSeriesType.h',
    '../include/SciQLopCore/Data/DateTimeRange.h',
    '../include/SciQLopCore/Data/DateTimeRangeHelper.h',
    '../include/SciQLopCore/Data/MultiComponentTimeSerie.h',
    '../include/SciQLopCore/Data/ScalarTimeSerie.h',
    '../include/SciQLopCore/Data/SpectrogramTimeSerie.h',
    '../include/SciQLopCore/Data/SqpIterator.h',
    '../include/SciQLopCore/Data/TimeSeriesUtils.h',
    '../include/SciQLopCore/Data/VectorTimeSerie.h',
    '../include/SciQLopCore/Common/DateUtils.h',
    '../include/SciQLopCore/Common/debug.h',
    '../include/SciQLopCore/Common/MetaTypes.h',
    '../include/SciQLopCore/Common/SortUtils.h',
    '../include/SciQLopCore/DataSource/DataSourceItem.h',
    '../include/SciQLopCore/DataSource/DataSourceItemMergeHelper.h',
    '../include/SciQLopCore/MimeTypes/MimeTypes.h'
)

sciqlopcore_sources = files(
    'Common/DateUtils.cpp',
    'Common/SignalWaiter.cpp',
    'DataSource/DataSourceItem.cpp',
    'DataSource/DataSourceItemMergeHelper.cpp',
    'DataSource/DataSourceItemAction.cpp',
    'DataSource/datasources.cpp',
    'GUI/MainWindow.cpp',
    'SqpApplication.cpp'
)

sciqlopcore_lib = library('sciqlopcore',
                          sciqlopcore_moc_gen,
                          sciqlopcore_sources,
                          dependencies: [time_series_dep, cpp_utils_dep, qt_core, qt_widgets, qt_gui],
                          include_directories:'../include',
                          extra_files:[sciqlopcore_headers,sciqlopcore_moc_headers]
                         )

sciqlopcore_dep = declare_dependency( link_with: sciqlopcore_lib,
                                      include_directories:'../include',
                                      dependencies: [time_series_dep, cpp_utils_dep, qt_core, qt_widgets, qt_gui]
                                    )
