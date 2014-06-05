#include "Location.h"
#include <boost/lexical_cast.hpp>

extern "C" {
    void* _ReturnAddress();
}

Location::Location()
: func_name_("Unknown"), file_name_("Unknown"), line_number_(-1), program_counter_(NULL)
{
}

Location::Location(const char* func_name, const char* file_name, int line_number, const void* program_counter, boost::thread::id tid)
: func_name_(func_name), file_name_(file_name), line_number_(line_number), program_counter_(program_counter), tid_(tid)
{
}

Location::Location(const Location& right)
{
    func_name_ = right.func_name_;
    file_name_ = right.file_name_;
    line_number_ = right.line_number_;
    program_counter_ = right.program_counter_;
    tid_ = right.tid_;
}

std::string Location::getFilename()
{
    std::string path(file_name_);
    int pos = path.find_last_of('/');
    if (pos == -1) pos = path.find_last_of('\\');
    if (pos == -1) return file_name_;
    else return path.substr(pos+1);
}

std::string Location::toString()
{
    return boost::lexical_cast<std::string>(tid_) + "@" + func_name_ + "@" + getFilename() + ":" + boost::lexical_cast<std::string>(line_number_);
}

const void* GetProgramCounter()
{
    return _ReturnAddress();
    //return __builtin_extract_return_addr(__builtin_return_address(0));
}

