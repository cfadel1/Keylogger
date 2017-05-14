/*
Used to determine the precise time intervals for the execution of the threads. 
*/

#ifndef TIMER_H
#define TIMER_H

#include <thread>
#include <chrono>

using namespace std;
using namespace chrono;

class Timer
{
	thread Thread;
	bool Alive = false;

	//long is used to avoid an overflow 
	long CallNumber = -1L;
	long repeat_count = -1L;
	milliseconds interval = milliseconds(0);

	//<void<void>> is a pointer to a function
	//can accept a function that takes nothing and does not return anything 
	function<void(void)> funct = nullptr;

	void SleepAndRun()
	{
		this_thread::sleep_for(interval);
		if (Alive)
		{
			//() calls the function called function 
			//() calls the functoin that the above function returned
			Function()();
		}
	}

	void ThreadFunc()
	{
		if (CallNumber == Infinite)
		{
			while (Alive)
				SleepAndRun();
		}
		else
		{
			while (repeat_count--) 
				SleepAndRun();
		}
	}

public:
	static const long Infinite = -1L;
	//Empty constructor, used to create a timer as a global variable
	Timer() {}

	//reference to a constant object, the function object will be initialized to f
	Timer(const function<void(void)> &f) : funct(f) {}

	Timer(const function<void(void)> &f,
		  const unsigned long &i,
		  const long repeat = Timer::Infinite) : funct(f), interval(milliseconds(i)), CallNumber(repeat) {}

	//function to start the timer
	void Start(bool Async = true)
	{
		if (IsAlive())
			return;
		
		Alive = true;
		repeat_count = CallNumber;

		//Thread vairable created above
		//Parameters, function that will be executed and a pointer to this class
		//Checks if there's an asynchronous call
		if (Async)
		{
			Thread = thread(&Timer::ThreadFunc, this);
		}
		else {
			//function is a concurent thread that will block the main one 
			this->ThreadFunc();
		}
	}

	void Stop()
	{
		Alive = false;
		//no more concurrent execution
		//only the main one will keep on running
		Thread.join();
	}

	//Sets the function that will be executed
	void SetFunction(const std::function<void(void)> &f)
	{
		funct = f;
	}

	//Checks if thread is alive
	bool IsAlive() const 
	{ 
		return Alive; 
	}

	//Set the number of repetitions
	void RepeatCount(const long r)
	{
		if (Alive) 
			return;

		CallNumber = r;
	}

	//Get number of iterations left
	long GetLeftCount() const { 
		return repeat_count; 
	}

	//Total number of repititions, we set it once at the beginning
	long RepeatCount() const { 
		return CallNumber;
	}

	//Function that sets an interval
	void SetInterval(const unsigned long &i)
	{
		if (Alive) 
			return;

		interval = milliseconds(i);
	}

	//inteval is of type milliseconds 
	//count will return a number which can be placed in a long data type
	unsigned long Interval() const 
	{ 
		return interval.count(); 
	}

	//returns a function
	const function<void(void)> &Function() const
	{
		return funct;
	}
};

#endif 