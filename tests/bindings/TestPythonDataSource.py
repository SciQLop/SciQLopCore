#!/usr/bin/env python
import unittest
from SciQLopBindings import DataProvider, Product, SciQLopCore, ScalarTimeSerie, DataSeriesType
import numpy as np


class MyProvider(DataProvider):
    def __init__(self, path="/tests/scalar"):
        super(MyProvider,self).__init__()
        self.register_products([Product(path,[], DataSeriesType.SCALAR,{"type":"scalar"})])

    def get_data(self,metadata,start,stop):
        return ScalarTimeSerie(np.arange(start,stop)*1., np.cos(np.arange(start,stop)))

t=MyProvider()

class AProvider(unittest.TestCase):
    def test_can_be_retrieved(self):
        p = SciQLopCore.dataSources().provider("/tests/scalar")
        self.assertIsNotNone(p)
        self.assertEqual(p, t)

    def test_can_be_deleted(self):
        another = MyProvider("/another/scalar")
        self.assertEqual(SciQLopCore.dataSources().provider("/another/scalar"), another)
        del another
        self.assertIsNone(SciQLopCore.dataSources().provider("/another/scalar"))


if __name__ == '__main__':
    unittest.main()
