/*
 * rest_api_helper.h
 *
 *  Created on: Feb 10, 2014
 *      Author: mfbari
 */

#ifndef REST_API_HELPER_H_
#define REST_API_HELPER_H_

#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>  
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include <cstdlib>

#include "../database/cassandra_db.h"
#include "../global.h"
#include "../models/home_agent_index.h"
#include "../models/user.h"
#include "../models/device.h"
#include "../controllers/home_agent_index_cassandra_controller.h"
#include "../controllers/user_cassandra_controller.h"
#include "../controllers/device_cassandra_controller.h"

using namespace std;

class RESTAPIHelper
{
	boost::shared_ptr <CassandraDBDriver> database;
	string homeAgentAlias;

	void replaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace) {
		size_t pos = 0;
		while ((pos = subject.find(search, pos)) != std::string::npos) {
			subject.replace(pos, search.length(), replace);
			pos += replace.length();
		}
	}

	void escapeSingleQuote(string& value){
		replaceStringInPlace(value, "'","''");
	}

	string timestamp2String(unsigned long value)
	{
		string result = "0";
		try
	        {
        	    result = boost::lexical_cast<string>(value);
        	}
	        catch(const boost::bad_lexical_cast &)
        	{
	        }
            	return result;
	}
	unsigned long string2Timestamp(string value)
	{
		unsigned long result = 0;
		try
	        {
        	    result = boost::lexical_cast<unsigned long>(value);
        	}
	        catch(const boost::bad_lexical_cast &)
        	{
	        }
            	return result;
	}

	string port2String(int value)
	{
		string result = "0";
		try
	        {
        	    result = boost::lexical_cast<string>(value);
        	}
	        catch(const boost::bad_lexical_cast &)
        	{
	        }
            	return result;
	}
public:
	RESTAPIHelper(string homeAgentAlias):homeAgentAlias(homeAgentAlias)
	{
		database = boost::dynamic_pointer_cast<CassandraDBDriver>(CassandraDBDriver::getDatabaseDriverObject());
		database->openConnection();	
	}

	////////////////////////////////////////////
	//  USER 				  //
	////////////////////////////////////////////

	string registerUser(const string& username, const string& password, const string& email,
					const string& fullname, const string& location, const string& affiliation)
	{
		/*
		escapeSingleQuote(username);
		escapeSingleQuote(password);
		escapeSingleQuote(email);
		escapeSingleQuote(fullname);
		escapeSingleQuote(location);
		escapeSingleQuote(affiliation);
		*/

		UserCassandraController userController(database);
		
		if(!userController.isUsernameAvailable(username))
		{
			return "{\"status\":\"error\", \"error\":\"APP:7501\"}";
		}
		
		boost::shared_ptr <User> user = boost::shared_ptr <User> (new User(username, password, email, fullname, location, affiliation));
		int result = userController.addUser(user);
		if(result == 0)
		{
			return "{\"status\":\"success\"}";
		}
		string resultStr = boost::lexical_cast<string>(result);
		return "{\"status\":\"error\", \"error\":\"CDB:" + resultStr + "\"}";
	}

	string isUsernameAvailable(const string& username)
	{
		UserCassandraController userController(database);
		
		string errorCode = "";
		bool isAvailable = userController.isUsernameAvailable(username, errorCode);
		
		if(!errorCode.empty())
		{
			return "{\"status\":\"error\", \"error\":\"CDB:" + errorCode + "\"}";
		}
		if(isAvailable)
		{
			return "{\"status\":\"success\", \"availability\":\"yes\"}";
		}

		string suggestions = "";
		for(int i = 0; i < REST_API_SUGGESION_COUNT; i++)
		{
			ostringstream availableUsername;
			int rnd = rand() % 100;
			availableUsername << username;	
			availableUsername << rnd;
			string uname(availableUsername.str());
			isAvailable = userController.isUsernameAvailable(username, errorCode);
			if(!errorCode.empty())
			{
				return "{\"status\":\"error\", \"error\":\"CDB:" + errorCode + "\"}";
			}
			if(isAvailable)
			{
				suggestions.append("\"" + uname + "\",");
			}	
			else
			{
				i--;
			}
		}
		if(suggestions != "")
		{
			suggestions = suggestions.substr(0, suggestions.size() - 1);
		}
		return "{\"status\":\"success\", \"availability\":\"no\", \"suggestion\":[" + suggestions + "]}";
	}

	string authenticateUser(const string& username, const string& password)
	{
		UserCassandraController userController(database);
		
		string errorCode = "";
		bool isAuthentic = userController.authenticateUser(username, password, errorCode);

		if(!errorCode.empty())
		{
			return "{\"status\":\"error\", \"error\":\"CDB:" + errorCode + "\"}";
		}

		if(isAuthentic)
		{
			return "{\"status\":\"success\", \"authenticated\":\"yes\"}";
		}
		return "{\"status\":\"success\", \"authenticated\":\"no\"}";
	}

	////////////////////////////////////////////
	//  DEVICE 				  //
	////////////////////////////////////////////


	string registerDevice(const string& devicename, const string& username, const string& type, const string& ip,
					const string& port, const string& os, const string& description, const string& isIndexed)
	{
		UserCassandraController userController(database);
		
		string errorCode = "";
		bool isAvailable = userController.isUsernameAvailable(username, errorCode);
		if(isAvailable)
		{
			return "{\"status\":\"error\", \"error\":\"APP:7502\"}";
		}

		DeviceCassandraController deviceController(database);

		errorCode = "";
		isAvailable = deviceController.isDevicenameAvailable(devicename, username, errorCode);
		if(!errorCode.empty())
		{
			return "{\"status\":\"error\", \"error\":\"CDB:" + errorCode + "\"}";
		}
		if(!isAvailable)
		{
			return "{\"status\":\"error\", \"error\":\"APP:6401\"}";
		}

		time_t timestamp = time(NULL);

		boost::shared_ptr <Device> device = boost::shared_ptr <Device> (new Device(username, devicename, type, ip, boost::lexical_cast<unsigned short>(port),
			"", 0, "", "",
			timestamp, os, description, "", (boost::iequals(isIndexed, "true"))?true:false, timestamp));
		int result = deviceController.addDevice(device);
		if(result == 0)
		{
			return "{\"status\":\"success\"}";
		}
		string resultStr = boost::lexical_cast<string>(result);
		return "{\"status\":\"error\", \"error\":\"CDB:" + resultStr + "\"}";
	}

	string registerDevice(const string& devicename, const string& username, const string& ip, const string& port, const string& isIndexed, boost::unordered_map<string, string>& params)
	{
		UserCassandraController userController(database);
		
		string errorCode = "";
		bool isAvailable = userController.isUsernameAvailable(username, errorCode);
		if(isAvailable)
		{
			return "{\"status\":\"error\", \"error\":\"APP:7502\"}";
		}

		DeviceCassandraController deviceController(database);

		errorCode = "";
		isAvailable = deviceController.isDevicenameAvailable(devicename, username, errorCode);
		if(!errorCode.empty())
		{
			return "{\"status\":\"error\", \"error\":\"CDB:" + errorCode + "\"}";
		}
		if(!isAvailable)
		{
			return "{\"status\":\"error\", \"error\":\"APP:6401\"}";
		}

		time_t timestamp = time(NULL);

		boost::shared_ptr <Device> device = boost::shared_ptr <Device> (new Device(username, devicename, ip, boost::lexical_cast<unsigned short>(port), (boost::iequals(isIndexed, "true"))?true:false,
			timestamp, params));
		int result = deviceController.addDevice(device);
		if(result == 0)
		{
			return "{\"status\":\"success\"}";
		}
		string resultStr = boost::lexical_cast<string>(result);
		return "{\"status\":\"error\", \"error\":\"CDB:" + resultStr + "\"}";
	}


	string isDevicenameAvailable(const string& devicename, const string& username)
	{
		DeviceCassandraController deviceController(database);
		
		string errorCode = "";
		bool isAvailable = deviceController.isDevicenameAvailable(devicename, username, errorCode);
		
		if(!errorCode.empty())
		{
			return "{\"status\":\"error\", \"error\":\"CDB:" + errorCode + "\"}";
		}

		if(isAvailable)
		{
			return "{\"status\":\"success\", \"availability\":\"yes\"}";
		}

		string suggestions = "";
		for(int i = 0; i < REST_API_SUGGESION_COUNT; i++)
		{
			ostringstream availableDevicename;
			int rnd = rand() % 100;
			availableDevicename << devicename;
			availableDevicename << rnd;
			string dname(availableDevicename.str());
			isAvailable = deviceController.isDevicenameAvailable(dname, username, errorCode);
			if(!errorCode.empty())
			{
				return "{\"status\":\"error\", \"error\":\"CDB:" + errorCode + "\"}";
			}
			if(isAvailable)
			{
				suggestions.append("\"" + dname + "\",");
			}
			else
			{
				i--;
			}
		}
		if(suggestions != "")
		{
			suggestions = suggestions.substr(0, suggestions.size() - 1);
		}
		return "{\"status\":\"success\", \"availability\":\"no\", \"suggestion\":[" + suggestions + "]}";
	}

	string deleteDevice(const string& devicename, const string& username)
	{
		DeviceCassandraController deviceController(database);
		int result = deviceController.deleteDevice(devicename, username);
		if(result == 0)
		{
			return "{\"status\":\"success\"}";
		}
		if(result == -1)
		{
			return "{\"status\":\"error\", \"error\":\"APP:6402\"}";
		}
		string resultStr = boost::lexical_cast<string>(result);
		return "{\"status\":\"error\", \"error\":\"CDB:" + resultStr + "\"}";
	}

	string updateDevice(const string& devicename, const string& username, boost::unordered_map<string, string>& params)
	{
		DeviceCassandraController deviceController(database);
		int result = deviceController.updateDevice(devicename, username, params);
		if(result == 0)
		{
			return "{\"status\":\"success\"}";
		}
		if(result == -1)
		{
			return "{\"status\":\"error\", \"error\":\"APP:6402\"}";
		}
		string resultStr = boost::lexical_cast<string>(result);
		return "{\"status\":\"error\", \"error\":\"CDB:" + resultStr + "\"}";
	}

	string updateDevice(const string& currentDevicename, const string& newDevicename, const string& username, const string& ip, const string& port, const string& 					publicFolder, const string& privateFolder)
	{
		DeviceCassandraController deviceController(database);
		int result = deviceController.updateDevice(currentDevicename, newDevicename, username, ip, port, publicFolder, privateFolder);
		if(result == 0)
		{
			return "{\"status\":\"success\"}";
		}
		if(result == -1)
		{
			return "{\"status\":\"error\", \"error\":\"APP:6402\"}";
		}
		string resultStr = boost::lexical_cast<string>(result);
		return "{\"status\":\"error\", \"error\":\"CDB:" + resultStr + "\"}";
	}

	string updateDeviceMetadata(const string& devicename, const string& username, const string& metadata)
	{
		DeviceCassandraController deviceController(database);
		int result = deviceController.updateDeviceMetadata(devicename, username, metadata);
		if(result == 0)
		{
			return "{\"status\":\"success\"}";
		}
		if(result == -1)
		{
			return "{\"status\":\"error\", \"error\":\"APP:6402\"}";
		}
		string resultStr = boost::lexical_cast<string>(result);
		return "{\"status\":\"error\", \"error\":\"CDB:" + resultStr + "\"}";
	}

	string updateDeviceIPPort(const string& devicename, const string& username, const string& ip, const string& port)
	{
		DeviceCassandraController deviceController(database);
		int result = deviceController.updateDeviceIPPort(devicename, username, ip, port);
		if(result == 0)
		{
			return "{\"status\":\"success\"}";
		}
		if(result == -1)
		{
			return "{\"status\":\"error\", \"error\":\"APP:6402\"}";
		}
		string resultStr = boost::lexical_cast<string>(result);
		return "{\"status\":\"error\", \"error\":\"CDB:" + resultStr + "\"}";
	}

	string getUserDevices(const string& username)
	{
		DeviceCassandraController deviceController(database);
		
		string errorCode = "";
		vector < boost::shared_ptr <Device> > deviceVector = deviceController.getUserDevices(username, errorCode);
		if(!errorCode.empty())
		{
			return "{\"status\":\"error\", \"error\":\"CDB:" + errorCode + "\"}";
		}
		if(deviceVector.size() <= 0)
		{
			return "{\"status\":\"success\", \"username\":\"" + username + "\", \"devices\":[]}";
		}

		bool dataAppended = false;
		string result = "{\"status\":\"success\", \"username\":\"" + username + "\", \"devices\":[";
		for (vector < boost::shared_ptr <Device> >::iterator it = deviceVector.begin(); it != deviceVector.end(); ++it)
		{
			result.append("{\"devicename\":\"" + (*it)->getDevicename() + "\", \"type\":\"" + (*it)->getType() + "\", \"ip\":\"" + (*it)->getIp() + 
					"\", \"port\":\"" + boost::lexical_cast<string>((*it)->getPort()) + "\", \"dir_ip\":\"" + (*it)->getDirIp() + 
					"\", \"dir_port\":\"" + boost::lexical_cast<string>((*it)->getDirPort()) + "\", \"public_folder\":\"" + 
					(*it)->getPublicFolder() + "\", \"private_folder\":\"" + (*it)->getPrivateFolder() + 
					"\", \"os\":\"" + (*it)->getOs() + "\", \"description\":\"" + (*it)->getDescription() + "\"},");
			if(!dataAppended)
			{
				dataAppended = true;
			}
		}
		if(dataAppended)
		{
			result = result.substr(0, result.size() - 1);
		}
		result.append("]}");
		return result;
	}

	////////////////////////////////////////////
	//  CRAWLER 				  //
	////////////////////////////////////////////

	string getHomeAgentList()
	{
		HomeAgentIndexCassandraController homeAgentIndexController(database);

                string result = "{\"home_agents\":[";
		string list = "";
                vector < boost::shared_ptr <HomeAgentIndex> > homeAgents = homeAgentIndexController.getAllHomeAgentIndex();
                for (vector < boost::shared_ptr <HomeAgentIndex> >::iterator hit = homeAgents.begin(); hit != homeAgents.end(); ++hit)
                {
                        list += ("{\"alias\":\"" + (*hit)->getName() + ".pwebproject.net\", \"ip\":\"" + (*hit)->getIp()  + "\", \"port\":\"" + port2String((*hit)->getPort()) + "\"},");
                }
		if(list.size() > 0)
		{
			list = list.substr(0, list.size() - 1);
		}
		result += list;
                result += "]}";
		return result;
	}


	string getDeviceList(const string& timestamp)
	{
		string result = "<getall><name>" + this->homeAgentAlias + "</name>";
		UserCassandraController userController(database);
		DeviceCassandraController deviceController(database);
		HomeAgentIndexCassandraController homeAgentIndexController(database);
		vector < boost::shared_ptr <User> > users = userController.getAllUser();

		result += "<neighbours>";
		vector < boost::shared_ptr <HomeAgentIndex> > homeAgents = homeAgentIndexController.getAllHomeAgentIndex();
		for (vector < boost::shared_ptr <HomeAgentIndex> >::iterator hit = homeAgents.begin(); hit != homeAgents.end(); ++hit)
		{
			result += "<home agent>";
			result += "<hostname>" + (*hit)->getName() + ".pwebproject.net</hostname>";
			result += "<port>" + port2String((*hit)->getPort()) + "</port>";
			result += "</home agent>";
		}
		result += "</neighbours>";

		string errorCode = "";
		string contentUpdates = "";
		result += "<devices>";
		for (vector < boost::shared_ptr <User> >::iterator uit = users.begin(); uit != users.end(); ++uit)
		{
			vector < boost::shared_ptr <Device> > devices = deviceController.getUserDevices((*uit)->getUsername(), errorCode);
			for (vector < boost::shared_ptr <Device> >::iterator dit = devices.begin(); dit != devices.end(); ++dit)
			{
				if((*dit)->getLastSeen() >= string2Timestamp(timestamp))
				{
					result += "<device>";
					result.append("<owner>" + (*uit)->getFullname() + "</owner>");
					result.append("<name>" + (*dit)->getDevicename() + "." + (*uit)->getUsername() + "</name>");
					result.append("<port>" + port2String((*dit)->getPort()) + "</port>");
					result.append("<type>" + (*dit)->getType() + "</type>");
					result.append("<timestamp>" + timestamp2String((*dit)->getLastSeen()) + "</timestamp>");
					result.append("<location>" + (*uit)->getLocation() + "</location>");
					result.append("<description>" + (*dit)->getDescription() + "</description>");
					result += "</device>";
				}
				if((*dit)->getContentTimestamp() >= string2Timestamp(timestamp))
				{
					contentUpdates += (*dit)->getDevicename() + "." + (*uit)->getUsername() + ",";
				}
			}
		}
		result += "</devices>";
		if(contentUpdates.size() > 0)
		{
			contentUpdates = contentUpdates.substr(0, contentUpdates.size() - 1);
		}
		result += "<content updates>" + contentUpdates + "</content updates>";
		result += "</getall>";
		return result;
	}
	
	string getContentList(const string& devicename, const string& username)
	{
		DeviceCassandraController deviceController(database);

		string errorCode = "";
		boost::shared_ptr <Device> device = deviceController.getDevice(devicename, username, errorCode);
		if(!errorCode.empty())
		{
			return "{\"status\":\"error\", \"error\":\"CDB:" + errorCode + "\"}";
		}
		if(!device->isSearchable())
		{
			return "{\"status\":\"error\", \"error\":\"APP:6403\"}";
		}
		return device->getContentMeta();
	}
	
};


#endif /* REST_API_HELPER_H_ */
