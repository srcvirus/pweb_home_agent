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
	std::string extension = "html";

	// Fill out the reply to be sent to the client.
	//rep.status = reply::ok;
	this->build_response(qsp, rep.content, rep.status);
	rep.headers.resize(2);
	rep.headers[0].name = "Content-Length";
	rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
	rep.headers[1].name = "Content-Type";
	rep.headers[1].value = mime_types::extension_to_type(extension);
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
			else
			{
				http_code = reply::bad_request;
			}
		}
		/*else if (strtolower(method_name) == "is_username_availabe")
		{
			string name, password, email, fullname, location, affiliation;
			if (qsp.get_value("name", name)
					&& qsp.get_value("password", password)
					&& qsp.get_value("email", email)
					&& qsp.get_value("fullname", fullname)
					&& qsp.get_value("location", location)
					&& qsp.get_value("affiliation", affiliation))
			{
				http_payload.append(
						registerUser(name, password, email, fullname, location,
								affiliation));
				http_code = "200 OK";
			}
			else
			{
				http_code = "451 Parameter Not Understood";
			}
		}
		else if (strtoupper(method_name) == "AUTHENTICATE")
		{
			string name, password;
			if (qsp.get_value("name", name)
					&& qsp.get_value("password", password))
			{
				http_payload.append(authenticate(name, password));
				http_code = "200 OK";
			}
			else
			{
				http_code = "451 Parameter Not Understood";
			}
		}
		else if (strtoupper(method_name) == "GETDEVICELIST")
		{
			string username;
			if (qsp.get_value("username", username))
			{
				http_payload.append(getDeviceList(username));
				http_code = "200 OK";
			}
			else
			{
				http_code = "451 Parameter Not Understood";
			}
		}
		else if (strtoupper(method_name) == "ISAVAILABLE")
		{
			string username, devicename;
			if (qsp.get_value("devicename", devicename)
					&& qsp.get_value("username", username))
			{
				//int dot_index = name.find_last_of('.');
				//devicename = name.substr(0, dot_index);
				//username = name.substr(dot_index+1, string::npos);
				http_payload.append(existDevicename(username, devicename));
				http_code = "200 OK";
			}
			else
			{
				http_code = "451 Parameter Not Understood";
			}
		}
		else if (strtoupper(method_name) == "MODIFYDEVICE")
		{
			string username, devicename, newdevicename, ip, port, public_folder,
					private_folder;
			if (qsp.get_value("username", username)
					&& qsp.get_value("devicename", devicename)
					&& qsp.get_value("newdevicename", newdevicename)
					&& qsp.get_value("ip", ip) && qsp.get_value("port", port)
					&& qsp.get_value("public_folder", public_folder)
					&& qsp.get_value("private_folder", private_folder))
			{
				//int dot_index = name.find_last_of('.');
				//devicename = name.substr(0, dot_index);
				//username = name.substr(dot_index+1, string::npos);
				http_payload.append(
						modifyDevice(username, devicename, newdevicename, ip,
								port, public_folder, private_folder));
				http_code = "200 OK";
			}
			else
			{
				http_code = "451 Parameter Not Understood";
			}
		}
		else if (strtoupper(method_name) == "DELETEDEVICE")
		{
			string name, username, devicename;
			if (qsp.get_value("name", name))
			{
				int dot_index = name.find_last_of('.');
				devicename = name.substr(0, dot_index);
				username = name.substr(dot_index + 1, string::npos);
				http_payload.append(deleteDevice(username, devicename));
				http_code = "200 OK";
			}
			else
			{
				http_code = "451 Parameter Not Understood";
			}
		}
		else if (strtoupper(method_name) == "REGISTER")
		{
			string name, username, devicename, type, ip, port, public_folder,
					private_folder, os, description, is_publicly_indexed;
			if (qsp.get_value("name", name) && qsp.get_value("type", type)
					&& qsp.get_value("ip", ip) && qsp.get_value("port", port)
					&& qsp.get_value("public_folder", public_folder)
					&& qsp.get_value("private_folder", private_folder)
					&& qsp.get_value("os", os)
					&& qsp.get_value("description", description)
					&& qsp.get_value("ispublicly_indexed", is_publicly_indexed))
			{
				int dot_index = name.find_last_of('.');
				devicename = name.substr(0, dot_index);
				username = name.substr(dot_index + 1, string::npos);
				bool searchable = true;
				if (is_publicly_indexed == "no") searchable = false;
				http_payload.append(
						registerDevice(username, devicename, type, ip, port,
								public_folder, private_folder, os, description,
								searchable));
				http_code = "200 OK";
			}
			else
			{
				http_code = "451 Parameter Not Understood";
			}
		}
		else if (strtoupper(method_name) == "UPDATE")
		{
			string name, username, devicename, ip, port;
			if (qsp.get_value("name", name) && qsp.get_value("ip", ip)
					&& qsp.get_value("port", port))
			{
				int dot_index = name.find_last_of('.');
				devicename = name.substr(0, dot_index);
				username = name.substr(dot_index + 1, string::npos);
				http_payload.append(
						updateDevice(username, devicename, ip, port));
				http_code = "200 OK";
			}
			else
			{
				http_code = "451 Parameter Not Understood";
			}
		}
		else if (strtoupper(method_name) == "UPDATEMETA")
		{
			string name, username, devicename, data;
			if (qsp.get_value("name", name) && qsp.get_value("data", data))
			{
				int dot_index = name.find_last_of('.');
				devicename = name.substr(0, dot_index);
				username = name.substr(dot_index + 1, string::npos);
				http_payload.append(updateMeta(username, devicename, data));
				http_code = "200 OK";
			}
			else
			{
				http_code = "451 Parameter Not Understood";
			}
		}
		else if (strtoupper(method_name) == "GETALL"
				|| strtoupper(method_name) == "DEVICELIST")
		{
			string timestamp, neighbours;
			if (qsp.get_value("timestamp", timestamp))
			{
				string result = "<getall><name>" + this_peer->get_peer_name()
						+ "</name>";
				routingTable2XML(*this_peer->getProtocol()->getRoutingTable(),
						neighbours);
				result.append(neighbours);
				result.append(getall(timestamp));
				result.append("</getall>");
				http_payload.append(result);
				http_code = "200 OK";
			}
			else
			{
				http_code = "451 Parameter Not Understood";
			}
		}
		else if (strtoupper(method_name) == "GETCONTENTLIST")
		{
			return_as_html = true;
			string name, username, devicename;
			if (qsp.get_value("name", name))
			{
				int dot_index = name.find_last_of('.');
				devicename = name.substr(0, dot_index);
				username = name.substr(dot_index + 1, string::npos);
				http_payload.append("<html>");
				http_payload.append(getContentList(username, devicename));
				http_payload.append("</html>");
				http_code = "200 OK";
			}
			else
			{
				http_code = "451 Parameter Not Understood";
			}
		}*/
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

