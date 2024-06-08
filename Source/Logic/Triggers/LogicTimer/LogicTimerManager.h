#ifndef LOGIC_TIMER_MANAGER_H
#define LOGIC_TIMER_MANAGER_H

#include <list>
#include "LogicTimer.h"

/**
* Manager of logic timers.
*/

class LogicTimerManager : public std::list< LogicTimer * >
{
public:
	LogicTimerManager() {}
	~LogicTimerManager();
	void clear();

	void add( LogicTimer *logic_timer ); // Va a pertenecer a este manager
	void add( const std::string &script_name, float timeout, LogicTimer::Type type );
	void del( const std::string &ascript_name );
	iterator del( iterator it );

	void update( float deltaTime );
};

#endif //LOGIC_TIMER_MANAGER_H
