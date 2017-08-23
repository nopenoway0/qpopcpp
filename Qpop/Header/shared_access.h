#ifndef SHARED_ACCESS_H
#define SHARED_ACCESS_H
#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>

boost::condition_variable condition;
volatile int _close_qpop;
std::string _profile_name;
volatile int _profile_changed;
boost::mutex _process_chosen_lock;
boost::mutex _close_qpop_lock;
volatile int _paused;
boost::mutex _img_number_lock;
boost::mutex _monitor_pause;
volatile int _img_number;

#endif