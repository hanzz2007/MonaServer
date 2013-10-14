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

#pragma once

#include "Mona/Mona.h"
#include "Mona/MemoryReader.h"
#include "Mona/MemoryWriter.h"
#include "Mona/Time.h"
#include "Poco/Buffer.h"
#include <openssl/aes.h>

namespace Mona {

#define RTMFP_SYMETRIC_KEY (Mona::UInt8*)"Adobe Systems 02"
#define RTMFP_MIN_PACKET_SIZE	12
#define RTMFP_MAX_PACKET_LENGTH 1192
#define RTMFP_TIMESTAMP_SCALE	4
#define RTMFP_PACKET_RECV_SIZE	2048	
#define RTMFP_PACKET_SEND_SIZE	1215

class RTMFPEngine
{
public:
	enum Direction {
		DECRYPT=0,
		ENCRYPT
	};
	enum Type {
		DEFAULT=0,
		EMPTY,
		SYMMETRIC
	};
	RTMFPEngine();
	RTMFPEngine(const Mona::UInt8* key,Direction direction);
	RTMFPEngine(const RTMFPEngine& other);
	RTMFPEngine(const RTMFPEngine& other,Type type);
	virtual ~RTMFPEngine();

	RTMFPEngine&  operator=(const RTMFPEngine& other);
	RTMFPEngine	next(Type type);
	RTMFPEngine	next();
	void		process(const Mona::UInt8* in,Mona::UInt8* out,Mona::UInt32 size);

	const Type	type;
private:
	static RTMFPEngine	_DefaultDecrypt;
	static RTMFPEngine	_DefaultEncrypt;

	Direction	_direction;
	AES_KEY		_key;
	
};

inline RTMFPEngine RTMFPEngine::next() {
	return RTMFPEngine(*this);
}

inline RTMFPEngine RTMFPEngine::next(Type type) {
	return RTMFPEngine(*this,type);
}


class RTMFP
{
public:
	static Mona::UInt32				Unpack(MemoryReader& packet);
	static void						Pack(MemoryWriter& packet,Mona::UInt32 farId);

	static bool						ReadCRC(MemoryReader& packet);
	static void						WriteCRC(MemoryWriter& packet);
	static bool						Decode(RTMFPEngine& aesDecrypt,MemoryReader& packet);
	static void						Encode(RTMFPEngine& aesEncrypt,MemoryWriter& packet);
	

	static void						ComputeAsymetricKeys(const Poco::Buffer<Mona::UInt8>& sharedSecret,
														const Mona::UInt8* initiatorNonce,Mona::UInt16 initNonceSize,
														const Mona::UInt8* responderNonce,Mona::UInt16 respNonceSize,
														 Mona::UInt8* requestKey,
														 Mona::UInt8* responseKey);
	static Mona::UInt16				TimeNow();
	static Mona::UInt16				Time(Mona::Int64 timeVal);

private:
	static Mona::UInt16				CheckSum(MemoryReader& packet);
};


inline Mona::UInt16 RTMFP::TimeNow() {

	return Mona::Time().toInt();
}

}  // namespace Mona
