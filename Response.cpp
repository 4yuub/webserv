#include "Response.hpp"
#include "CGI.hpp"

int isDirectory(const char *path)
{
	struct stat statbuf;
	if (stat(path, &statbuf) != 0)
		return 0;
	return S_ISDIR(statbuf.st_mode);
}

void Response::init_response_code_message()
{
	_response_message[200] = "OK";
	_response_message[403] = "Forbidden";
	_response_message[404] = "Not Found";
	_response_message[405] = "Method Not Allowed";
	_response_message[500] = "Internal Server Error";
	_response_message[502] = "Bad Gateway";		
}

void Response::set_status_code(std::string &path, std::map<std::string, std::string> const &_location)
{
	if (access(path.c_str(), F_OK) == -1)
		_status_code = 404;
	else if (access(path.c_str(), R_OK) == -1 || isDirectory(path.c_str()))
	{
		std::string index;
		try {
			index = _location.at("index");
		}
		catch (std::exception &e) {
			index = _vserver->get_index();
			if (index == "")
				_status_code = 403;
			else
			{
				path += index;
				if (access(path.c_str(), F_OK) == -1)
					_status_code = 404;
				else if (access(path.c_str(), R_OK) == -1)
					_status_code = 403;
				else
					_status_code = 200;
			}
		}
	}	
	else
		_status_code = 200;
}

std::string Response::get_content_of_path(std::string path, std::map<std::string, std::string> const &location)
{
	std::string content;
	if (_status_code == 404)
		return "<h1>404 Not found</h1>";
	else if (_status_code == 403)
		return "<h1>403 Forbidden</h1>";	
	try {
		std::string cgi_path = location.at("fastcgi_pass");
		CGI gci(_request, path, cgi_path);
		if (gci._get_status() == 500) {
			_status_code = 500;
			return "<h1>500 Internal Server Error</h1>";
		}
		content = gci._get_content();
	}
	catch (std::exception &e) {
		std::ifstream file(path);
		std::getline(file, content, '\0');
		file.close();
	}
	return content;
}

void Response::format_response(std::string content)
{
	_response = "HTTP/1.1 " + std::to_string(_status_code) + " " + _response_message[_status_code] + "\r\n";
	_response += "Content-Type: text/html\r\n";
	_response += "Content-Length: " + std::to_string(content.size()) + "\r\n";
	if (this->_request.get_connection() == "close")
		_response += "Connection: close\r\n";
	_response += "\r\n";
	_response += content;
}

void Response::handle_response(Request &request)
{
	std::string content;
	std::string path = request.get_path();
	std::string location = _vserver->location_match(path);
	std::string root;
	std::string index;

	if (location == "none") {
		_status_code = 404;
		content = "<h1>404 Not found</h1>";
	}
	else {
		_status_code = 200;
		std::map<std::string, std::string> const &_location = _vserver->get_locations().at(location);
		location =  _location.at("location");
		try {
			root = _location.at("root");
		}
		catch (std::exception &e) {
			root = _vserver->get_root();
		}
		path = root + path.erase(0, location.length() - 2);
		set_status_code(path, _location);
		content = get_content_of_path(path, _location);
	}
	format_response(content);
}

std::string get_host(Request &request)
{
	std::vector<std::pair<std::string, std::string> > const &heades = request.get_headers();
	std::vector<std::pair<std::string, std::string> >::const_iterator it;
	for (it = heades.begin(); it != heades.end(); ++it)
	{
		if (it->first == "Host") {
			std::string host = it->second;
			host.resize(host.length() - 1);
			return host;
		}
	}
	return "none";
}

void Response::match_virtual_server() {
	std::vector<VirtualServer>::const_iterator it;
	std::vector<std::string>::iterator it2;
	std::string	host = get_host(_request);
	for (it = _vservers.begin(); it < _vservers.end(); it++)
	{
		std::vector<std::string> server_names = it->get_server_names();
		for (it2 = server_names.begin(); it2 < server_names.end(); it2++)
		{
			if (*it2 == host)
			{
				_vserver = &(*it);
				return;
			}
		}
	}
	_vserver = &(*_vservers.begin());
}

Response::Response(Request &request, std::vector<VirtualServer> const &vservers)
	: _request(request), _vservers(vservers)
{
	match_virtual_server();
	init_response_code_message();
	handle_response(request);
}



std::string Response::operator*() const
{
	return this->_response;
}