/*
 * protocol_helper.h
 *
 *  Created on: Jan 15, 2014
 *      Author: sr2chowd
 */

#ifndef PROTOCOL_HELPER_H_
#define PROTOCOL_HELPER_H_

#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

using namespace std;

class ProtocolHelper
{
public:

	/**
	 * The placeDataIntoByteBuffer function is a template function and
	 * it can serialize atomic type data and place it into a byte buffer
	 * Custome types, i.e., sturctures or classes are supposed to be
	 * supported as long as it overloads the <b>sizeof()</b> operator
	 * and the data can be "deterministically serialized (?!)". STL types
	 * are not supported.
	 *
	 * @param offset The starting byte offset to write into the buffer
	 * @param value	 The value to be serialized
	 * @param buf	 The byte buffer
	 * @return Returns the next available position in the byte buffer for
	 * writing
	 */

	template <class AtomicType>
	static long placeAtomicDataIntoByteBuffer(char *buf, long offset, const AtomicType& value)
	{
		memcpy(buf + offset, &value, sizeof(AtomicType));
		return offset + sizeof(AtomicType);
	}

	static int placeStringIntoByteBuffer(char *buf, int offset, const std::string& value)
	{
		//copy the length of the string
		unsigned char strLength = (unsigned char)value.length();
		memcpy(buf + offset, &strLength, sizeof(strLength));
		offset += sizeof(strLength);

		//now copy the string data
		memcpy(buf + offset, value.c_str(), strLength * sizeof(char));
		offset += sizeof(char) * strLength;
		return offset;
	}

	/**
	 * The function extractDataFromByteBuffer extracts atomic type data
	 * from a byte array.
	 *
	 * @param offset  Starting byte offset of the byte buffer
	 * @param buf	  Pointer to the byte buffer
	 * @param dataPtr A pointer to hold the extracted data. The type of
	 * passed pointer determines the type of returned data
	 * @return Returns the next available offset in the byte buffer, from
	 * where new data can be read.
	 */
	template <class AtomicType>
	static long extractAtomicDataFromByteBuffer(char *buf, long offset, AtomicType *dataPtr)
	{
		memcpy(dataPtr, buf + offset, sizeof(AtomicType));
		return offset + sizeof(AtomicType);
	}

	static long extractStringFromByteBuffer(char *buf, long offset, std::string& strData)
	{
		//read the length of the string first;
		unsigned char strLength;
		memcpy(&strLength, buf + offset, sizeof(strLength));
		offset += sizeof(strLength);

		//read the string data
		strData = std::string(buf + offset, 0, strLength);
		return offset + sizeof(char) * strLength;
	}

	static void setBit(char *buf, long offset, int bit)
	{
		*(buf + offset) |= (1 << bit);
	}

	static void resetBit(char *buf, long offset, int bit)
	{
		*(buf + offset) &= ~(1 << bit);
	}

	static bool getBit(char *buf, long offset, int bit)
	{
		return ((*(buf + offset)) & (1 << bit)) ? true : false;
	}
};


#endif /* PROTOCOL_HELPER_H_ */
