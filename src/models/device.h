/*
 * device.h
 *
 *  Created on: Feb 6, 2014
 *      Author: sr2chowd
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include <string>

using namespace std;

class Device
{
	string username, devicename;
	string type;
	string ip, dirIp;
	unsigned short port,dirPort;
	string publicFolder, privateFolder;
	unsigned long lastSeen, contentTimestamp;
	string os;
	string description;
	string contentMeta;
	bool searchable;

public:

	static const string COL_USER_NAME, COL_DEVICE_NAME, COL_TYPE, COL_IP, COL_DIR_IP, COL_PUBLIC_FOLDER, COL_PRIVATE_FOLDER, COL_OS, COL_DESCRIPTION, COL_CONTENT_META;
	static const string COL_PORT, COL_DIR_PORT, COL_LAST_SEEN, COL_CONTENT_TIME_STAMP, COL_SEARCHABLE;
	static string TABLE_NAME;

	Device(const string& username, const string& devicename, const string& type, const string& ip, unsigned short port,
			const string& dirIp, unsigned short dirPort, const string& publicFolder, const string& privateFolder,
			unsigned long lastSeen, const string& os, const string& description, const string& contentMeta,
			bool searchable, unsigned long contentTimestamp):
				username(username), devicename(devicename), type(type), ip(ip), port(port),
				dirIp(dirIp), dirPort(dirPort), publicFolder(publicFolder), privateFolder(privateFolder),
				lastSeen(lastSeen), os(os), description(description), contentMeta(contentMeta),
				searchable(searchable), contentTimestamp(contentTimestamp)
	{
		;
	}

	string& getContentMeta()
	{
		return contentMeta;
	}

	void setContentMeta(const string& contentMeta)
	{
		this->contentMeta = contentMeta;
	}

	unsigned long getContentTimestamp()
	{
		return contentTimestamp;
	}

	void setContentTimestamp(unsigned long contentTimestamp)
	{
		this->contentTimestamp = contentTimestamp;
	}

	string& getDescription()
	{
		return description;
	}

	void setDescription(const string& description)
	{
		this->description = description;
	}

	string& getDevicename()
	{
		return devicename;
	}

	void setDevicename(const string& devicename)
	{
		this->devicename = devicename;
	}

	string& getDirIp()
	{
		return dirIp;
	}

	void setDirIp(const string& dirIp)
	{
		this->dirIp = dirIp;
	}

	unsigned short getDirPort()
	{
		return dirPort;
	}

	void setDirPort(unsigned short dirPort)
	{
		this->dirPort = dirPort;
	}

	string& getIp()
	{
		return ip;
	}

	void setIp(const string& ip)
	{
		this->ip = ip;
	}

	unsigned long getLastSeen()
	{
		return lastSeen;
	}

	void setLastSeen(unsigned long lastSeen)
	{
		this->lastSeen = lastSeen;
	}

	string& getOs()
	{
		return os;
	}

	void setOs(const string& os)
	{
		this->os = os;
	}

	unsigned short getPort()
	{
		return port;
	}

	void setPort(unsigned short port)
	{
		this->port = port;
	}

	string& getPrivateFolder()
	{
		return privateFolder;
	}

	void setPrivateFolder(const string& privateFolder)
	{
		this->privateFolder = privateFolder;
	}

	string& getPublicFolder()
	{
		return publicFolder;
	}

	void setPublicFolder(const string& publicFolder)
	{
		this->publicFolder = publicFolder;
	}

	bool isSearchable()
	{
		return searchable;
	}

	void setSearchable(bool searchable)
	{
		this->searchable = searchable;
	}

	string& getType()
	{
		return type;
	}

	void setType(const string& type)
	{
		this->type = type;
	}

	string& getUsername()
	{
		return username;
	}

	void setUsername(const string& username)
	{
		this->username = username;
	}
};


#endif /* DEVICE_H_ */
