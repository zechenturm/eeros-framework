#include <eeros/core/Executor.hpp>

Executor::Executor(double period)
{
	this->period = period;
	this->status = kStopped;
}

int Executor::getThreadId()
{
	return this->threadId;
}

double Executor::getPeriod()
{
	return this->period;
}

int Executor::getStatus()
{
	return this->status;
}

void Executor::addRunnable(Runnable* runnable)
{
	runnables.push_back(runnable);
}

void Executor::addRunnable(Runnable& runnable)
{
	runnables.push_back(&runnable);
}

void Executor::start()
{
	status = kRunning;
	threadId = ExecutorService::createNewThread(this);
}

bool Executor::isTerminated()
{
	return status == kStopped;
}
void Executor::stop()
{
	this->status = kStop;
}

void Executor::join()
{
	// TODO
}

void Executor::run()
{
	for(std::list<Runnable*>::iterator i = runnables.begin(); i != runnables.end(); i++)
	{
		(*i)->run();
	}
}