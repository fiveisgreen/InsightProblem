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


class Analyzer_Insight_Problem : public Analyzer { //runs 647853 entries in 24.8s 
    public:
        Analyzer_Insight_Problem ();
        ~Analyzer_Insight_Problem () {    delete SOC_code_book;   }
        bool Analyze_Header (string firstline);
        void Analyzer_line (string line);
        void Write_Results ();  
 
    protected:
        bool all_fields_were_found ();
 
        //These indicies are the column numbers of fields of interest, corresponding to the indicies that will come out of the line parser. 
        int CASE_NUMBER_INDEX;
        int STATUS_INDEX;
        int VISA_CLASS_INDEX;
        int SOC_CODE_INDEX;
        int SOC_NAME_INDEX;
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
    WORKLOC1_STATE_INDEX = not_set;
 
    Build_state_map(State_book);
    SOC_code_book = new SOC_Code_Book(&SOC_Code_to_SOC_name);
    //Init_map(SOC_Code_to_SOC_name, "SOC_Codes.txt"); //space to create a function to pre-fill this map with an official list.
}
 
void Analyzer_Insight_Problem::
Write_Results () 
{   //Write results of the Insight problem to appropriate files;
    Write_Top_States(State_book);
    Write_Top_Occupations(*SOC_code_book, SOC_Code_to_SOC_name);
}
 
 
bool Analyzer_Insight_Problem::
Analyze_Header(string firstline)
{   //This looks through the first line of the file. It sets the *_INDEX members of the class, which indicate which columns to look at to find desired information. 
 
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
        } 
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
                 (WORKLOC1_STATE_INDEX == not_set);
 
    //cout<<"The value of something_not_set is"<<something_not_set<<endl;
    //cout<<"The Status index = "<<STATUS_INDEX<<" and it's test is "<<(STATUS_INDEX == not_set)<<endl;
 
    if (something_not_set) {
        cerr << "Error! Expected header format not found. No output will be produced!"<<endl;   
        cout<<"STATUS: \t\t"<<(STATUS_INDEX >= 0?"found":"NOT FOUND")<<endl;
        cout<<"VISA_CLASS: \t\t"<<(VISA_CLASS_INDEX >= 0?"found":"NOT FOUND")<<endl;
        cout<<"SOC_CODE: \t\t"<<(SOC_CODE_INDEX >= 0?"found":"NOT FOUND")<<endl;
        cout<<"SOC_NAME: \t\t"<<(SOC_NAME_INDEX >= 0?"found":"NOT FOUND")<<endl;
        cout<<"WORKLOC1_STATE: \t"<<(WORKLOC1_STATE_INDEX >= 0?"found":"NOT FOUND")<<endl;
    }
    return not something_not_set;
}
 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//approach 1: make a vector of strings and just chop them, then index them.
 
inline int get_SOC_category (string SOC_Code) { return stoi(SOC_Code.substr(0,2)); }
inline int get_SOC_subcategory (string SOC_Code) { return stoi(SOC_Code.substr(3,4)); }
 
void Analyzer_Insight_Problem::
Analyzer_line (string line) {    
    //Approach 1. Fully parse every line, then index the resulting vector. 
    //Most general, also the slowest. 
    vector<string> entries = Parse_line_safely(line);//Parse_line(line);
 
    //Check if this is an entry we want to record.
    if (not strings_match_exactly(entries[STATUS_INDEX], "CERTIFIED")) 
        return;
    if (not strings_match_exactly(entries[VISA_CLASS_INDEX], "H-1B")) 
        return; //THere turn out to be a lot of E-3 Australian visas mixed in.
 
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
 
//approach 2: start making a vector of strings, and if you get to field VISA_CLASS and it's not CERTIFIED, stop.
class Analyzer_Insight_Problem_v2 : public Analyzer_Insight_Problem { //runs 647853 entries in 23.5 seconds
    public: 
    void Analyzer_line (string line);
};
 
void Analyzer_Insight_Problem_v2::
Analyzer_line (string line) {   
    //approach 2: start making a vector of strings, and if you get to field VISA_CLASS and it's not CERTIFIED, stop.
    //This is somewhat task spicific, 
    //runs 647853 entries in 23.5 seconds. It assuming that cuts will be made on status and visa class.
 
    vector<string> entries;
    if (Parse_line_safely_cuts(line, STATUS_INDEX, VISA_CLASS_INDEX, "CERTIFIED", "H-1B", entries) )
        return;
 
    //States
    string state = entries[WORKLOC1_STATE_INDEX];
    if (state.size() == 2){ //this if does not consume a measurable amount of time, but also never fails, can be removed.
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
 
}//end Analyzer_line v2

class Analyzer_Insight_Problem_v3 : public Analyzer_Insight_Problem { //runs 647853 entries in 19.6 seconds
    public: 
    void Analyzer_line (string line);
};
 
void Analyzer_Insight_Problem_v3::
Analyzer_line (string line) {   
    //approach 3: don't push back every entry, but instead only save those entries that are meaningful to you. 
    //do this by putting the conditionals inside the parse loop
    //This is very task spicific and not as flexible
    //with this, we run 647853 entries in 19.6 seconds

    istringstream thisline_stringstream( line );  //read the next line of the file into the string 

    //Start parsing the line, only saving entries of interest
    parse_error_healer parser;
    //bool semicolon_flag = false; //A flag to indicate that we need use the entry from the previous semicolon parse.
    //string first_part_of_entry = "";//A place to store the previous entry when needed.
		//can move these into set_entry_handling_parser_errors

    int i=0;
    string case_number = "";
    string state = "";
    string SOC_Code = "";
    string SOC_Name = "";
 
    while (thisline_stringstream) {  //for every semicollon delimited entry in the line
        string this_entry;
        if (!getline( thisline_stringstream, this_entry, ';')) //parse the string, break when at the end of the string.
        break;
 
	//fill this_entry, continuing and appending as necessary to correctly handle parser errors. 
	if ( parser.handling_parser_errors(this_entry) ) continue;
 
        //identify and store entries of interest
        if (i == CASE_NUMBER_INDEX) case_number = this_entry; else 
	if (i == STATUS_INDEX) {
        if (not strings_match_exactly(this_entry, "CERTIFIED"))
            return;
        }
        else if (i == VISA_CLASS_INDEX) {
        if (not strings_match_exactly(this_entry, "H-1B"))
            return;
        }
        else if (i == SOC_CODE_INDEX) SOC_Code = this_entry;
        else if (i == SOC_NAME_INDEX) SOC_Name = this_entry;
        else if (i == WORKLOC1_STATE_INDEX) state = this_entry;
 
        i++;
    }//end while //end line parsing and cuts
 
    //States
    if (state.size() == 2){ //this if does not consume a measurable amount of time, but also never fails, can be removed.
        State_book[state] += 1; //else, state is found, increment it's counter.
    } else {
        State_book["??"] += 1; //If we still don't find a listed state, increment the ?? state counter.
 
        cout<<"State not formatted as a state. Booking state as ??. CASE_NUMBER = "<<case_number<<endl;
        cout<<"    SOC_CODE: "<< SOC_Code<<endl;
        cout<<"    SOC_NAME: "<< SOC_Name<<endl;
        cout<<"    WORKLOC1_STATE_INDEX: "<< state<<endl;
    }
 
    //SOC codes and occupations.
    unsigned int SOC_Code_size = SOC_Code.size();
    if(SOC_Code_size >= 7) //SOC codes may be more than 7 characters, such as 12-1234.99. We truncate away the .99. 
        SOC_code_book->book_SOC_code(get_SOC_category(SOC_Code), get_SOC_subcategory(SOC_Code), SOC_Name);
    else if(SOC_Code_size == 1 or SOC_Code_size == 2)
        SOC_code_book->book_SOC_code(stoi(SOC_Code), 0, SOC_Name);
    else 
        SOC_code_book->book_SOC_code(0, 0, SOC_Name);
 
}//end Analyzer_line v3

//dolittle is a little Analyzer, just for testing the speed at which we read in lines and parse them with gitline.
//This tells us about the maximum speed that any Analyzer class class can possibly run that operates with istringstream.getline
//I find that it can run 647853 entries in 10.05 seconds, so half our run time just goes into reading. 
class dolittle : public Analyzer { //runs 647853 entries in 10.9 s
    public:
	bool Analyze_Header (string firstline){ j = 0; return true;}
	void Analyzer_line (string line){
	    //for every line, read the first 8 semi-colon delimited entries and add the first character of each to j.
	    istringstream thisline_stringstream( line );  //read the next line of the file into the string 
	    for(int i=0;i<8 and thisline_stringstream;i++){
		string this_entry;
		if (!getline( thisline_stringstream, this_entry, ';')) //parse the string, break when at the end of the string. 
		    break;
		j += this_entry[0];
	    }//end for
	}//end Analyzer_line
	void Write_Results () { cout <<"J: "<<j<<endl;}
	int j;
};

//doless is another theoretical class, who'se only point is to test the speed of ifstream getline. 
class doless : public Analyzer {  //runs 647853 entries in 8.71s. 
    public:
	bool Analyze_Header (string firstline){ j = 0; return true;}
	void Analyzer_line (string line){ j += line[0]; } //add the first character of every line to j
	void Write_Results () { cout <<"J: "<<j<<endl;}
	int j;
};

//tacks on both reading, parsing, and handeling semicolon parser errors.
class dolittle_scp: public Analyzer { //runs 647853 entries in 11.2s
    public:
	bool Analyze_Header (string firstline){ j = 0; return true;}
	void Analyzer_line (string line){
	    //for every line, read the first 8 semi-colon delimited entries and add the first character of each to j.
	    istringstream thisline_stringstream( line );  //read the next line of the file into the string 
	    parse_error_healer parser;
	    for(int i=0;i<8 and thisline_stringstream;i++){
		string this_entry;
		if (!getline( thisline_stringstream, this_entry, ';')) //parse the string, break when at the end of the string. 
		    break;
		if ( parser.handling_parser_errors(this_entry) ) continue; //consumes 0.3s
		j += this_entry[0];
	    }//end for
	}//end Analyzer_line
	void Write_Results () { cout <<"J: "<<j<<endl;}
	int j;
};

//We find that just reading the file in line by line is taking most of the time of the dolittle,
//reading in every line: 8.7s
//parsing semicolons with getline: 2.2s.  
//handeling parser errors: 0.3s
//everything else--mostly booking: 8.4s
 
#endif //ANALYZER_CPP
