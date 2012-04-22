#include <iostream>


#include <boost/spirit/include/qi.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

 template <typename Iterator>
    bool parse_numbers(Iterator first, Iterator last)
    {
        using qi::double_;
        using qi::phrase_parse;
        using ascii::space;
	using std::cout;
	using std::endl;

        bool r = phrase_parse(
            first,                          /*< start iterator >*/
            last,                           /*< end iterator >*/
            
	    (
	      '(' >> double_[[](double d) { cout<<"first was:"<<d<<endl; }]
		  >> -(',' >> double_[[](double d) { cout<<"second was:"<<d<<endl;}]) >> ')'
	    |	double_[[](double d) { cout<<"simple number was:"<<d<<endl; }]
	    ),
	    
            space                           /*< the skip-parser >*/
        );
        if (first != last) // fail if we did not get a full match
            return false;
        return r;
    }

int main(int argc, char **argv) {
    std::string str;
    while (getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        if (parse_numbers(str.begin(), str.end()))
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing succeeded\n";
            std::cout << str << " Parses OK: " << std::endl;
        }
        else
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "-------------------------\n";
        }
    }

    std::cout << "Bye... :-) \n\n";
    return 0;
}
