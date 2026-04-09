from tcompress_ms import create_MS, read_and_check
import argparse as ap
import subprocess as sp


def parse_args():
    parser = ap.ArgumentParser()
    parser.add_argument("config")
    return parser.parse_args()

if __name__ == '__main__':
    args = parse_args()
    ORIG_SHAPE = [10000, 120, 4]
    ORIG = "test_config.ms"
    COMPRESSED = "test_config_out.ms"
    COLNAME = "DATA"
    CONFIG = args.config
    
    create_MS(ORIG_SHAPE, ORIG, COLNAME, False)
    command_str = "mpirun -np 2 /mnt/compress_ms/src/compress_ms -c" + CONFIG
    cmd_list = shlex.split(command_str)
    sp.check_call(cmd_list,shell=False)
    read_and_check(ORIG,COMPRESSED,COLNAME, "none", 0.05)
