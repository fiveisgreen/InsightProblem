#!/bin/bash
mkdir -p input/
mkdir -p output/
mkdir -p insight_testsuite/tests/

g++ -std=c++11 -Wall src/process_file.cpp -o process_file

if [ $# -eq 0 ]; then #If no arguments, grab the first file we find from input/ and process that
    firstFile=$(ls input/ | head -1) 

    if [ ! -f input/$firstFile ]; then  #which points to a file that doesn't exist 
        echo "Error: Unable to find a file to run in the input/ directory"

    else #we find a file, run it.
        ./process_file input/$firstFile
    fi

elif [ $# != 1 ]; then #If more than one argument is provided, complain that we don't know what to do. 
    echo "Error: This requires at most one argument, which is a relative path to an input csv file."
    echo "Example syntax: $ ./run.sh input/H1B_2014.csv"

elif [ ! -f $1 ]; then #if one argument is provided, but no such file is in the listed directory

	               #attempt to interpret this as a file name in the input directory
    if [ ! -f input/$1 ]; then  #Complain if we still can't find a file to run
        echo "Error: The provided input file does not exist. Please remember to provide a local path, such as run.sh input/H1B_2014.csv. "
        echo "Example syntax: $ ./run.sh input/H1B_2014.csv"
    else
        ./process_file input/$1
    fi
else #The single input is a file which exists. Run it.
    ./process_file $1
fi

