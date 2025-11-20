#!/bin/bash

set -e

function usage() {
    # default usage
    echo "usage: $0 [ base | adios2 | casacore | compress_ms | all ]"
    exit 0
}

function build_base() {
    docker build -f docker/ubuntu-base.docker --tag alxndrwllmsn/ubuntu-base .
}

function build_adios() {
    docker build -f docker/adios2.docker --tag alxndrwllmsn/adios2 . 
}

function build_casacore() {
    docker build -f docker/casacore.docker --tag alxndrwllmsn/casacore .
}

function build_compress_ms() {
    docker build -f docker/compress_ms.docker --tag alxndrwllmsn/compress_ms .
}

if [[ $1 = "base" ]]
then
    build_base
elif [[ $1 = "adios2" ]]
then
    build_adios
elif [[ $1 = "casacore" ]]
then
    build_casacore
elif [[ $1 = "compress_ms" ]]
then
    build_compress_ms
elif [[ $1 = "all" ]]
then
    build_base
    build_adios
    build_casacore
    build_compress_ms
else
    usage
fi