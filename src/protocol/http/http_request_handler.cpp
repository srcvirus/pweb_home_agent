/*
 * http_request_handler.cpp
 *
 *  Created on: 2014-02-10
 *      Author: sr2chowd
 */

#include "http_request_handler.h"
#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include "datastructure/http_mime_types.h"
#include "datastructure/http_reply.h"
#include "datastructure/http_request.h"
#include <ctype.h>
#include <algorithm>

using namespace std;

request_handler::request_handler(const string& homeAgentAlias):restapi(boost::shared_ptr <RESTAPIHelper> (new RESTAPIHelper(homeAgentAlias))), homeAgentAlias(homeAgentAlias)
{
	;
}

void request_handler::handle_request(const request& req, reply& rep)
{
	// Decode url to path.
	std::string request_path;

	unsigned long tid = (unsigned long) pthread_self();

	if (!url_decode(req.uri, request_path))
	{
		rep = reply::stock_reply(reply::bad_request);
		return;
	}

	QueryStringParser qsp;
	qsp.parse(request_path.substr(2));

	printf("[DEBUG] [Thread 0x%lx] http request_path: %s\n", tid,
			request_path.c_str());

	/*// Request path must be absolute and not contain "..".
	 if (request_path.empty() || request_path[0] != '/'
	 || request_path.find("..") != std::string::npos)
	 {
	 rep = reply::stock_reply(reply::bad_request);
	 return;
	 }*/

	/*// If path ends in slash (i.e. is a directory) then add "index.html".
	 if (request_path[request_path.size() - 1] == '/')
	 {
	 request_path += "index.html";
	 }*/

	/*// Determine the file extension.
	 std::size_t last_slash_pos = request_path.find_last_of("/");
	 std::size_t last_dot_pos = request_path.find_last_of(".");
	 std::string extension;
	 if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
	 {
	 extension = request_path.substr(last_dot_pos + 1);
	 }*/
	//std::string extension = "html";
	std::string extension = "json";

	// Fill out the reply to be sent to the client.
	//rep.status = reply::ok;
	this->build_response(qsp, rep.content, rep.status);
	rep.headers.resize(3);
	rep.headers[0].name = "Content-Length";
	rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
	rep.headers[1].name = "Content-Type";
	rep.headers[1].value = mime_types::extension_to_type(extension);
	rep.headers[2].name = "Access-Control-Allow-Origin";
	rep.headers[2].value = "*";
}

void request_handler::build_response(QueryStringParser& qsp,
		std::string& http_payload, reply::status_type& http_code)
{
	string methodKey = "method";
	string method_name;

	http_payload = "";
	http_code = reply::bad_request;

	if (qsp.get_value(methodKey, method_name))
	{
		if (strtolower(method_name) == "is_username_available")
		{
			string username;
			if (qsp.get_value("username", username))
			{
				http_payload.append(restapi->isUsernameAvailable(username));
				http_code = reply::ok;
			}
		}
		/*
		else if (strtolower(method_name) == "register_user")
		{
			string username, password, email, fullname, location, affiliation;
			if (qsp.get_value("username", username)
					&& qsp.get_value("password", password)
					&& qsp.get_value("email", email)
					&& qsp.get_value("fullname", fullname)
					&& qsp.get_value("location", location)
					&& qsp.get_value("affiliation", affiliation))
			{
				if(!isValidName(username))
					return;
				http_payload.append(restapi->registerUser(username, password, email, fullname, location, affiliation));
				http_code = reply::ok;
			}
		}
		*/
		else if (strtolower(method_name) == "register_user")
		{
			string username, password, email, fullname, location, affiliation;
			if (qsp.get_value("username", username) 
					&& qsp.get_value("password", password)
					&& qsp.get_value("email", email))
			{
				boost::unordered_map<string, string> params;
				
				if(!isValidName(username))
					return;

				if(qsp.get_value("fullname", fullname))
				{
					if(!fullname.empty())
						params["fullname"] = fullname;
				}
				if(qsp.get_value("location", location))
				{
					if(!location.empty())
						params["location"] = location;
				}
				if(qsp.get_value("affiliation", affiliation))
				{
					if(!affiliation.empty())
						params["affiliation"] = affiliation;
				}
				http_payload.append(restapi->registerUser(username, password, email, params));
				http_code = reply::ok;
			}
		}
		else if (strtolower(method_name) == "update_user")
		{
			string username, password, email, fullname, location, affiliation;
			if (qsp.get_value("username", username))
			{
				boost::unordered_map<string, string> params;
				
				if(!isValidName(username))
					return;

				if(qsp.get_value("password", password))
				{
					if(!password.empty())
						params["password"] = password;
				}
				if(qsp.get_value("email", email))
				{
					if(!email.empty())
						params["email"] = email;
				}
				if(qsp.get_value("fullname", fullname))
				{
					if(!fullname.empty())
						params["fullname"] = fullname;
				}
				if(qsp.get_value("location", location))
				{
					if(!location.empty())
						params["location"] = location;
				}
				if(qsp.get_value("affiliation", affiliation))
				{
					if(!affiliation.empty())
						params["affiliation"] = affiliation;
				}
				http_payload.append(restapi->updateUser(username, params));
				http_code = reply::ok;
			}
		}
		else if (strtolower(method_name) == "authenticate_user")
		{
			string username, password;
			if (qsp.get_value("username", username)
					&& qsp.get_value("password", password))
			{
				http_payload.append(restapi->authenticateUser(username, password));
				http_code = reply::ok;
			}
		}
		else if (strtolower(method_name) == "get_user_devices")
		{
			string username;
			if (qsp.get_value("username", username))
			{
				http_payload.append(restapi->getUserDevices(username));
				http_code = reply::ok;
			}
		}
		else if (strtolower(method_name) == "is_devicename_available")
		{
			string devicename, username;
			if (qsp.get_value("devicename", devicename)
					&& qsp.get_value("username", username))
			{
				http_payload.append(restapi->isDevicenameAvailable(devicename, username));
				http_code = reply::ok;
			}
		}
		/*
		else if (strtolower(method_name) == "update_device")
		{
			string devicename, username, newdevicename, ip, port, public_folder,
					private_folder;
			if (qsp.get_value("devicename", devicename)
					&& qsp.get_value("username", username)
					&& qsp.get_value("new_devicename", newdevicename)
					&& qsp.get_value("ip", ip) 
					&& qsp.get_value("port", port)
					&& qsp.get_value("public_folder", public_folder)
					&& qsp.get_value("private_folder", private_folder))
			{
				if(!isNumber(port))
					return;
				if(!isValidIP(ip))
					return;
				http_payload.append(restapi->updateDevice(devicename, newdevicename, username, ip, port, public_folder, private_folder));
				http_code = reply::ok;
			}
		}
		*/
		else if (strtolower(method_name) == "update_device")
		{
			string devicename, username, new_devicename, type, metadata, ip, port, dir_ip, dir_port, description, os, public_folder, private_folder, is_indexed;
			if (qsp.get_value("devicename", devicename) && qsp.get_value("username", username)){

				boost::unordered_map<string, string> params;
				
				if(!isValidName(username))
					return;
				if(!isValidName(devicename))
					return;

				if(qsp.get_value("new_devicename", new_devicename))
				{
					if(!isValidName(new_devicename))
						return;
					if(!new_devicename.empty())
						params["new_devicename"] = new_devicename;
				}
				if(qsp.get_value("type", type))
				{
					if(!type.empty())
						params["type"] = type;
				}
				if(qsp.get_value("metadata", metadata))
				{
					if(!metadata.empty())
						params["metadata"] = metadata;
				}
				if(qsp.get_value("dir_ip", dir_ip))
				{
					if(!isValidIP(dir_ip))
						return;
					if(!dir_ip.empty())
						params["dir_ip"] = dir_ip;
				}
				if(qsp.get_value("dir_port", dir_port))
				{
					if(!isNumber(dir_port))
						return;
					if(!dir_port.empty())
						params["dir_port"] = dir_port;
				}
				if(qsp.get_value("ip", ip))
				{
					if(!isValidIP(ip))
						return;
					if(!ip.empty())
						params["ip"] = ip;
				}
				if(qsp.get_value("port", port))
				{
					if(!isNumber(port))
						return;
					if(!port.empty())
						params["port"] = port;
				}
				if(qsp.get_value("os", os))
				{
					if(!os.empty())
						params["os"] = os;
				}
				if(qsp.get_value("description", description))
				{
					if(!description.empty())
						params["description"] = description;
				}
				if(qsp.get_value("public_folder", public_folder))
				{
					if(!public_folder.empty())
						params["public_folder"] = public_folder;
				}
				if(qsp.get_value("private_folder", private_folder))
				{
					if(!private_folder.empty())
						params["private_folder"] = private_folder;
				}
				if(qsp.get_value("is_indexed", is_indexed))
				{
					if(!boost::iequals(is_indexed, "true") && !boost::iequals(is_indexed, "false"))
						return;
					params["is_indexed"] = is_indexed;
				}

				http_payload.append(restapi->updateDevice(devicename, username, params));
				http_code = reply::ok;
			}
		}
		else if (strtolower(method_name) == "delete_device")
		{
			string devicename, username;
			if (qsp.get_value("username", username) && qsp.get_value("devicename", devicename))
			{
				http_payload.append(restapi->deleteDevice(devicename, username));
				http_code = reply::ok;
			}
		}
		/*
		else if (strtolower(method_name) == "register_device")
		{
			string devicename, username, type, ip, port, os, description, is_indexed;
			if (qsp.get_value("devicename", devicename) && qsp.get_value("username", username) 
					&& qsp.get_value("type", type)
					&& qsp.get_value("ip", ip) 
					&& qsp.get_value("port", port)
					&& qsp.get_value("os", os)
					&& qsp.get_value("description", description)
					&& qsp.get_value("is_indexed", is_indexed))
			{
				if(!isValidName(username))
					return;
				if(!isValidName(devicename))
					return;
				if(!isValidIP(ip))
					return;
				if(!isNumber(port))
					return;
				if(!boost::iequals(is_indexed, "true") && !boost::iequals(is_indexed, "false"))
					return;
				http_payload.append(restapi->registerDevice(devicename, username, type, ip, port, os, description, is_indexed));
				http_code = reply::ok;
			}
		}
		*/
		else if (strtolower(method_name) == "register_device")
		{
			string devicename, username, type, ip, port, dir_ip, dir_port, description, os, public_folder, private_folder, is_indexed;
			if (qsp.get_value("devicename", devicename) && qsp.get_value("username", username) 
					&& qsp.get_value("ip", ip) && qsp.get_value("port", port))
			{
				boost::unordered_map<string, string> params;

				if(!isValidName(username))
					return;
				if(!isValidName(devicename))
					return;
				if(!isValidIP(ip))
					return;
				if(!isNumber(port))
					return;

				if(qsp.get_value("type", type))
				{
					if(!type.empty())
						params["type"] = type;
				}
				if(qsp.get_value("dir_ip", dir_ip))
				{
					if(!isValidIP(dir_ip))
						return;
					if(!dir_ip.empty())
						params["dir_ip"] = dir_ip;
				}
				if(qsp.get_value("dir_port", dir_port))
				{
					if(!isNumber(dir_port))
						return;
					if(!dir_port.empty())
						params["dir_port"] = dir_port;
				}
				if(qsp.get_value("ip", ip))
				{
					if(!ip.empty())
						params["ip"] = ip;
				}
				if(qsp.get_value("port", port))
				{
					if(!port.empty())
						params["port"] = port;
				}
				if(qsp.get_value("os", os))
				{
					if(!os.empty())
						params["os"] = os;
				}
				if(qsp.get_value("description", description))
				{
					if(!description.empty())
						params["description"] = description;
				}
				if(qsp.get_value("public_folder", public_folder))
				{
					if(!public_folder.empty())
						params["public_folder"] = public_folder;
				}
				if(qsp.get_value("private_folder", private_folder))
				{
					if(!private_folder.empty())
						params["private_folder"] = private_folder;
				}
				if(qsp.get_value("is_indexed", is_indexed))
				{
					if(!boost::iequals(is_indexed, "true") && !boost::iequals(is_indexed, "false"))
						return;
					params["is_indexed"] = is_indexed;
				}

				http_payload.append(restapi->registerDevice(devicename, username, ip, port, is_indexed, params));
				http_code = reply::ok;
			}
		}
		/*
		else if (strtolower(method_name) == "update_device_ip_port")
		{
			string username, devicename, ip, port;
			if (qsp.get_value("username", username) 
					&& qsp.get_value("devicename", devicename) 
					&& qsp.get_value("ip", ip)
					&& qsp.get_value("port", port))
			{
				if(!isValidIP(ip))
					return;
				if(!isNumber(port))
					return;
				http_payload.append(restapi->updateDeviceIPPort(devicename, username, ip, port));
				http_code = reply::ok;
			}
		}
		*/
		/*
		else if (strtolower(method_name) == "update_device_metadata")
		{
			string username, devicename, metadata;
			if (qsp.get_value("username", username) 
					&& qsp.get_value("devicename", devicename) 
					&& qsp.get_value("metadata", metadata))
			{
				http_payload.append(restapi->updateDeviceMetadata(devicename, username, metadata));
				http_code = reply::ok;
			}
		}
		*/
		else if (strtolower(method_name) == "getall"
				|| strtolower(method_name) == "devicelist")
		{
			string timestamp;
			if (qsp.get_value("timestamp", timestamp))
			{
				if(!isNumber(timestamp)) 
					return;
				http_payload.append(restapi->getDeviceList(timestamp));
				http_code = reply::ok;
			}
		}
		else if (strtolower(method_name) == "getcontentlist")
		{
			string username, devicename;
			if (qsp.get_value("devicename", devicename) && qsp.get_value("username", username))
			{
				http_payload.append("<html>");
				http_payload.append(restapi->getContentList(devicename, username));
				http_payload.append("</html>");
				http_code = reply::ok;
			}
		}
		else if (strtolower(method_name) == "get_homeagent_list")
		{
			http_payload.append(restapi->getHomeAgentList());
			http_code = reply::ok;
		}
	}
}

bool request_handler::url_decode(const std::string& in, std::string& out)
{
	out.clear();
	out.reserve(in.size());
	for (std::size_t i = 0; i < in.size(); ++i)
	{
		if (in[i] == '%')
		{
			if (i + 3 <= in.size())
			{
				int value = 0;
				std::istringstream is(in.substr(i + 1, 2));
				if (is >> std::hex >> value)
				{
					out += static_cast<char>(value);
					i += 2;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else if (in[i] == '+')
		{
			out += ' ';
		}
		else
		{
			out += in[i];
		}
	}
	return true;
}

string& request_handler::strtolower(std::string& str)
{
	for(int i = 0; i < (int)str.length(); i++)
		str[i] = tolower(str[i]);

	return str;
}

bool request_handler::isNumber(const string& input)
{
	for(int i = 0; i < input.size(); ++i)
	{
		if(!isdigit(input.at(i))) 
			return false;
	}
	return true;
}

bool request_handler::isValidName(const string& input)
{
	return (input.find(".") == string::npos);
}

bool request_handler::isValidIP(const string& input)
{
	int oct1, oct2, oct3, oct4;
	int result = sscanf(input.c_str(), "%d.%d.%d.%d", &oct1, &oct2, &oct3, &oct4);

	if(result != 4)
		return false;
	if(oct1 < 0 && oct1 > 255 
		&& oct2 < 0 && oct2 > 255 
		&& oct3 < 0 && oct3 > 255 
		&& oct4 < 0 && oct4 > 255)
		return false;
	return true;
}




