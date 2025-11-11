from casacore.tables import table, makearrcoldesc, maketabdesc, makedminfo
import argparse as ap
import os
import numpy as np
import operator
import functools
import subprocess as sp

def parse_args():
    parser = ap.ArgumentParser()
    parser.add_argument("original")
    parser.add_argument("compressed")
    parser.add_argument("real_complex")
    parser.add_argument("compressor")
    parser.add_argument("config_file")
    args = parser.parse_args()
    if args.real_complex not in ('real', 'complex'):
        raise Exception("specify 'real' or 'complex'")
    if args.compressor not in ("none", "mgard", "mgard_complex", "zfp", "sz", "bzip2"):
        raise Exception(f"the compressor name needs to be one of {args.compressor}")
    return args

def run_test():
    create_MS()
    sp.check_call(["compress_ms", ORIG, COMPRESSED, COLNAME, "--ADIOS2_config", CONFIG, "--step_size", STEP_SIZE])
    read_and_check()
    pass

def create_MS():
    """Write generated visibilities to a column using the TiledStMan.
    """
    size = functools.reduce(operator.mul, ORIG_SHAPE[1:], ORIG_SHAPE[0] * 8)
    nrows = ORIG_SHAPE[0]
    cell_shape = ORIG_SHAPE[1:]

    rng = np.random.default_rng()
    vis = rng.normal(size=ORIG_SHAPE)
    if COMPLEX:
        vis = np.complex64(vis)
        vis+= 1j*rng.normal(size=ORIG_SHAPE)

        vtype = 'complex'
    else:
        vis = vis.astype(float)
        vtype = 'float'

    print(f"Will write {size / 1024 / 1024:.2f} MB of data into {ORIG}\n\n")

    shape = vis.shape
    nrows = shape[0]
    cell_shape = shape[1:]
    dminfo={
        "TYPE": "TiledShapeStMan",
        "NAME": "tsm1", 
        "SPEC": {
            'MaxCacheSize': 0,
            'DEFAULTTILESHAPE': np.array([   4,    1, nrows/10], dtype=np.int32),
            'MAXIMUMCACHESIZE': 0,
            'HYPERCUBES': {
                '*1': {
                    'CubeShape': np.array(ORIG_SHAPE, dtype=np.int32),
                    'TileShape': np.array([   4,    1, nrows/10], dtype=np.int32),
                    'CellShape': np.array([  4, 251], dtype=np.int32),
                    'BucketSize': 63936, 'ID': {}}},
            'SEQNR': 1,
            'IndexSize': 1}}

    coldesc = makearrcoldesc(COLNAME,
                            '',valuetype=vtype, 
                            shape=cell_shape,
                            datamanagergroup=dminfo['NAME'], 
                            datamanagertype='TiledShapeStMan')

    tabdesc = maketabdesc(coldesc)
    dminfo = makedminfo(
        tabdesc, 
        {dminfo['NAME']: dminfo['SPEC']})
    tab = table(ORIG, tabledesc = tabdesc, dminfo = dminfo, readonly=False, ack=False)
    tab.addrows(nrows)
    tab.putcol(COLNAME, value = vis)
    print(f'wrote complex visibilities to {COLNAME} column')

def read_and_check():
    tab_orig = table(ORIG)
    tab_comp = table(COMPRESSED)
    data_orig = tab_orig.getcol(COLNAME)
    data_comp = tab_comp.getcol(COLNAME)
    lossless = COMPRESSOR in ("none", "bzip2")
    if lossless:
        threshold = 1e-10
    else:
        threshold = 0.1
    diff = abs(data_orig - data_comp)
    mdiff = diff.mean()
    check = (diff > threshold).sum()
    check_pt = check/data_orig.size
    print(f'check for operator: {COMPRESSOR} returned {check} difference(s) above threshold ({threshold}) or {check_pt*100:0.2f}%')
    if not lossless:
        print(f'Check percentage is {CHECK_THRESHOLD*100}%')
        if check_pt > CHECK_THRESHOLD:
            raise AssertionError("The Check was above the threshold, failing.")
    print(f'Mean difference is {mdiff}')
    if lossless and (mdiff > 0):
        raise AssertionError("Lossless compression should not have mean_diff > 0, failing.")
    elif (not lossless) and (mdiff == 0):
        print("Lossy compression has a mean_diff == 0, ensure there is sufficient data and config is valid.")

if __name__ == '__main__':
    ORIG_SHAPE = [10000, 120, 4]
    args = parse_args()
    ORIG = args.original
    COMPRESSED = args.compressed
    COMPLEX = args.real_complex == 'complex'
    COMPRESSOR = args.compressor
    COLNAME = f"DATA_{COMPRESSOR}"
    CONFIG = args.config_file
    CHECK_THRESHOLD = 0.05
    STEP_SIZE = "401"
    run_test()
