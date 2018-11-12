#include <iostream>
#include <fstream>
#include <sstream>

#include "Analyzer.cpp"
#include "IO_Utilities.cpp"

using namespace std;

int 
main(string input_file_name)
{ 

	if ( not file_Exists(input_file_name))
	{
		cerr<<"Unable to find file "<<input_file_name<<". Exiting without processing data."<<endl;
		return 0;
	}

	Analyzer analyzer = new Analyzer_Insight_Problem();

        ifstream input_file( input_file_name );

        if(input_file)  //If the file is successfully opened.
	{ 	//Proceed with reading the file. 	

		//Try to read the first line.
                string firstline;
                if (!getline( input_file, firstline )){
                    cerr << "Error! Input file is empty. No output will be produced!";
                    return -1;
                }
		analyzer.Analyze_Header(firstline); //interpret the first line to determine where columns of interest are

                while (input_file) //Primary line loop
                {
                        string thisline;
                        if (!getline( input_file, thisline )) break; //termination condition

			analyzer.Analyzer_line(thisline); //parse the line, 

                }
        }//end while read file 

        if (!input_file.eof()) //If we quit the read loop for any reason other than reaching the end of file, complain, but don't exit.
		cerr << "Error while reading input file! Unexpectedly quit file read loop";

	analyzer.Write_Results(); //Generate output files containing the top 10 lists.

	input_file.close();
	return 1;

}//end main

