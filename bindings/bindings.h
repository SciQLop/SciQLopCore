#ifndef SCIQLOP_BINDINGS_H
#define SCIQLOP_BINDINGS_H
#define QT_ANNOTATE_ACCESS_SPECIFIER(a) __attribute__((annotate(#a)))
#include <memory>
#include "PyDataProvider.hpp"
#include "SciQLopCore/GUI/MainWindow.hpp"
#include "numpy_wrappers.hpp"

#include <SciQLopCore/Data/DataSeriesType.hpp>
#include <SciQLopCore/Data/IDataProvider.hpp>
#include <SciQLopCore/Data/ScalarTimeSerie.hpp>
#include <SciQLopCore/Data/VectorTimeSerie.hpp>
#include <SciQLopCore/DataSource/DataSources.hpp>
#include <SciQLopCore/Common/SciQLopObject.hpp>
#include <SciQLopCore/GUI/PorductsTree.hpp>
#include <SciQLopCore/SciQLopCore.hpp>

#endif // SCIQLOP_BINDINGS_H
