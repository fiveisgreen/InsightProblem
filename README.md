# InsightProblem
This is my solution to the Insight Data Engineering H1B problem. A detailed description of the problem can be found here: https://github.com/InsightDataScience/h1b_statistics/blob/master/README.md

## Problem
In summary, we are given the task of analyzing rather long text file listing of H1-B visa records. We are then tasked with constructing a formatted top-10 list of work-site states for certified H1-B applications, and another top-10 list of SOC occupations for certified H1-B applications. Both lists are to be sorted first by the number of certified applicants, and then alphabetically in case of ties. These databases tend to be large, so ties are unlikely except under test conditions with small files. It is necessary to check that each entire in the database has STATUS = CERTIFIED and VISA_CLASS = H1-B.

Computational challenges include: 

* efficiently processing a large file,
* handling different arrangements of the columns and different labelings of the column headers, 
* handling semicolons within fields without fouling the line parsing, handling inconsistent and typo ridden spelling of occupations. 
* ambiguity in the count of applications in a state from people who work in 2 states,
* SOC_codes that sometimes only list the category
* SOC_codes that are not on officially documentation 

## Approach
We locate the file, then read it in line by line. First the header is read and relevant columns identified. Then each of the data lines are read, parsed, and analyzed. Internal counters in memory are incremented according to the contents of the lines. At the end, the internal lists of counts are sorted and output files are written.

Occupations are identified by SOC code, and a map between SOC codes and occupation names is maintained, called SOC_Code_to_SOC_name.

There is an Analyzer class tasked managing how the header is read, how the lines are parsed, how the files are written, and how the counters are stored in memory.
Counters for states are stored in a map between 2-letter state abbreviations and ints.
Counters for SOC codes have a custom container (SOC_Code_Book): First an array of containers (SOC_Code_Category), one container for each SOC category. This array is indexed by the SOC category its self. The SOC_Code_Category container stores vectors of observed SOC Codes and a vector of the corresponding counts.

Some detailed choices:
In some years, there can be 2 work states listed, which may be different states. This produces a fundamental ambiguity in the count of certified application per state. I've chosen to ignore the second work state and only count the first.
If no valid state is found, then a counter corresponding to state abbreviation prefix = "??" (which normally would be something like "TX", "CA", or "NY") is incremented. This would also produce an error, and has not occurred in testing.
If no valid SOC Code is found, we increment code 00-0000, which is an actual code representing the most general occupation. If the SOC code has a category, but no sub-category the subcategory is set to 0000. (Real life example: SOC Code = 2. Normally it's something like 12-3456.)
Frequently, SOC Codes are found with an extra 2 digits, such as 12-3456.99. It is also seen that these extra 2 digits do not influence the SOC Name, so they are truncated off.

## Run 
Available ways to run this:

./run.sh               attempts to run the alphabetically first file it finds in the input/ directory

./run.sh H1B.csv       If H1B.csv is not located in the current directory, but is in input/, this will process input/H1B.csv

./run.sh input/H1B.csv Processes input/H1B.csv, presuming that it exists.

run.sh compiles and runs process_file.cpp. It tries to accommodate whatever reasonable inputs the user gives to it, as stated above. It also enforces the presence of the input and output directories.

