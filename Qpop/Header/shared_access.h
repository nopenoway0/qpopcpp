#ifndef SHARED_ACCESS_H
#define SHARED_ACCESS_H
#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>

boost::condition_variable condition;
volatile int _close_qpop;
boost::mutex _monitor_pause;
boost::mutex _delay_after_click;
volatile int delay_count;

#endif