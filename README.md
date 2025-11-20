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