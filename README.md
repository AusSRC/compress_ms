# Compress MS
## Description
Copy a measurement set and replace a column's storage manager with the Adios2 Storage manager. At the same time, compress the desired column with MGARD.
## Dependencies (in compilation order)
- MGARD
- ADIOS2
- casacore (> 3.5.0)
## Usage
```
compress_ms <input_ms> <output_ms> <column_name> <error_bound> <ABS/REL>
```
- `input_ms`: The Measurement Set (MS) to be copied
- `output_ms`: The desired name of the output MS
- `column_name`: The column to be converted/compressed (default: "DATA")
- `error_bound`: The desired accuracy with with which to compress the data with MGARD
- `ABS/REL`: Whether to interpret the previously defined as an Absolute error bound or as a relative error bound (relative to the range of the data. ie. as a proportion of $`(x_{max}-x_{min})`$).