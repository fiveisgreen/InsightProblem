****** Run Instructions *******
Available ways to run this: 
./run.sh               #attempts to run the alphabetically first file it finds in the input/ directory
./run.sh H1B.csv       #If H1B.csv is not located in the current directory, but is in input/, this will process input/H1B.csv
./run.sh input/H1B.csv #Processes input/H1B.csv, presuming that it exists. 

run.sh compiles and runs process_file.cpp. It tries to accommodate whatever reasonable inputs the user gives to it, as stated above. It also enforces the presence of the input and output directories. 



******* The Problem ******* 
	We are tasked with analyzing semicolon separated lists of immigration records. We want to create top-10 lists of both states and occupations for H1-B visas.
	Both lists are to be sorted first by the number of certified applicants, and then alphabetically in case of ties. These databases tend to be large, so ties are unlikely except under test conditions with small files. It is necessary to check that each entire in the database has STATUS = CERTIFIED and VISA_CLASS = H1-B. 


****** Technical details of the problem *******
	Input files are semicolon delimited text files stored in the input/ directory. The program produces two files in the output/ directory: top_10_states.txt and 		
top_10_occupations.txt. 

	The top_10_occupations.txt file starts with a header, followed by a list of occupation names (in all caps), corresponding number of certified applications, and the percentage that that number represents among all certified H1-B applications. These three terms are separated by semicolons. Example: 
	TOP_OCCUPATIONS;NUMBER_CERTIFIED_APPLICATIONS;PERCENTAGE
	SOFTWARE DEVELOPERS, APPLICATIONS;17;29.8
	COMPUTER OCCUPATIONS, ALL OTHER;7;12.3
	...

	The top_10_states.txt file has a similar format, but with states instead of occupations. Example: 
	TOP_STATES;NUMBER_CERTIFIED_APPLICATIONS;PERCENTAGE
	TX;10;17.5
	CA;9;15.8
	MA;4;7.0
	

****** Challenges *******
	The program spends most of it's time parsing lines. So the code is arranged around making this step as fast as possible. 

	The input files are semicolon delimited, but sometimes fields in the database contain semicolons. In development, this confused the line parser. It was seen that fields containing semicolons always wrap the text in quotation marks. So it's necessary to carefully parse the fields, tracking any quotation marks present, then correct for instances where the standard library's parser incorrectly parses the line. This adds significant complexity to the analysis of each line, and likely also to the run time. This leads to the difference between the Parse_line_safely and Parse_line in String_Utilities.cpp. 

	The column headers are not in the same order from year to year, and they change semantics significantly. The 2008 database in particular is seen to use significantly different semantics than the 2011 and 2016 databases. This is handled in the Analyze_Header function in Analyzer.cpp.

	The databases list occupations in several ways, most reliably as SOC occupation names. But even these are seen to have typos and missing letters. For each occupation category there is a corresponding SOC Code, which is a more reliable means of uniquely identifying an occupation. Therefore, we count unique SOC codes and map those to the corresponding SOC occupation names we find in the database. 

	There's an available list of SOC codes online, but it was found that there are many SOC codes in the data base not found on that site. Therefore, it was necessary to have a dynamic and continuously updated list of SOC codes and corresponding occupation names. Sometimes the SOC codes are listed as the category only. 


****** Approach ******
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


***** Files in src/ *****
process_file.cpp	Where execution begins. This contains main and process_file function. 
			process_file() initializes the analysis and reads the file line by line, feeding each line into the analyzer. 

Analyzer.cpp		This defines an Analyzer class. It internally stores counts of states and occupations, and provides three main functions: 
				Analyze_Header: looks through the first line and locates columns of interest
				Analyzer_line: analyzes a line of the database (other than the first line), looks through it, 
					       and updates counts of states and occupations according to what it finds.
				Write_Results: Writes top-10 lists to file. 

SOC_Codes.cpp		This handles code specific to storing and registering occupations. It has a two-tiered class structure for storing SOC codes, 
			arranging them first by category (first number), and then by sub-category (second number). 
			It also keeps track of the map between SOC codes and SOC occupation names. 
			It also contains the code for writing the top-10 occupations file.

States.cpp		This handles the code specific to storing and registering states where an applicant works. 
			It centers around a map between two-letter state abbreviations and integer counts of the corresponding number of certified applications. 
			It also contains the code for writing the top-10 states file.

Map_Utilities.cpp	Some functions for manipulating maps, and checking if keys are defined in a map.

String_Utilities.cpp	Some functions for manipulating strings. In particular, it includes the semicolon parsers. 

IO_Utilities.cpp	Currently this just contains a file_Exists function.

test.cpp		A suite of tests for the minor functions. Major functions are tested on the actual databases. 
