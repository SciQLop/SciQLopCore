import sys
import os

import sciqlopqt
import pysciqlopcore

import numpy as np
import datetime
import time
import unittest
import ddt

def listify(obj):
    if hasattr(obj, "__getitem__"):
        return obj
    return [obj]

@ddt.ddt
class TimeSeriesCtors(unittest.TestCase):
    @ddt.data(
             (pysciqlopcore.ScalarTimeSerie,10),
             (pysciqlopcore.VectorTimeSerie,10),
             (pysciqlopcore.SpectrogramTimeSerie,[10,10]),
    )
    def test_construct(self, case):
        ts = case[0](case[1])
        self.assertEqual(ts.shape,listify(case[1]))

class TimeSeriesData(unittest.TestCase):
    def test_set_ScalarTimeSerie_values(self):
        ts = pysciqlopcore.ScalarTimeSerie(10)
        ts.t[0]=111.
        self.assertEqual(ts.t[0],111.)
        ts[0]=123.
        self.assertEqual(ts[0],123.)

    def test_build_ScalarTimeSerie_from_np_arrays(self):
        ts = pysciqlopcore.ScalarTimeSerie(np.arange(10),np.zeros(10))

    def test_build_VectorTimeSerie_from_np_arrays(self):
        ts = pysciqlopcore.VectorTimeSerie(np.arange(10),np.zeros((3,10)))

if __name__ == '__main__':
    unittest.main()
