
catalogicpp_dep = dependency('catalogicpp', required : true, fallback:['catalogicpp','catalogicpp_dep'])
pybind11_dep = dependency('pybind11', required : true, fallback:['pybind11','pybind11_dep'])


core_moc_headers = [
 './include/Common/containers.h',
 './include/Common/StringUtils.h',
 './include/Common/Numeric.h',
 './include/Common/spimpl.h',
 './include/Common/DateUtils.h',
 './include/Common/MimeTypesDef.h',
 './include/Common/SignalWaiter.h',
 './include/Common/deprecate.h',
 './include/Common/debug.h',
 './include/Common/MetaTypes.h',
 './include/Common/cpp_utils.h',
 './include/Common/SortUtils.h',
 './include/Data/DataSeries.h',
 './include/Data/DataSeriesIterator.h',
 './include/Data/DateTimeRangeHelper.h',
 './include/Data/SpectrogramSeries.h',
 './include/Data/DateTimeRange.h',
 './include/Data/VectorSeries.h',
 './include/Data/DataProviderParameters.h',
 './include/Data/ArrayData.h',
 './include/Data/Unit.h',
 './include/Data/ScalarSeries.h',
 './include/Data/SqpIterator.h',
 './include/Data/IDataProvider.h',
 './include/Data/DataSeriesUtils.h',
 './include/Data/OptionalAxis.h',
 './include/Data/IDataSeries.h',
 './include/Data/DataSeriesType.h',
 './include/Data/ArrayDataIterator.h',
 './include/Data/DataSeriesMergeHelper.h',
 './include/CoreGlobal.h',
 './include/Network/NetworkController.h',
 './include/Network/Response.h',
 './include/Network/Downloader.h',
 './include/Settings/SqpSettingsDefs.h',
 './include/Settings/ISqpSettingsBindable.h',
 './include/DataSource/DataSourceController.h',
 './include/DataSource/DataSourceItem.h',
 './include/DataSource/DataSourceItemAction.h',
 './include/DataSource/DataSourceItemMergeHelper.h',
 './include/Time/TimeController.h',
 './include/PluginManager/PluginManager.h',
 './include/Version.h',
 './include/Catalogue/CatalogueController.h',
 './include/Plugin/IPlugin.h',
 './include/Variable/VariableModel2.h',
 './include/Variable/Variable.h',
 './include/Variable/VariableCacheStrategy.h',
 './include/Variable/VariableController2.h',
 './include/Variable/VariableCacheStrategyFactory.h',
 './include/Variable/VariableSynchronizationGroup2.h',
 './include/Variable/ProportionalCacheStrategy.h',
 './include/Variable/private/VCTransaction.h',
 './include/Variable/SingleThresholdCacheStrategy.h',
 './include/Visualization/VisualizationController.h'
 ]


core_moc_sources = ['src/Network/Downloader.cpp']


core_moc_files = qt5.preprocess(moc_headers : core_moc_headers, moc_sources: core_moc_sources)

core_sources = [
 './src/Common/MimeTypesDef.cpp',
 './src/Common/SignalWaiter.cpp',
 './src/Common/DateUtils.cpp',
 './src/Common/StringUtils.cpp',
 './src/Data/VectorSeries.cpp',
 './src/Data/ScalarSeries.cpp',
 './src/Data/OptionalAxis.cpp',
 './src/Data/DataSeriesIterator.cpp',
 './src/Data/SpectrogramSeries.cpp',
 './src/Data/ArrayDataIterator.cpp',
 './src/Data/DataSeriesUtils.cpp',
 './src/Network/Downloader.cpp',
 './src/Network/NetworkController.cpp',
 './src/Settings/SqpSettingsDefs.cpp',
 './src/DataSource/DataSourceItemAction.cpp',
 './src/DataSource/DataSourceItemMergeHelper.cpp',
 './src/DataSource/DataSourceItem.cpp',
 './src/DataSource/DataSourceController.cpp',
 './src/Time/TimeController.cpp',
 './src/PluginManager/PluginManager.cpp',
 './src/pybind11_wrappers/CatalogWrappers.cpp',
 #'./src/pybind11_wrappers/QtWrappers.cpp',
 #'./src/pybind11_wrappers/CoreWrappers.cpp',
 './src/Version.cpp',
 './src/Catalogue/CatalogueController.cpp',
 './src/Variable/Variable.cpp',
 './src/Variable/VariableSynchronizationGroup2.cpp',
 './src/Variable/VariableController2.cpp',
 './src/Variable/VariableModel2.cpp',
 './src/Visualization/VisualizationController.cpp']


core_inc = include_directories(['include', 'include/Plugin'])

sciqlop_core_lib = library('sciqlopcore',
                       core_sources,
                       core_moc_files,
                       cpp_args : '-DCORE_LIB',
                       include_directories : core_inc,
                       dependencies : [qt5core, qt5network, catalogicpp_dep, pybind11_dep],
                       install : true
                       )


sciqlop_core = declare_dependency(link_with : sciqlop_core_lib,
                                include_directories : core_inc,
                                dependencies : [qt5core, qt5network, catalogicpp_dep, pybind11_dep])



subdir('tests')

