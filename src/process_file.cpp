#include <iostream>
#include <fstream>

#include "Analyzer.cpp"
#include "IO_Utilities.cpp"

using namespace std;

int process_file(string input_file_name)
{ 
	//returns 1 if successful. Returns 0 if the file doesn't exist, or is empty, or is a recipie for cookies. Return -1 if some other error.

	if (not file_Exists(input_file_name)) {
		cerr<<"Unable to find file "<<input_file_name<<". Exiting without processing data."<<endl;
		return 0;
	}

	Analyzer* analyzer = new Analyzer_Insight_Problem();

        ifstream input_file( input_file_name.c_str() );

        if (input_file) {  //If the file is successfully opened, proceed with reading the file. 	
		//Try to read the first line.
                string firstline;
                if (!getline( input_file, firstline )) { //If we terminate upon trying to read the header, strong indication the file is empty.
                    cerr << "Error! Input file is empty. No output will be produced!"<<endl;
                    return -1;
                }

		bool successfully_analyzed_header = analyzer->Analyze_Header(firstline);  //interpret the first line to determine where columns of interest are

		if (not successfully_analyzed_header) {
                    cerr << "Error! Expected header format not found. No output will be produced!"<<endl;
                    return 0;
		}
		cout<<"start"<<endl;
	
		//Primary line loop
                while (input_file) {
                        string thisline;
                        if (!getline( input_file, thisline ))  //both read file into thisline, and termination condition
				break;  

			analyzer->Analyzer_line(thisline);  //analyze line and book contents into memory internal to the Analyzer
                }//end while read file 
        } else {
		 cerr << "Error: Unable to open input file "<<input_file_name<<endl;
		 return 0;
	}

        if (!input_file.eof()) { //If we quit the read loop for any reason other than reaching the end of file. 
		cerr << "Error: while reading input file! Unexpectedly quit file read loop";
		return -1;
	}

	analyzer->Write_Results();  //Generate output files containing the top 10 lists.

	input_file.close();
	cout<<"Done"<<endl;
	return 1;  //successful termination.
}//end process_file

int main(int argc, char** argv) {
	if(argc!=2){
		if(argc<2)
			cout<<"Error: an input file argument is required. Example syntax: "<<endl;
		else
			cout<<"Error: only one file argument is currently possible. Example syntax: "<<endl;
		cout<<"./process_file input/H1B_file.csv"<<endl;
		return -1;
	}
	return process_file(argv[1]);
}
