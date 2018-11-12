#ifndef ANALYZER_CPP
#define ANALYZER_CPP


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "States.cpp"
#include "SOC_Codes.cpp"
#include "String_Utilities.cpp"

#define not_set -1

using namespace std;

//void Parse_line (string line, int* desired_fields);

class Analyzer {
	public:
		Analyzer () {}
		~Analyzer () {}
		virtual bool Analyze_Header (string firstline) = 0;
		virtual void Analyzer_line (string line) {}
		virtual void Write_Results () {}
};

class Analyzer_Insight_Problem : public Analyzer {
	public:
		Analyzer_Insight_Problem ();
		~Analyzer_Insight_Problem () {    delete SOC_code_book;   }
		bool Analyze_Header (string firstline);
		void Analyzer_line (string line);
		void Write_Results ();	
		//vector<string> entries = Parse_line (line, desired_fields);

	protected:
		bool all_fields_were_found ();

		//These indicies are the column numbers of fields of interest, corresponding to the indicies that will come out of the line parser. 
		int CASE_NUMBER_INDEX;
		int STATUS_INDEX;
		int VISA_CLASS_INDEX;
		int SOC_CODE_INDEX;
		int SOC_NAME_INDEX;
		//int EMPLOYER_STATE_INDEX;
		int WORKLOC1_STATE_INDEX;
		//int WORKLOC2_STATE_INDEX;

		map<string,int> State_book;
		SOC_Code_Book* SOC_code_book;
		map<int, string> SOC_Code_to_SOC_name;
};

Analyzer_Insight_Problem::
Analyzer_Insight_Problem ()
{	
	CASE_NUMBER_INDEX    = not_set;
	STATUS_INDEX         = not_set;
	VISA_CLASS_INDEX     = not_set;
	SOC_CODE_INDEX       = not_set;
	SOC_NAME_INDEX       = not_set;
	//EMPLOYER_STATE_INDEX = not_set;
	WORKLOC1_STATE_INDEX = not_set;

	Build_state_map(State_book);
	SOC_code_book = new SOC_Code_Book(&SOC_Code_to_SOC_name);
	//Init_map(SOC_Code_to_SOC_name, "SOC_Codes.txt"); //space to create a function to pre-fill this map with an official list.
}

void Analyzer_Insight_Problem::
Write_Results () 
{	//Write results of the Insight problem to appropriate files;
	Write_Top_States(State_book);
	Write_Top_Occupations(*SOC_code_book, SOC_Code_to_SOC_name);
}


bool Analyzer_Insight_Problem::
Analyze_Header(string firstline)
{	//This looks through the first line of the file. It sets the *_INDEX members of the class, which indicate which columns to look at to find desired information. 

	//First fully parse the first line by ; into a vector of strings. 
	vector<string> parsed_line = Parse_line(firstline, ';');
	//vector<string> parsed_line = Parse_line(firstline, ';');
	int nfields = parsed_line.size();

	//cout<<"Header File Contents:"<<endl;//TEST
	//Loop over the terms, and note down recognized desired fields. 
	for (int n=0;n<nfields;++n) {
		string THIS_FIELD = Capitalize(parsed_line[n]);
		//cout<<THIS_FIELD<<" "<<n<<endl;//TEST

	//bool Contains(string query, string body)
	
		if (CASE_NUMBER_INDEX == not_set and  //if not already set
				(Contains("CASE_NUMBER", THIS_FIELD ) or Contains("CASE_NO", THIS_FIELD) )) {
			CASE_NUMBER_INDEX = n;
			//cout<<"CASE_NUMBER found! CASE_NUMBER_INDEX = "<<n<<endl;//TEST
			//2008 uses CASE_NO, 2011-2014 used LCA_CASE_NUMBER, 2015 on use CASE_NUMBER
		} else if (STATUS_INDEX == not_set and 
				Contains("STATUS", THIS_FIELD ) ) {
			STATUS_INDEX = n;
			//cout<<"STATUS found! STATUS_INDEX = "<<n<<endl;//TEST
			//2008 uses APPROVAL_STATUS, 2011-2014 uses "STATUS", 2015 use CASE_STATUS
		} else if (VISA_CLASS_INDEX == not_set and 
				Contains("VISA_CLASS", THIS_FIELD ) ) {
			VISA_CLASS_INDEX = n;
			//All years use "VISA_CLASS"
			//cout<<"VISA_CLASS found! VISA_CLASS_INDEX = "<<n<<endl;//TEST
		} else if (SOC_CODE_INDEX == not_set and 
				(Contains("SOC_CODE", THIS_FIELD ) or Contains("JOB_CODE", THIS_FIELD) )) {
			SOC_CODE_INDEX = n;
			//cout<<"SOC_CODE found! SOC_CODE_INDEX = "<<n<<endl;//TEST
			//2008 uses JOB_CODE, 2011-2014 used LCA_CASE_SOC_CODE, 2015 on use SOC_CODE
		} else if (SOC_NAME_INDEX == not_set and 
				(Contains("SOC_NAME", THIS_FIELD ) or Contains("OCCUPATIONAL_TITLE", THIS_FIELD) )) {
			SOC_NAME_INDEX = n;
			//cout<<"SOC_NAME found! SOC_NAME_INDEX = "<<n<<endl;//TEST
			//2008 uses OCCUPATIONAL_TITLE, 2011-2014 used LCA_CASE_JOB_TITLE, 2015 on use LCA_CASE_SOC_NAME
		} else if (WORKLOC1_STATE_INDEX == not_set and  
				(Contains("WORKSITE_STATE", THIS_FIELD ) or Contains("LCA_CASE_WORKLOC1_STATE", THIS_FIELD) or Contains("STATE_1", THIS_FIELD) )) {
			WORKLOC1_STATE_INDEX = n;
			//2008 uses STATE_1, 2011-2014 used LCA_CASE_WORKLOC1_STATE, 2015 on use WORKSITE_STATE
		} /*else if (EMPLOYER_STATE_INDEX == not_set and 
				(Contains("EMPLOYER_STATE", THIS_FIELD ) or strings_match_exactly("STATE", THIS_FIELD))) {
			EMPLOYER_STATE_INDEX = n;
			//2008 uses STATE, 2011-2014 used LCA_CASE_EMPLOYER_STATE, 2015 on use EMPLOYER_STATE
			//cout<<"EMPLOYER_STATE found! EMPLOYER_STATE_INDEX = "<<n<<endl;//TEST
		}*/
	}//end for each field in the header 

	return all_fields_were_found();
}//end Analyze_Header

bool Analyzer_Insight_Problem::
all_fields_were_found () {
	//This returns true iff Analyze_Header locates all the columns needed to run.
	//If something is not found a report is printed
	bool something_not_set = (STATUS_INDEX == not_set) or 
			     (VISA_CLASS_INDEX == not_set) or 
			     (SOC_CODE_INDEX == not_set) or 
			     (SOC_NAME_INDEX == not_set) or 
			     //(EMPLOYER_STATE_INDEX == not_set) or 
			     (WORKLOC1_STATE_INDEX == not_set);

	//cout<<"The value of something_not_set is"<<something_not_set<<endl;
	//cout<<"The Status index = "<<STATUS_INDEX<<" and it's test is "<<(STATUS_INDEX == not_set)<<endl;

	if (something_not_set) {
		cerr << "Error! Expected header format not found. No output will be produced!"<<endl;	
		cout<<"STATUS: \t\t"<<(STATUS_INDEX >= 0?"found":"NOT FOUND")<<endl;
		cout<<"VISA_CLASS: \t\t"<<(VISA_CLASS_INDEX >= 0?"found":"NOT FOUND")<<endl;
		cout<<"SOC_CODE: \t\t"<<(SOC_CODE_INDEX >= 0?"found":"NOT FOUND")<<endl;
		cout<<"SOC_NAME: \t\t"<<(SOC_NAME_INDEX >= 0?"found":"NOT FOUND")<<endl;
		//cout<<"EMPLOYER_STATE: \t"<<(EMPLOYER_STATE_INDEX >= 0?"found":"NOT FOUND")<<endl;
		cout<<"WORKLOC1_STATE: \t"<<(WORKLOC1_STATE_INDEX >= 0?"found":"NOT FOUND")<<endl;
	}
	return not something_not_set;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//approach 1: make a vector of strings and just chop them, then index them.

inline int get_SOC_category (string SOC_Code) { return stoi(SOC_Code.substr(0,2)); }
inline int get_SOC_subcategory (string SOC_Code) { return stoi(SOC_Code.substr(3,4)); }

void Analyzer_Insight_Problem::
Analyzer_line (string line)
{	//Approach 1. Fully parse every line, then index the resulting vector. 
	vector<string> entries = Parse_line_safely(line);//Parse_line(line);

	//Check if this is an entry we want to record.
	if (not strings_match_exactly(entries[STATUS_INDEX], "CERTIFIED")) 
		return;
	if (not strings_match_exactly(entries[VISA_CLASS_INDEX], "H-1B")) 
		return; //THere turn out to be a lot of E-3 Australian visas mixed in.

	cout<<entries[CASE_NUMBER_INDEX]<<endl;//To query the test.

	//States
	string state = entries[WORKLOC1_STATE_INDEX];
	if (state.size() == 2){
		State_book[state] += 1; //else, state is found, increment it's counter.
	} else {
		State_book["??"] += 1; //If we still don't find a listed state, increment the ?? state counter.

		cout<<"State not formatted as a state. Booking state as ??. CASE_NUMBER = "<<entries[CASE_NUMBER_INDEX]<<endl;
		cout<<"    STATUS: "<< entries[STATUS_INDEX]<<endl;
		cout<<"    VISA_CLASS: "<< entries[VISA_CLASS_INDEX]<<endl;
		cout<<"    SOC_CODE: "<< entries[SOC_CODE_INDEX]<<endl;
		cout<<"    SOC_NAME: "<< entries[SOC_NAME_INDEX]<<endl;
		cout<<"    WORKLOC1_STATE_INDEX: "<< entries[WORKLOC1_STATE_INDEX]<<endl;
	}

	//SOC codes and occupations.
	string SOC_Code = entries[SOC_CODE_INDEX];
	unsigned int SOC_Code_size = SOC_Code.size();
	if(SOC_Code_size >= 7) //SOC codes may be more than 7 characters, such as 12-1234.99. We truncate away the .99. 
	    SOC_code_book->book_SOC_code(get_SOC_category(SOC_Code), get_SOC_subcategory(SOC_Code), entries[SOC_NAME_INDEX]);
	else if(SOC_Code_size == 1 or SOC_Code_size == 2)
	    SOC_code_book->book_SOC_code(stoi(SOC_Code), 0, entries[SOC_NAME_INDEX]);
	else 
	    SOC_code_book->book_SOC_code(0, 0, entries[SOC_NAME_INDEX]);

}//end Analyzer_line

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
//approach 2: start making a vector of strings, and if you get to field VISA_CLASS and it's not CERTIFIED, stop.
bool Parse_line_and_select (string line, vector<string>& entries, int cut_field = 1, string select_condition = "CERTIFIED")
{
	//This parses the line by ';' and stores each entry in string vector, then returns that vector.
	//If you get to the CERTIFIED field, and 
	istringstream thisline_stringstream( thisline ); //read the next line of the file into the string
	int i=0;
	while (thisline_stringstream) {  //for every semicollon delimited entry in the line
		string this_entry;
		if (!getline( thisline_stringstream, this_entry, ';' )) 
			break; //parse the string, break when at the end of the string.
		entries.push_back( this_entry );

		if (i==cut_field and this_entry.compare(select_condition) != 0) 
			return false; //if not CERTIFIED, stop parsing.
		++i;
	}
	return true;
}//end Parser_line
*/

//approach 3: don't push back every entry, but instead only save those entries that are meaningful to you. 
	//this requires the conditionals to be inside the parse loop

//Parse line needs to be fast.


/*	 //Example code code of a file reader and line parser.
void populate_from_file(eventRegistry* BadEventRegistry, string filename, bool verbose) {
        if (verbose) 
		cout << "Reading CSC file" << std::endl;
        long int nEventsRead = 0;
        ifstream infile( filename );
        while (infile) {
                string thisline;
                if (!getline( infile, thisline )) 
			break;

                istringstream thisline_stringstream( thisline );
                vector <string> line;
                while (thisline_stringstream) {
                        string thenumber;
                        if (!getline( thisline_stringstream, thenumber, ':' )) 
				break;
                        line.push_back( thenumber );
                }
                int run= std::atoi(line[0].c_str());
                int lumi = std::atoi(line[1].c_str());
                int event= std::atoi(line[2].c_str());
                RegisterEvent(run, lumi, event, BadEventRegistry);
                nEventsRead++;
        }
        if (!infile.eof()) 
		cerr << "Error while reading bad event file!";
        if (verbose) 
		cout << "Done reading Bad Evetns file, read in  "<<nEventsRead << " events"<<std::endl;
}//end populate_from_file
*/

#endif //ANALYZER_CPP
