#ifndef SCIQLOP_BINDINGS_H
#define SCIQLOP_BINDINGS_H
#define QT_ANNOTATE_ACCESS_SPECIFIER(a) __attribute__((annotate(#a)))
#include <memory>
#include "PyDataProvider.hpp"
#include "numpy_wrappers.hpp"

#include <SciQLopCore/Data/DataSeriesType.hpp>
#include <SciQLopCore/Data/ScalarTimeSerie.hpp>
#include <SciQLopCore/Data/VectorTimeSerie.hpp>
#include <SciQLopCore/DataSource/IDataProvider.hpp>
#include <SciQLopCore/DataSource/DataSources.hpp>
#include <SciQLopCore/Common/SciQLopObject.hpp>
#include <SciQLopCore/Common/Product.hpp>
#include <SciQLopCore/SciQLopCore.hpp>
#include <SciQLopPlots/Qt/SyncPanel.hpp>
#include <SciQLopCore/GUI/MainWindow.hpp>
#include <SciQLopCore/GUI/CentralWidget.hpp>
#include <SciQLopCore/GUI/PorductsTree.hpp>
#include <SciQLopCore/GUI/TimeSyncPanel.hpp>
#include <SciQLopCore/GUI/EventTimeSpan.hpp>
#include <SciQLopCore/MimeTypes/MimeTypes.hpp>

#endif // SCIQLOP_BINDINGS_H
