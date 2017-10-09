#ifndef SHARED_ACCESS_H
#define SHARED_ACCESS_H
#include <thread>
#include <mutex>

std::condition_variable condition;
volatile int _close_qpop;
std::mutex _monitor_pause;
std::mutex _delay_after_click;
volatile int delay_count;

/**
 * Used to set the current delay count in the main while loop of qpop
 * @param count number of loop iterations that must be made
 */
inline void SetDelayCount(unsigned int count){
	_delay_after_click.lock();
	delay_count = count;
	_delay_after_click.unlock();
}

/**
 * Decerement delay count
 */
inline void DecDelayCount(){
	_delay_after_click.lock();
	delay_count--;
	_delay_after_click.unlock();	
}

#endif