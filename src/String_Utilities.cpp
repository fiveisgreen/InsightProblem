#ifndef String_Utilities_CPP
#define String_Utilities_CPP

#include <string>
#include <vector>
#include <sstream> //needed for istringstream

using namespace std;

char Capitalize_char (char chr)
{	//returns the capitalized character corresponding to chr
	static const char offset = 'a' - 'A';
	if (chr >= 'a' and chr <= 'z') 
	    return chr - offset;
	else 
	    return chr;
}

string Capitalize (string str)
{	//returns a capitalized copy of string str.
	string out = str;  //copy string.
	for(unsigned int i=0;i<out.size();++i) //capitalize each character in the string
		out[i] = Capitalize_char(out[i]);
	return out;
}

inline bool is_empty_string (const string str) {return str.compare("") == 0;}

inline bool strings_match_exactly (string str1, string str2) {return str1.compare(str2)==0;}

inline bool Contains (string query, string body) {
	//returns true if string query is found in string body;
	return body.find(query) != std::string::npos;
}

inline bool Contains_caps (string query, string body) {
	//First capitalizes body, then returns true if string query is found in string body;
	//use with query in all caps.
	return Contains(query, Capitalize(body));
}

vector<string> Parse_line (string line, char parse_char = ';')
{	 //This parses the line by ';' and stores each entry in string vector, then returns that vector.
        vector<string> entries;
        istringstream thisline_stringstream( line );  //read the next line of the file into the string 
        while (thisline_stringstream) {  //for every semicollon delimited entry in the line
                string this_entry;
                if (!getline( thisline_stringstream, this_entry, parse_char)) //parse the string, break when at the end of the string.
			break;  
                entries.push_back( this_entry );
        }
        return entries;
}
	//I can't use this parse because fields contain semicolons. I need something smarter: 
	//ex: "CALIFORNIA DEPARTMENT OF CORRECTIONS &AMP; REHABIL"
	//We can't even guarentee there will be only one stray ; in the field.
	//ex: "MECKLENBURG&AMP;NBSP;"
	
	//So instead, use Parse_line_safely

vector<string> Parse_line_safely (string line)
{	//This parses the line by ';' and stores each entry in string vector, then returns that vector.
	//This is very spicific to the Insight problem, so there's no point in making it really general
        vector<string> entries;
        istringstream thisline_stringstream( line );  //read the next line of the file into the string 
	
	bool semicolon_flag = false; //A flag to indicate that we need use the entry from the previous semicolon parse.
	string first_part_of_entry = "";//A place to store the previous entry when needed.

        while (thisline_stringstream) {  //for every semicollon delimited entry in the line
                string this_entry;
                if (!getline( thisline_stringstream, this_entry, ';')) //parse the string, break when at the end of the string.
			break;  

		//Here we deal with misparsing from semicolons in the text of fields
		if (semicolon_flag){ 
			//cout<<"Running misparser. Joining "<<first_part_of_entry<<" and "<< this_entry<<". Orig. line:"<<endl;
			//cout<<line<<endl;
			if (this_entry.rfind("\"") != string::npos) {
				this_entry = first_part_of_entry+";"+this_entry;
				semicolon_flag = false;
			} else { //we haven't encountered the close quote, so there are multiple ;'s 
				first_part_of_entry = first_part_of_entry+";"+this_entry;
				continue;
			}
			//it may work better by commenting the else out and getting rid of the conditional entirely
		} else if ( (this_entry.find("\"") != string::npos) and //There's a double quote
			    (this_entry.find("\"") == this_entry.rfind("\""))) { //and no closing quote
			//Then the field contains a semicolon, which has been misparsed. 
			//It is necessary to join this_entry with the next entry according to the conditional above.
			semicolon_flag = true;
			first_part_of_entry = this_entry;			
			continue;
		}
		
                entries.push_back( this_entry );
        }
        return entries;
} //end Parse_line_safely

bool Parse_line_safely_cuts(string line, const int &status_index, const int &visa_class_index, const string &status, const string &visa_class, vector<string> &entries){
	//Looks for the entry at status_index to match status, and for the entry at visa_class_index to match visa_class
	//If either of those don't match, return true and stop reading the line. So the rest of the program can skip the line.
	//else return false, and read the whole line into entries.
        istringstream thisline_stringstream( line );  //read the next line of the file into the string 
	
	int i=0;
	bool semicolon_flag = false; //A flag to indicate that we need use the entry from the previous semicolon parse.
	string first_part_of_entry = "";//A place to store the previous entry when needed.

        while (thisline_stringstream) {  //for every semicollon delimited entry in the line
                string this_entry;
                if (!getline( thisline_stringstream, this_entry, ';')) //parse the string, break when at the end of the string.
			break;  

		//Here we deal with misparsing from semicolons in the text of fields
		if (semicolon_flag){ 
			if (this_entry.rfind("\"") != string::npos) {
				this_entry = first_part_of_entry+";"+this_entry;
				semicolon_flag = false;
			} else { //we haven't encountered the close quote, so there are multiple ;'s 
				first_part_of_entry = first_part_of_entry+";"+this_entry;
				continue;
			}
		} else if ( (this_entry.find("\"") != string::npos) and //There's a double quote
			    (this_entry.find("\"") == this_entry.rfind("\""))) { //and no closing quote
			//Then the field contains a semicolon, which has been misparsed. 
			//It is necessary to join this_entry with the next entry according to the conditional above.
			semicolon_flag = true;
			first_part_of_entry = this_entry;			
			continue;
		}
		
		if (i==status_index     and not strings_match_exactly(this_entry, status)) 
		    return true;
		if (i==visa_class_index and not strings_match_exactly(this_entry, visa_class)) 
		    return true;

                entries.push_back( this_entry );
		i++;
        }
        return false;
}//Parse_line_safely_cuts


#endif //String_Utilities_CPP
