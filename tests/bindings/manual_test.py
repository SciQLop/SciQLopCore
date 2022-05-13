from SciQLopBindings import DataProvider, Product, ScalarTimeSerie
from SciQLopBindings import SciQLopCore, MainWindow, TimeSyncPannel, ProductsTree, DataSeriesType
from PySide6 import QtCore, QtWidgets
import numpy as np

app = QtWidgets.QApplication([])


class MyProvider(DataProvider):
    def __init__(self, path="/tests/scalar"):
        super(MyProvider, self).__init__()
        self.register_products([Product(path, [], DataSeriesType.SCALAR, {"type": "scalar"})])

    def get_data(self, metadata, start, stop):
        return ScalarTimeSerie(
            np.arange(start, stop) * 1.0, np.cos(np.arange(start, stop))
        )


t = MyProvider()
w = MainWindow()
w.addTimeSynPannel(TimeSyncPannel())
w.addTimeSynPannel(TimeSyncPannel())
w.addWidgetIntoDock(QtCore.Qt.LeftDockWidgetArea, ProductsTree())

w.show()
app.exec()
