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

#include "Mona/RTMP/RTMPSender.h"
#include "Mona/FlashWriter.h"
#include "Mona/Logs.h"

using namespace std;
using namespace Poco;
using namespace Poco::Net;

namespace Mona {

RTMPSender::RTMPSender(const SharedPtr<RC4_KEY>& pEncryptKey,SocketHandler<StreamSocket>& handler): TCPSender(handler,true),_pEncryptKey(pEncryptKey),_chunkSize(DEFAULT_CHUNKSIZE),_sizePos(0) {
}

RTMPSender::RTMPSender(const RTMPSender& sender): TCPSender(sender.handler,true),_pEncryptKey(sender._pEncryptKey),_chunkSize(DEFAULT_CHUNKSIZE),_sizePos(0) {
}

RTMPSender::~RTMPSender() {

}

bool RTMPSender::flush() {
	pack();
	dump();
	if(!_pEncryptKey.isNull())
		RC4(_pEncryptKey.get(),size(),begin(),(UInt8*)begin());
	return TCPSender::flush();
}

void RTMPSender::pack() {
	if(writer.stream.size()==3) { // Just usefull for FlashWriter and the connection step (to delete the response.endObject() part)
		writer.stream.clear();
		return;
	}
	if(_sizePos>0) {
		// writer the size of the precedent playload!
		_channel.bodySize = writer.stream.size()-_sizePos+4-_channel.headerSize;
		writer.stream.resetWriting(_sizePos);
		writer.writer.write24(_channel.bodySize);
		writer.stream.resetWriting(_sizePos-4+_channel.headerSize+_channel.bodySize);
		_sizePos=0;
	}
}

AMFWriter& RTMPSender::write(UInt32 id,AMF::ContentType type,UInt32 time,UInt32 streamId,MemoryReader* pData) {
	pack();

	if(time<_channel.time) {
		ERROR("Not increasing time on RTMP channel %u",id)
		time = _channel.time;
	}

	UInt8 headerFlag=0;
	if(_channel.streamId == streamId) {
		++headerFlag;
		if(pData && _channel.type == type && _channel.bodySize == pData->available())
			++headerFlag;
	}

	_channel.type = type;
	_channel.streamId=streamId;
	_channel.time=time;
	
	BinaryWriter& data = writer.writer;
	data.write8((headerFlag<<6)|id);

	_channel.headerSize = 12 - 4*headerFlag;

	if(_channel.headerSize<12)
		time -= _channel.time; // relative time!

	if(time<0xFFFFFF)
		data.write24(time);
	else {
		data.write24(0xFFFFFF);
		_channel.headerSize += 4;
	}

	if(_channel.headerSize > 4) {
		_sizePos = writer.stream.size();
		writer.stream.next(3); // For size
		data.write8(type);
		if(_channel.headerSize > 8) {
			data.write8(streamId);
			data.write8(streamId>>8);
			data.write8(streamId>>16);
			data.write8(streamId>>24);
			// if(type==AMF::DATA) TODO?
				//	pWriter->write8(0);
			if(_channel.headerSize >12)
				data.write32(time);
		}
	}

	if(pData) {
		data.writeRaw(pData->current(),pData->available());
		pack();
		return FlashWriter::AMFWriterNull;
	}
	return writer;
}





} // namespace Mona
