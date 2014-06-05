#include "Logging.h"

void test_logging()
{
    ILOG(logfmt("thread(%1%) begin.") % boost::this_thread::get_id());

    for(int i=0; i<2; i++) {
        FLOG(logfmt("%1%, %2%") % 1 % 2);
        VLOG(logfmt("%1%, %2%") % 1 % 2);
        LOG_ASSERT(1!=1);
    }

    ILOG(logfmt("thread(%1%) finish.") % boost::this_thread::get_id());
}

int main(int argc, char* argv[])
{
    //Location loc(FROM_HERE);
    LogMessage::setTimeFormat("%m-%d %H:%M:%S.%f");
    Logger::get().set_stream(new ConsoleStream);

    ILOG(logfmt("main thread(%1%) entry.") % boost::this_thread::get_id());

    for(int i=0; i<2; i++) {
        FLOG(logfmt("%1%, %2%") % 1 % 2);
        DLOG(logfmt("%1%, %2%") % 1 % 2);
        LOG_ASSERT(1!=1);
    }

    for(int i=0; i<100; i++) {
        boost::thread(boost::bind(&test_logging));
    }

    system("pause");

    ILOG(logfmt("main thread(%1%) exit.") % boost::this_thread::get_id());

    return 0;
}


