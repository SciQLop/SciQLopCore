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
             (pysciqlopcore.SpectrogramTimeSerie,[10,10]),
             (pysciqlopcore.MultiComponentTimeSerie,[10,10]),
             (pysciqlopcore.ScalarTimeSerie,0),
             (pysciqlopcore.VectorTimeSerie,0),
             (pysciqlopcore.SpectrogramTimeSerie,[0,10]),
             (pysciqlopcore.MultiComponentTimeSerie,[0,10])
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

    def test_set_VectorTimeSerie_values(self):
        ts = pysciqlopcore.VectorTimeSerie(10)
        ts.t[0]=111.
        self.assertEqual(ts.t[0],111.)
        ts[0].x=111.
        ts[0].y=222.
        ts[0].z=333.
        self.assertEqual(ts[0].x,111.)
        self.assertEqual(ts[0].y,222.)
        self.assertEqual(ts[0].z,333.)

    def test_set_SpectrogramTimeSerie_values(self):
        ts = pysciqlopcore.SpectrogramTimeSerie((10,100))
        ts.t[0]=111.
        self.assertEqual(ts.t[0],111.)
        ts[0][11]=123.
        self.assertEqual(ts[0][11],123.)

    def test_build_ScalarTimeSerie_from_np_arrays(self):
        ts = pysciqlopcore.ScalarTimeSerie(np.arange(10), np.arange(10)*10)
        for i in range(len(ts)):
            self.assertEqual(ts[i],i*10.)

    def test_build_VectorTimeSerie_from_np_arrays(self):
        v=np.ones((10,3))
        for i in range(3):
            v.transpose()[:][i] = np.arange(10)*10**i
        ts = pysciqlopcore.VectorTimeSerie(np.arange(10), v)
        for i in range(len(ts)):
            self.assertEqual(ts[i].x,i)
            self.assertEqual(ts[i].y,i*10.)
            self.assertEqual(ts[i].z,i*100.)


    def test_build_MultiComponentTimeSerie_from_np_arrays(self):
        v=np.ones((10,5))
        for i in range(5):
            v.transpose()[:][i] = np.arange(10)*10**i
        ts = pysciqlopcore.MultiComponentTimeSerie(np.arange(10), v)
        for i in range(len(ts)):
            self.assertEqual(ts[i][0],i)
            self.assertEqual(ts[i][1],i*10.)
            self.assertEqual(ts[i][2],i*100.)

    def test_build_MultiComponentTimeSerie_from_np_arrays_of_nan(self):
        v=np.empty((2,5))
        v.fill(np.nan)
        ts = pysciqlopcore.MultiComponentTimeSerie(np.arange(2), v)
        for i in range(len(ts)):
            self.assertTrue(np.isnan(ts[i][0]))
            self.assertTrue(np.isnan(ts[i][1]))
            self.assertTrue(np.isnan(ts[i][2]))
            self.assertTrue(np.isnan(ts[i][3]))

    def test_build_VectorTimeSerie_from_np_arrays_row(self):
        v=np.ones((10,3))
        for i in range(3):
            v.transpose()[:][i] = np.arange(10)*10**i
        ts = pysciqlopcore.VectorTimeSerie(np.arange(10), v)
        for i in range(len(ts)):
            self.assertEqual(ts[i].x,i)
            self.assertEqual(ts[i].y,i*10.)
            self.assertEqual(ts[i].z,i*100.)

    def test_build_ScalarTimeSerie_from_np_dataframe(self):
        df = pds.DataFrame(data=np.zeros((10,1)),index=np.arange(10))
        df[0] = np.arange(10)
        ts = pysciqlopcore.ScalarTimeSerie(df.index.values, df.values)
        for i in range(len(ts)):
            self.assertEqual(ts[i],i)

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
        v=np.ones((10,30))
        for i in range(30):
            v.transpose()[:][i] = np.arange(10)*10**(i/10.)
        ts = pysciqlopcore.SpectrogramTimeSerie(np.arange(10),np.arange(30), v)
        for i in range(len(ts)):
            for j in range(30):
                self.assertEqual(ts[i][j], i*10**(j/10.))
        for i in range(30):
            self.assertEqual(ts.axis(1)[i], i)

class VariableData(unittest.TestCase):
    def test_default_state(self):
        v=pysciqlopcore.Variable2("hello")
        self.assertEqual(str(v.name), str("hello"))
        self.assertEqual(type(v.data), type(None))
        self.assertEqual(len(v), 0)

    def test_set_name(self):
        v=pysciqlopcore.Variable2("hello")
        self.assertEqual(str(v.name), str("hello"))
        v.name="newName"
        self.assertEqual(str(v.name), str("newName"))

if __name__ == '__main__':
    unittest.main(exit=False)

