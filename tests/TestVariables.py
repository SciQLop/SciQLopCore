import sys
import os
if not hasattr(sys, 'argv') or len(sys.argv)==0:
    sys.argv = ['']
current_script_path = os.path.dirname(os.path.realpath(__file__))
sys.path.append(current_script_path)

import sciqlopqt
import pysciqlopcore

import numpy as np
import pandas as pds
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
             (pysciqlopcore.SpectrogramTimeSerie,[10,10])
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
        ts = pysciqlopcore.ScalarTimeSerie(np.arange(10), np.arange(10)*10)
        for i in range(len(ts)):
            self.assertEqual(ts[i],i*10.)

    def test_build_VectorTimeSerie_from_np_arrays(self):
        v=np.ones((3,10))
        for i in range(3):
            v[:][i] = np.arange(10)*10**i
        ts = pysciqlopcore.VectorTimeSerie(np.arange(10), v)
        for i in range(len(ts)):
            self.assertEqual(ts[i].x,i)
            self.assertEqual(ts[i].y,i*10.)
            self.assertEqual(ts[i].z,i*100.)

    def test_build_VectorTimeSerie_from_np_arrays_row(self):
        v=np.ones((10,3))
        for i in range(3):
            v.transpose()[:][i] = np.arange(10)*10**i
        ts = pysciqlopcore.VectorTimeSerie(np.arange(10), v)
        for i in range(len(ts)):
            self.assertEqual(ts[i].x,i)
            self.assertEqual(ts[i].y,i*10.)
            self.assertEqual(ts[i].z,i*100.)

    def test_build_VectorTimeSerie_from_np_dataframe(self):
        df = pds.DataFrame(data=np.zeros((10,3)),index=np.arange(10))
        for i in range(3):
            df[i] = np.arange(10)*10**i
        ts = pysciqlopcore.VectorTimeSerie(df.index.values, df.values)
        for i in range(len(ts)):
            self.assertEqual(ts[i].x,i)
            self.assertEqual(ts[i].y,i*10.)
            self.assertEqual(ts[i].z,i*100.)

    def test_build_SpectrogramTimeSerie_from_np_arrays(self):
        v=np.ones((4,10))
        for i in range(4):
            v[:][i] = np.arange(10)*10**i
        ts = pysciqlopcore.SpectrogramTimeSerie(np.arange(10), v)
        for i in range(len(ts)):
            for j in range(4):
                print(f"ts[{i}][{j}] = " + str(ts[i][j]))


if __name__ == '__main__':
    unittest.main(exit=False)

