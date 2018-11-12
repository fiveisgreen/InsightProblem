#ifndef TEST_CPP
#define TEST_CPP

#include <string>
#include <iostream>
#include <vector>

#include "String_Utilities.cpp"
#include "IO_Utilities.cpp"
#include "Map_Utilities.cpp"

using namespace std;

//bool test_String_Utilities()
//bool test_IO_Utilities()
//bool test_Map_Utilities{}

inline bool my_assert(bool ought_to_be_true, string complaint_if_false){
	if (!ought_to_be_true) 
		cout<<complaint_if_false<<endl;
	return ought_to_be_true;
}


bool test_Map_Utilities(){
	//returns true iff everything in IO_Utilities.cpp does what it's supposed to do.
        bool ok = true;
        map<string,int> map1;
        map1["asdf"] = 2;
        map1["qwer"] = 2;
        map<int,string> map2;
        map2[2] = "asdf";
        map2[1] = "qwer";

        //test my_assert <string,int>
        ok &= my_assert( is_in_map("qwer", map1),
                "is_in_map test 1 fails");
        ok &= my_assert( not is_in_map("qwe", map1),
                "is_in_map test 2 fails");
        ok &= my_assert( not is_in_map("betternot", map1),
                "is_in_map test 3 fails");

        //test my_assert <int,string>
        ok &= my_assert( is_in_map(2, map2),
                "is_in_map test 4 fails");
        ok &= my_assert( not is_in_map(0, map2),
                "is_in_map test 5 fails");
        ok &= my_assert( not is_in_map(6, map2),
                "is_in_map test 6 fails");

        //test count_entries
        ok &= my_assert( count_entries(map1) == 4,
                "count_entries test 1 fails");

        return ok;
}

bool test_IO_Utilities(){
	//returns true iff everything in IO_Utilities.cpp does what it's supposed to do.
	bool ok = true;

	//test file_Exists
	ok &= my_assert( file_Exists("IO_Utilities.cpp"),
			"file_Exists test 1 fails");	
	ok &= my_assert( not file_Exists("clearly_nonexistant.cpp"),
			"file_Exists test 2 fails");	
	ok &= my_assert( not file_Exists("clearly_nonexistant/clearly_nonexistant.cpp"),
			"file_Exists test 3 fails");	
	return ok;
}

bool test_String_Utilities(){
	//returns true iff everything in String_Utilities.cpp does what it's supposed to do.
	bool ok = true;

	//test Capitalize_char
	ok &= my_assert( Capitalize_char('a') == 'A',
			"Capitalize_char test 1 fails");	
	ok &= my_assert( Capitalize_char('t') == 'T',
			"Capitalize_char test 2 fails");	
	ok &= my_assert( Capitalize_char('z') == 'Z',
			"Capitalize_char test 3 fails");	
	ok &= my_assert( Capitalize_char(',') == ',',
			"Capitalize_char test 4 fails");	

	//test is_empty_string
	ok &= my_assert( is_empty_string(""),
			"is_empty_string test 1 fails");	
	ok &= my_assert(not is_empty_string("asdf"),
			"is_empty_string test 2 fails");	

	//test strings_match_exactly
	ok &= my_assert( strings_match_exactly("asdf","asdf"),
			"strings_match_exactly test 1 fails");	
	ok &= my_assert( not strings_match_exactly("asdf","asdfd"),
			"strings_match_exactly test 2 fails");	
	ok &= my_assert( not strings_match_exactly("asdf","dasdf"),
			"strings_match_exactly test 3 fails");	
	ok &= my_assert( not strings_match_exactly("asdfd","asdf"),
			"strings_match_exactly test 4 fails");	
	ok &= my_assert( not strings_match_exactly("dasdf","asdf"),
			"strings_match_exactly test 5 fails");	
	
	//test Contains
	ok &= my_assert(Contains("asdf","asdf"),
			"Contains test 1 fails");	
	ok &= my_assert(Contains("asdf","asdfd"),
			"Contains test 2 fails");	
	ok &= my_assert(Contains("asdf","dasdf"),
			"Contains test 3 fails");	
	ok &= my_assert(not Contains("asdfd","asdf"),
			"Contains test 4 fails");	
	ok &= my_assert(not Contains("dasdf","asdf"),
			"Contains test 5 fails");	
	ok &= my_assert(not Contains("asdf","qwer"),
			"Contains test 6 fails");	

	//test Contains_caps, 
	ok &= my_assert( Contains_caps("ASDF","qwerASDF"),
			"Contains_caps test 1 fails");	
	ok &= my_assert( Contains_caps("ASDF","qwerasdf"),
			"Contains_caps test 2 fails");	
	ok &= my_assert(not Contains_caps("asdf","qwerASDF"),
			"Contains_caps test 3 fails");	
	ok &= my_assert( Contains_caps("ASDF","qwerASDF"),
			"Contains_caps test 4 fails");	

	//test Parse_line
	string Parse_line_test1 = "asdf;qwer";
	string Parse_line_test2 = ";asdf;qwer";
	string Parse_line_test3 = ";asdf;qwer;";
	string Parse_line_test4 = ":asdf:qwer:";
	ok &= my_assert( strings_match_exactly( "qwer", Parse_line(Parse_line_test1)[1] ),
			"Parse_line test 1 fails");	
	ok &= my_assert( strings_match_exactly( "qwer", Parse_line(Parse_line_test2)[2] ),
			"Parse_line test 2 fails");	
	ok &= my_assert( strings_match_exactly( "qwer", Parse_line(Parse_line_test3)[2] ),
			"Parse_line test 3 fails");	
	ok &= my_assert( strings_match_exactly( "qwer", Parse_line(Parse_line_test4,':')[2] ),
			"Parse_line test 4 fails");	

	
	return ok;
}//end test_suite

#endif //TEST_CPP
