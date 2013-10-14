/* 
	Copyright 2013 Mona - mathieu.poux[a]gmail.com
 
	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License received along this program for more
	details (or else see http://www.gnu.org/licenses/).

	This file is a part of Mona.
*/

#include "Mona/Trigger.h"
#include "Mona/Logs.h"
#include "Poco/Exception.h"


using namespace std;
using namespace Poco;

namespace Mona {

Trigger::Trigger() : _time(0),_cycle(-1),_running(false) {
	
}


Trigger::~Trigger() {
}

void Trigger::reset() {
	_timeInit = Mona::Time();
	_time=0;
	_cycle=-1;
}

void Trigger::start() {
	if(_running)
		return;
	reset();
	_running=true;
}

bool Trigger::raise() {
	if(!_running)
		return false;
	// Wait at least 1 sec before to begin the repeat cycle, it means that it will be between 1 and 3 sec in truth (freg mangement is set to 2)
	if(_time==0 && !_timeInit.isElapsed(1000000))
		return false;
	++_time;
	if(_time>=_cycle) {
		_time=0;
		++_cycle;
		if(_cycle==7)
			throw Exception("Repeat trigger failed");
		DEBUG("Repeat trigger cycle %02x",_cycle+1);
		return true;
	}
	return false;
}



} // namespace Mona
