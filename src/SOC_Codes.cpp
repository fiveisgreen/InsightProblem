#ifndef SOC_CODES_CPP
#define SOC_CODES_CPP

#include <iostream>
#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

#include "String_Utilities.cpp"
#include "Map_Utilities.cpp"

#define N_SOC_CATEGORIES 64
#define TOP_N_OCCUPATIONS 10

using namespace std;

/*
In establishing the count of SOC professions, we have the problem that they are not always typed into the database precisely, and typos have been noticed in the excel spreadsheet versions. Therefore we will instead count SOC Codes, and keep track of the relationship between SOC codes and SOC names. 

A partial list of all SOC codes is here: https://www.bls.gov/oes/current/oes_stru.htm, thouhg I do not rely on it except to inform the general structure of SOC codes. 
The codes come in a several categories, generally between 00 and 53, and then sub-categories innumerated by a 4 digit number. There are further sub-sub-categories which I will not track since the SOC name seems to not depend on these. I've allocated space for 64 categories in total.

Booking comes in two stages: 
	First use the category as an array index to jump to a subcategory. 
	then linearly step through the integer codes in the subcategory to locate the matching array index. Then increment a second array of the same length at that index.
	This makes the slowest step of booking iterating and comparing an unsorted array of about 20 ints.
	This is preferable to a binary approach or a sorted list, since it maximizes hardware performance by keeps the SOC codes seperate from the counts and minimizes jumping around. 

Presume the presence of a map such as map<int, string> SOC_Code_to_SOC_name
*/

inline int concat_SOC_category (int SOC_category, int SOC_subcategory) { return 10000*SOC_category + SOC_subcategory; } 
//SOC Codes are formatted like 53-7199. 53 is the SOC Category, and 7199 is the SOC_subcategory, 
//The concatination returns 537199, which will be used for the map SOC_Code_to_SOC_name.

struct SOC_Code_Category
{
	vector<int> SOC_subcategories;
	vector<int> counts;  //These two vectors will have the same length, probably less than 20.

	bool book (int SOC_subcategory) {
		//speed critical.
		//books the SOC_subcategory: incrementing the corresponding counter in counts.
		//Returns true if and only if this subcategory has never been encountered before.
		//sub_categories will sit in the order in which they were encountered. SOC subcategories are short, so ticking through is fast.
		//At worst this may cost 20-30 comparisons, but it's right down an array, so should make good of the hardware prefetcher and be speedy.
		for (unsigned int i=0;i<SOC_subcategories.size();++i) {
			if (SOC_subcategories[i] == SOC_subcategory) {
				counts[i]++;
				return false;
			}
		}
		SOC_subcategories.push_back(SOC_subcategory);
		counts.push_back(1);
		return true;
	}//end book
};

struct SOC_Code_Book 
{
	SOC_Code_Book (map<int, string>* SOC_Code_to_SOC_name_) {
		total = 0;
		SOC_Code_to_SOC_name = SOC_Code_to_SOC_name_;
		SOC_Code_Categories = new SOC_Code_Category*[N_SOC_CATEGORIES];  //make an array of N_SOC_CATEGORIES pointers to 
		for (int i=0;i<N_SOC_CATEGORIES;++i) 
			SOC_Code_Categories[i] = new SOC_Code_Category();  //make an array of pointers to SOC_Code_Category
	}
	~SOC_Code_Book () { 
		delete[] SOC_Code_Categories; 
		delete SOC_Code_to_SOC_name;
	}

	void book_SOC_code (int SOC_category, int SOC_subcategory, string SOC_Name) {
		//speed critical.  Assumes steralized inputs. 
		total++;
		bool never_before_seen = SOC_Code_Categories[SOC_category]->book(SOC_subcategory);
		if (never_before_seen) 
			(*SOC_Code_to_SOC_name)[concat_SOC_category(SOC_category,SOC_subcategory)] = SOC_Name;
	}//end book_SOC_code

	int total;  //total number booked
	map<int, string>* SOC_Code_to_SOC_name;  //a pointer to the map relating SOC codes to the corresponding string saying what they mean.
	SOC_Code_Category** SOC_Code_Categories;  //An array of 64 pointers to SOC_Code_Category, more than half of which will always be empty. #63 is for erros.
};


struct SOC_code_record{ int SOC_Code; int count; float percentage; string SOC_Name;};

bool SOC_code_record_Compare (SOC_code_record* firstElem, SOC_code_record* secondElem) { 
	if (firstElem->count != secondElem->count)
		return firstElem->count > secondElem->count; //order by number of certified H1B applications
	else
		return firstElem->SOC_Name < secondElem->SOC_Name; //Alphabetical order or occupation name. 
		//Strings must be capitalized for correct alphabetical ordering, this string built-in < things Alabama < Texas < alabama
}

std::vector<SOC_code_record*> 
make_sorted_SOC_code_record_list (SOC_Code_Book &SOC_code_book, map<int, string> & SOC_Code_to_SOC_name)
{	//This unfolds the SOC_Code_Book, returning the same contents as a vector of SOC_code_record sorted by their counts.
	
	float total = (float) SOC_code_book.total;
	//cout<<"Total for occupations is"<<total<<endl;//TEST
	vector<SOC_code_record*> SOC_Code_list;

	for (unsigned int icat=0;icat<N_SOC_CATEGORIES;++icat) { //Loop over SOC categories
		SOC_Code_Category* ith_SOC_Code_Category = SOC_code_book.SOC_Code_Categories[icat];
		for (unsigned int j_subcat=0;j_subcat<ith_SOC_Code_Category->SOC_subcategories.size();++j_subcat) { //Loop over SOC subcategories.
			//Here, for every SOC code, we make a SOC_code_record and add that to a list of SOC_code_records
			//This reacord includes the SOC Code, the number of certified applications for that profession, and the percentage of all professions. 
			SOC_code_record *record = new SOC_code_record();

			record->SOC_Code = concat_SOC_category(icat,ith_SOC_Code_Category->SOC_subcategories[j_subcat]);
			record->count = ith_SOC_Code_Category->counts[j_subcat];
			record->percentage = (100.0*ith_SOC_Code_Category->counts[j_subcat])/total; 
			
			if (is_in_map(record->SOC_Code, SOC_Code_to_SOC_name)) { //check that the SOC key is in the map. 
				record->SOC_Name =  Capitalize(SOC_Code_to_SOC_name[record->SOC_Code]);
				//Fetch the SOC name corresponding to the SOC code, and capitalize it.
				//Capitalization needs to happen here or else the sort function won't work as required. 
			} else {
				record->SOC_Name =  "OCCUPATION NAME NOT FOUND";
				cerr<<"Warning: Cannot find an occupation name for SOC code, "<<record->SOC_Code<<" in the SOC_Code_to_SOC_name map."<<endl;
			}
			//cout<<"    percentages: icat = "<<icat<<" j_subcat="<<j_subcat<<" counts: "<<ith_SOC_Code_Category->counts[j_subcat] << " percentage "<<record->percentage<<endl; //TEST

			SOC_Code_list.push_back(record);	
		}//for every subcategory
	}//end for every category

	int n = min(TOP_N_OCCUPATIONS,SOC_Code_list.size());//this min is needed for very short lists.
	nth_element(SOC_Code_list.begin(), SOC_Code_list.begin() + n, SOC_Code_list.end(), SOC_code_record_Compare);
	//Gather the leading 10 or 11 occpuations to the beginning of the list.
        //depending on implimentation, this may sort the first 10 (C++2011) or 11 (C++2014) elements.
	sort(SOC_Code_list.begin(), SOC_Code_list.begin() + (n-1), SOC_code_record_Compare); //Sort the first 10 elements. 

	return SOC_Code_list;
}//end Make_Sorted_SOC_Code_list

void Write_Top_Occupations (vector<SOC_code_record*> records)
{ 	//Writes the top_10_occupations.txt file
	//Example Output:
	//TOP_OCCUPATIONS;NUMBER_CERTIFIED_APPLICATIONS;PERCENTAGE
	//SOFTWARE DEVELOPERS, APPLICATIONS;6;60.0%
	FILE* output_file;
	string output_file_path = "output/top_"+to_string(TOP_N_OCCUPATIONS)+"_occupations.txt";
	output_file = fopen(output_file_path.c_str(),"w");

	//Print the header line
	fprintf(output_file, "TOP_OCCUPATIONS;NUMBER_CERTIFIED_APPLICATIONS;PERCENTAGE\n");

	//Print the top N entries
	for (int i = 0;i<min(TOP_N_OCCUPATIONS,records.size());++i) 
		if(records[i]->count > 0)
			fprintf(output_file, "%s;%i;%.1f\n",records[i]->SOC_Name.c_str(), records[i]->count, records[i]->percentage);  //Write to top occupations to file.

	fclose(output_file);
}//end Write_Top_10_Occupations

void Write_Top_Occupations (SOC_Code_Book &SOC_code_book, map<int, string> & SOC_Code_to_SOC_name) {
	Write_Top_Occupations(make_sorted_SOC_code_record_list(SOC_code_book, SOC_Code_to_SOC_name));
}//end Write_Top_10_Occupations

#endif //SOC_CODES_CPP
