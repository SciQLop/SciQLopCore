#!/usr/bin/env python
import unittest
from SciQLopBindings import DataProvider, Product, SciQLopCore


class MyProvider(DataProvider):
    def __init__(self):
        super(MyProvider,self).__init__()
        self.register_products([Product("/tests/scalar",[],{"type":"scalar"})])

    def get_data(self,metadata,start,stop):
        ts_type = DataSeriesType.SCALAR
        return (((np.array([]), np.array([])), np.array([])), ts_type)

t=MyProvider()

class AProvider(unittest.TestCase):
    def test_can_be_retrieved(self):
        p = SciQLopCore.dataSources().provider("/tests/scalar")
        self.assertIsNotNone(p)
        self.assertEqual(p, t)

if __name__ == '__main__':
    unittest.main()
