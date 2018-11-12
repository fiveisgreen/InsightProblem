#!/bin/bash
mkdir -p input/
mkdir -p output/
mkdir -p insight_testsuite/tests/

if (( $# != 1 )); then
    echo "Error: This requires exactly one argument, which is a relative path to an input csv file."
    echo "Example syntax: $ ./run.sh input/H1B_2014.csv"
elif [ ! -f $1 ]; then
    if [ ! -f input/$1 ]; then
        echo "Error: The provided input file does not exist. Please remember to provide a local path, such as run.sh input/H1B_2014.csv. "
        echo "Example syntax: $ ./run.sh input/H1B_2014.csv"
    else
        g++ -std=c++11 -Wall src/process_file.cpp -o process_file
        ./process_file input/$1
    fi
else 
    g++ -std=c++11 -Wall src/process_file.cpp -o process_file
    ./process_file $1
fi

