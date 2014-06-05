#ifndef LOCATION_H
#define LOCATION_H

#include <boost/thread.hpp>
#include <string>

class Location
{
    const char*         func_name_;
    const char*         file_name_;
    int                 line_number_;
    const void*         program_counter_;
    boost::thread::id   tid_;
public:
    Location();
    Location(const char* func_name, const char* file_name, int line_number, const void* program_counter, boost::thread::id tid);
    Location(const Location& right);

    std::string getFilename();

    std::string toString();
};

const void* GetProgramCounter();

#define FROM_HERE Location(__FUNCTION__, __FILE__, __LINE__, GetProgramCounter(), boost::this_thread::get_id())

#endif
