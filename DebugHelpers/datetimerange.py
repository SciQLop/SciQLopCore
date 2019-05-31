#!/usr/bin/env python3

from dumper import *

import sys
import os
import numpy as np
import datetime
import time


def qdump__DateTimeRange(d , value):
    """

    :param d: DumperBase
    :param value: Value
    """
    tstart = datetime.datetime.utcfromtimestamp(value['m_TStart'].to('d'))
    tstop = datetime.datetime.utcfromtimestamp(value['m_TEnd'].to('d'))
    delta = str(tstop - tstart)
    tstart, tstop = str(tstart), str(tstop)

    d.putValue(tstart + ' : ' + tstop)
    d.putNumChild(3)
    if d.isExpanded():
        with Children(d, 3):
            with SubItem(d, "start"):
                d.putValue(tstart)
            with SubItem(d, "stop"):
                d.putValue(tstop)
            with SubItem(d, "delta"):
                d.putValue(delta)

