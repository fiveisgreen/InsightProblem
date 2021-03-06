#include <iostream>
#include <fstream>

#include "Analyzer.cpp"
#include "IO_Utilities.cpp"

using namespace std;

void print_run_time(float time_in_seconds);

int process_file(string input_file_name){ 
	//returns 1 if successful. Returns 0 if the file doesn't exist, or is empty, or is a recipie for cookies. Return -1 if some other error.

	if (not file_Exists(input_file_name)) {
		cerr<<"Unable to find file "<<input_file_name<<". Exiting without processing data."<<endl;
		return 0;
	}

	Analyzer* analyzer = new Analyzer_Insight_Problem_v3();//19.6s
        clock_t post_header_clock;//TEST
	clock_t endwhile_clock;//TEST

        ifstream input_file( input_file_name.c_str() );

        if (input_file) {  //If the file is successfully opened, proceed with reading the file. 	
		//Try to read the first line.
                string firstline;
                if (!getline( input_file, firstline )) { //If we terminate upon trying to read the header, strong indication the file is empty.
                    cerr << "Error! Input file is empty. No output will be produced!"<<endl;
                    return -1;
                }

		bool successfully_analyzed_header = analyzer->Analyze_Header(firstline);  //interpret the first line to determine where columns of interest are //0.3ms

		if (not successfully_analyzed_header) {
                    cerr << "Error! Expected header format not found. No output will be produced!"<<endl;
                    return 0;
		}
		cout<<"start"<<endl;

		//Primary line loop
		/** Time the loop **/ post_header_clock = clock(); //TEST
                while (input_file) {
                        string thisline;
                        if (!getline( input_file, thisline ))  //both read file into thisline, and termination condition
				break;  

			analyzer->Analyzer_line(thisline);  //analyze line and book contents into memory internal to the Analyzer
                }//end while read file 
        	/** End timer **/ endwhile_clock= clock();//TEST //24.8s
        } else {
		 cerr << "Error: Unable to open input file "<<input_file_name<<endl;
		 return 0;
	}

        if (!input_file.eof()) { //If we quit the read loop for any reason other than reaching the end of file. 
		cerr << "Error: while reading input file! Unexpectedly quit file read loop";
		return -1;
	}

	analyzer->Write_Results();  //Generate output files containing the top 10 lists. 0.8ms

	input_file.close();

	//TEST:
        cout << "Done. Main Loop time: "; 
	print_run_time(((float) (endwhile_clock - post_header_clock))/CLOCKS_PER_SEC);

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

void print_run_time(float time_in_seconds) {
	if (time_in_seconds < 0.1)         cout<<time_in_seconds*1000. <<" ms";
        else if (time_in_seconds < 120.)   cout<<time_in_seconds       <<" seconds";
        else if (time_in_seconds < 3600.)  cout<<time_in_seconds/60.   <<" minutes";
        else if (time_in_seconds < 86400.) cout<<time_in_seconds/3600. <<" hours";
        else 	 			   cout<<time_in_seconds/86400.<<" days";
	cout<<endl;
}
