#ifndef FILE_PARSER_H_
#define FILE_PARSER_H_

#include <algorithm>
#include <vector> 
#include <iostream>
#include <sstream>
#include <fstream>
#include <utility>
#include <cctype>
#include <string>

#if defined(__linux__)
#include <limits.h>
#endif

//#include <locale>

using namespace std;

// trim from start (in place)
static inline void ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
	}));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
	ltrim(s);
	rtrim(s);
}

// ----------------------------------------------------------------------------------------

std::string get_path(std::string filename, std::string sep)
{
    int prog_loc = (int)(filename.rfind(sep));
    return filename.substr(0, prog_loc);    // does not return the last separator
}

// ----------------------------------------------------------------------------------------
#if defined(__linux__)
std::string get_linux_path()
{
    std::string path = "/";
    char result[PATH_MAX+1];
    memset(result, 0, sizeof(result)); 
    
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    
    if (count != -1) 
    {
        path = get_path(dirname(result), "/");
    }

    return (path+"/");

}
#endif

// ----------------------------------------------------------------------------------------

std::string path_check(std::string path)
{
    if (path.empty())
        return path;

    std::string path_sep = path.substr(path.length() - 1, 1);
    if (path_sep != "\\" & path_sep != "/")
    {
        return path + "/";
    }

    return path;

}   // end of path_check

// ----------------------------------------------------------------------------------------

void parse_line(std::string input, const char delimiter, std::vector<std::string> &params)
{
    //params.clear();

    try
    {
        stringstream ss(input);
        while (ss.good())
        {
            std::string substr;
            std::getline(ss, substr, delimiter);
            trim(substr);
            if (substr.size() > 0)
            {
                params.push_back(substr);
            }

        }
    }
    catch (std::exception &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }
}   // end of parse_line

// ----------------------------------------------------------------------------------------

void parse_input_range(std::string input, std::vector<double> &range)
{
    range.clear();
    std::vector<std::string> params;
    std::vector<double> r;

    // parse out the input values. should be in the form min:step:max
    parse_line(input, ':', params);

    
    if (params.size() != 3)
    {
        std::cout << "Incorrect range parameters supplied.  Setting value to: 0" << std::endl;
        range.push_back(0);
        return;
    }
    else
    {
        r.push_back(std::stod(params[0]));
        r.push_back(std::stod(params[1]));
        r.push_back(std::stod(params[2]));
    }

    double s = r[0];
    if (r[1] > 0)
    {
        while (s <= r[2])
        {
            range.push_back(s);
            s += r[1];
        }
    }
    else if (r[1] < 0)
    {
        while (s >= r[2])
        {
            range.push_back(s);
            s += r[1];
        }
    }
    else
    {
        range.push_back(r[0]);
    }

}	// end of parse_input_range

// ----------------------------------------------------------------------------------------

void parseCSVLine(std::string line, std::vector<std::string> &line_params)
{
    stringstream ss(line);
    while (ss.good())
    {
        std::string substr;
        std::getline(ss, substr, ',');
        trim(substr);
        if (substr.size() > 0)
        {
            line_params.push_back(substr);
        }

    }     
}   // end of parseCSVLine

// ----------------------------------------------------------------------------------------

void parseCSVFile(std::string parse_filename, std::vector<std::vector<std::string>> &params)
{
	std::ifstream csv_file(parse_filename);
	std::string next_line;

	while (std::getline(csv_file, next_line))
	{
		if ((next_line[0] != '#') && (next_line.size() > 0))
		{
			std::vector<std::string> line_params;
            
            parseCSVLine(next_line, line_params);
            
			if (line_params.size() > 0)
			{
				params.push_back(line_params);
			}

		}
	}

}	// end of parseCSVFile


// ----------------------------------------------------------------------------------------


//void parse_group_line(std::string line, const char open, const char close, std::vector<std::string> &params, std::vector<std::string> &group_params)
void parse_group_line(std::string line, const char open, const char close, std::vector<std::string> &params)
{

    std::string sec_start = "";
    std::string sec_end = "";
    std::string group = "";

    // parse the lines - find the first instance of a group and then the last one
    // then separate the two sections
    // this assumes that there are no non-group section between groups
    uint32_t g_start = (uint32_t)line.find(open);
    uint32_t g_stop = (uint32_t)line.rfind(close);

    // get the substrings
    sec_start = line.substr(0, g_start);
    parse_line(sec_start, ',', params);

    if (g_stop < line.length())
    {
        group = line.substr(g_start, g_stop - g_start + 1);

        stringstream gs(group);
        while (gs.good())
        {
            std::string s1;
            std::string s2;
            std::getline(gs, s1, open);
            std::getline(gs, s2, close);

            trim(s2);
            if (s2.size() > 0)
            {
                params.push_back(s2);
            }
        }
        sec_end = line.substr(g_stop + 1, line.length() - 1);
        parse_line(sec_end, ',', params);    
    }
}

// ----------------------------------------------------------------------------------------

//void parse_group_csv_file(std::string parse_filename, const char open, const char close, std::vector<std::vector<std::string>> &params, std::vector < std::vector<std::string>> &group_params)
void parse_group_csv_file(std::string parse_filename, const char open, const char close, std::vector<std::vector<std::string>> &params)
{
    std::ifstream csv_file(parse_filename);
    std::string next_line;

    while (std::getline(csv_file, next_line))
    {
        if ((next_line[0] != '#') && (next_line.size() > 0))
        {
            std::vector<std::string> line_params, gp_params;

            parse_group_line(next_line, open, close, line_params);

            if (line_params.size() > 0)
            {
                params.push_back(line_params);
            }

        }
    }
}




#endif	// FILE_PARSER_H_
