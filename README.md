# InsightProblem

##Introduction
This is my solution to the Insight Data Engineering H1B problem. A detailed discription of the problem can be found here: https://github.com/InsightDataScience/h1b_statistics/blob/master/README.md
In sumary, we are given the task of analyzing rather long comma-seperated lists of H1-B visa reacords. We are then tasked with constructing a formatted top-10 list of states for certified applications, and another top-10 list of occupations for certified applications. 

##Run Instructions
$ ./run.sh input/H1B_2014.csv

Alternatively, you can neglect the input/ part of the path and the run.sh script will add it for you as long as there is no file in the same directory with that name.
$ ./run.sh H1B_2014.csv

##Approach
