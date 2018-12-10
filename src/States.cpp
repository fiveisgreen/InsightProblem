#ifndef STATES_CPP
#define STATES_CPP

#include <iostream>
#include <stdio.h>
#include <map>
#include <string>
#include <algorithm>

#include "String_Utilities.cpp"
#include "Map_Utilities.cpp"

#define TOP_N_STATES 10

using namespace std;


/*
There shouldn't be any unexpected states of the US, so we don't need to check if the state is on the list or worry about extending the list.
That allows us to get away with just using a map. 
Therefore, there shall be a map relating the 2 capital letter state abreviation and the int count of certified applications for that state.
*/

void Build_state_map (map<string,int> & state_count)
{	//Initalizes a map between all 2 letter state codes and integer counters initalized to 0. 	

	static const int nstates = 60;  //50 states + Washington DC, Puerto Rico, American Samoa, Micronesia, Palau, Guam, Marshall, Northern Mariana, Virgin Islands + ??
				   //State "??" is for the case where no state is found. 
				   //This list is compiled from http://www.postmarks.org/photos/state.htm 

//a list of all state abreviations in alphabetical order
//static const string states_abbrev_list[nstates] = {"AK", "AL", "AR", "AS", "AZ", "CA", "CO", "CT", "DC", "DE", "FM", "FL", "GA", "GU", "HI", "IA", "ID", "IL", "IN", "KS", "KY", "LA", "MA", "MD", "ME", "MH", "MI", "MN", "MO", "MP", "MS", "MT", "NC", "ND", "NE", "NH", "NJ", "NM", "NV", "NY", "OH", "OK", "OR", "PA", "PR", "PW", "RI", "SC", "SD", "TN", "TX", "UT", "VA", "VI", "VT", "WA", "WI", "WV", "WY", "??"};

//a list of all state abreviations in usage order
static const string states_abbrev_list[nstates] = {"CA", "TX", "NY", "NJ", "IL", "GA", "MA", "PA", "WA", "FL", "MI", "VA", "NC", "OH", "MD", "CT", "MN", "AZ", "MO", "WI", "IN", "CO", "TN", "OR", "DC", "DE", "AR", "IA", "SC", "UT", "KS", "KY", "RI", "LA", "NE", "OK", "AL", "NH", "NV", "NM", "ID", "MS", "ME", "WV", "ND", "HI", "VT", "GU", "SD", "MT", "DC", "PR", "AK", "MP", "VI", "MH", "PW", "WY","AS","??"};

	for (int i=0;i<nstates;i++)
		state_count[states_abbrev_list[i]] = 0;
}//end build map

struct state_record{ string state_abbrev; int count; double percentage; };

bool state_record_Compare (state_record* firstElem, state_record* secondElem) { 
	if (firstElem->count != secondElem->count) 
		return firstElem->count > secondElem->count;
	else 	
		return firstElem->state_abbrev < secondElem->state_abbrev; //Alphabetical order
}

vector<state_record*> make_sorted_state_record_list (map<string,int> state_count) 
{	//This turns the state_count map into a partially sorted vector of state_records with the first 10 (TOP_N_STATES) elements in order. 
	//Each reacord has the state name, count of certified applications for that state, and the percentage of applications for that state.

        double total = (double) count_entries(state_count);
        vector<state_record*> state_record_list;
	state_record_list.reserve(60);//60 "states".

	for (map<string,int>::iterator x = state_count.begin(); x != state_count.end(); ++x) {
		//Here, for every state, we make a state_record and add that to a list state_record_list
		//This reacord includes the 2 letter state abreviation, the number of certified applications for that profession, and the percentage 
		state_record *record = new state_record();

		record->state_abbrev = x->first;
		record->count = x->second;
		record->percentage = (100.0*x->second)/total;

		state_record_list.push_back(record);
        }//end for state

	nth_element(state_record_list.begin(), state_record_list.begin() + TOP_N_STATES, state_record_list.end(), state_record_Compare);
	//Gather the top 10 or 11 elements to the beginning of the vector. 
	//depending on implimentation, this may sort the first 10 (C++2011) or 11 (C++2014) elements, when TOP_N_STATES = 10.
        sort(state_record_list.begin(), state_record_list.begin() + (TOP_N_STATES -1), state_record_Compare);  //Sort the first 10 elements.
        //sort(state_record_list.begin(), state_record_list.end(), state_record_Compare);  //Full sort.

        return state_record_list;
}//end make_sorted_state_record_list


void Write_Top_States (vector<state_record*> records)
{	//Writes the top_10_states.txt file
	//Example output:
	//TOP_STATES;NUMBER_CERTIFIED_APPLICATIONS;PERCENTAGE
	//FL;2;20.0%
	
	FILE* output_file;
	string output_file_path = "output/top_"+to_string(TOP_N_STATES)+"_states.txt";
	output_file = fopen(output_file_path.c_str(),"w");

	//Print the header line
	fprintf(output_file, "TOP_STATES;NUMBER_CERTIFIED_APPLICATIONS;PERCENTAGE\n");

	//Print the top N entries
	//for (int i = 0;i<60;++i) 
	for (int i = 0;i<min(TOP_N_STATES,records.size());++i) 
		if(records[i]->count > 0)
			fprintf(output_file, "%s;%i;%.1f\n",Capitalize(records[i]->state_abbrev).c_str(), records[i]->count, records[i]->percentage);  //Write top states to file

	fclose(output_file);
}//end WriteState

void Write_Top_States (map<string,int> state_count){   
	Write_Top_States(make_sorted_state_record_list(state_count));   
}

#endif //STATES_CPP
