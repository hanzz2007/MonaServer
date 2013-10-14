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

#include "Mona/MemoryReader.h"
#include "Mona/Logs.h"

using namespace std;
using namespace Poco;

namespace Mona {

MemoryReader MemoryReader::Null(NULL,0);

MemoryReader::MemoryReader(const UInt8* buffer,UInt32 size) : _memory((const char*)buffer,size),BinaryReader(_memory),fragments(1) {
}


// Consctruction by copy
MemoryReader::MemoryReader(MemoryReader& other) : _memory(other._memory),BinaryReader(_memory),fragments(other.fragments) {
}


MemoryReader::~MemoryReader() {
}

void MemoryReader::shrink(UInt32 rest) {
	if(rest>available()) {
		WARN("rest %u more upper than available %u bytes",rest,available());
		rest = available();
	}
	_memory.resize(position()+rest);
}



} // namespace Mona
