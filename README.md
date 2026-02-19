# Compress MS

## Description
Copy a measurement set and replace a column's storage manager with the Adios2 Storage manager. At the same time, compress the desired column with the specified compressor.

## Dependencies (in compilation order)
- cmake
- The required compressor (eg. MGARD, zfp), see ADIOS2 for list of supported operators
- ADIOS2
- casacore (> 3.5.0)
- boost program_options
### for testing
- python (> 3.8)
- zfp
- MGARD

## Build (non-docker)
from within compress_ms
```
mkdir build
cd build
cmake ..
make
make install
```

## Build (docker)
```
./build_docker.sh all
```

## Usage
```
Usage: compress_ms <input_ms> <output_ms> <column_name> [options] 
```
### Command Line Options:
- `-h [ --help ]` -- produce help message
- `-c [ --config ] <config filename>` -- Config file name containing argument pairs like `input_ms=test.ms`
- `-o [ --compressor ] <compressor name>`--The compressor to use, see adios2 installation for valid operators (default: mgard)
- `-e [ --error_bound ] <error bound>` -- Error bound used by the compressor to determine the level of compression (default: 0.01)
- `-t [ --error_bound_type ] <error bound type>` -- The type of error bound (i.e. for MGARD, this is ABS or REL)
- `-s [ --step_size ] <step size>`-- The size of steps to split the data into (i.e. number of rows), remaining rows will be processed in the last step
- `-n [ --num_steps ] <number of steps>` -- The number of steps to split the data into
- `--ADIOS2_config <ADIOS2 config filename>` -- A yaml/xml config to be passed to the ADIOS2 storage manager. A useful alternative to manually setting the operator parameters.
### Extra Config file Options:
- `input_ms=<input filename>` -- The name of the input file
- `output_ms=<output filename>` -- The desired name of the output file
- `column_name=<column name>` -- The name of the column to apply the adios2 storage manager (and the selected compression operator) to.

### Command Line Example:

Running purely from the command line:
```Bash
compress_ms input_data.ms output_data.ms DATA -o mgard -e 0.01 -t ABS -s 10
```

The above command will open `input_data.ms` compress the `DATA` column and write the output to `output_data.ms`. 

It will use the **MGARD** operator (given by option `-o`), to compress the data with an **Absolute** error bound type (given by option `-t`) of **0.01** (given by option `-e`), and it will split the data into steps of **10** (`-s`, if you use `-n` you can specify the number of steps instead). The error bound provided will be in whatever units the data is stored in. Use `showtableinfo in=input_data.ms` (from `casacore`) to determine the units.

### Config Example:

Alternatively, you can set the above parameters in a config file, passed with `-c`:
```bash
compress_ms -c config.txt
```

An example of the config is:
```bash
#config.txt
input_ms=input_data.ms
output_ms=output_data.ms
column_name=DATA
compressor=mgard
error_bound=0.01
error_bound_type=ABS
step_size=10
```

This will run the compressor in an identical manner to the previous example.

### ADIOS2 Config Example (MGARD):

An advanced alternative option is to pass parameters directly to the ADIOS2 storage manager. This is useful if you want to use a custom compressor or access advanced compressor parameters:
```bash
compress_ms input_data.ms output_data.ms DATA --ADIOS2_config adios_config.yaml -s 10
```

Using this file:
```bash
#adios_config.yaml
- IO: "Adios2StMan"
  Engine:
    Type: "BP5"
  Variables:
    - Variable: "DATA"
      Operations:
        - Type: mgard
          tolerance: 0.01
          lossless: Huffman_Zstd
          mode: ABS
          s: inf
```

Ensuring that the `Variable` name is identical to the column name (`DATA` in this case).

The compression parameters here are the same as above (`tolerance=error_bound`, `mode=error_bound_type`), but exposes some more advanced compressor parameters such as the compressor to use for the final lossless step (`lossless`, specific to MGARD) and the error norm (`s`, i.e. how to calculate the error).

### Alternative ADIOS Config Example (ZFP):

Alternatively, if you want to use `zfp` to compress, you could change the `type` parameter:
```bash
#adios_config.yaml
- IO: "Adios2StMan"
  Engine:
    Type: "BP5"
  Variables:
  - Variable: "DATA"
      Operations:
        - Type: zfp
          accuracy: 0.01
```

The adios2 config can also be used with the compress_ms config file by adding:
```bash
#config.txt
ADIOS2_config=adios_config.yaml
```

## Usage (docker)
```
docker run --rm --mount type=bind,src=<local_workspace>,dst=</workspace> alxndrwllmsn/compress_ms compress_ms <input_ms> <output_ms> <column_name> [options]
```

## Usage (singularity)
Assuming the container is built and pushed to dockerhub
```
singularity pull docker://alxndrwllmsn/compress_ms compress_ms.sif
singularity exec --bind <local_workspace>:/workspace compress_ms.sif compress_ms <input_ms> <output_ms> <column_name> [options]
```
