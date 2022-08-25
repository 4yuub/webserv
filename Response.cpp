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
	std::string method = this->_request.get_method();
	_directory_listing = false;
	try {
		std::string allowed_methods = _location.at("allowed_methods");
		if (allowed_methods.find(method) == std::string::npos) {
			this->_status_code = 405;
			return;
		}
	}
	catch (std::exception &e) {
		std::string allowed_methods = _vserver->get_allowed_methods();
		if (allowed_methods == "")
			allowed_methods = "GET, POST, PATCH, PUT, DELETE";
		if (allowed_methods.find(method) == std::string::npos) {
			this->_status_code = 405;
			return;
		}
	}
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
			{
				if (isDirectory(path.c_str()))
					{
						try
						{
							std::string directory_listing = _location.at("directory_listing");
							if (directory_listing != "on")
								_status_code = 403;
							else
							{
								_status_code = 200;
								this->_directory_listing = true;
							}
						}
						catch (std::exception &e)
						{
							std::string directory_listing = _vserver->get_directory_listing();
							if (directory_listing != "on")
								_status_code = 403;
							else
							{
								_status_code = 200;
								this->_directory_listing = true;
							}
						}
					}
				else
					_status_code = 403;
			}
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

std::string Response::get_html_of_directory_listing(std::string const &path)
{
	DIR *dir;
	struct dirent *ent;
	std::string html = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>Index of " + path + "</title></head><body><h1>Index of " + _request.get_path() + "</h1><hr><pre>";
	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if (ent->d_name[0] != '.')
			{
				std::string file_path = path + "/" + ent->d_name;
				if (isDirectory(file_path.c_str()))
					html += "<a href=\"" + _request.get_path() + "/" + ent->d_name + "/\">" + ent->d_name + "/</a><br>";
				else
					html += "<a href=\"" + _request.get_path() + "/" + ent->d_name + "\">" + ent->d_name + "</a>\n";
			}
		}
		closedir(dir);
	}
	html += "</pre></body></html>";
	return html;
}

std::string Response::get_content_of_path(std::string path, std::map<std::string, std::string> const &location)
{
	std::string content;
	if (_status_code == 404)
		return "<h1>404 Not found</h1>";
	else if (_status_code == 403)
		return "<h1>403 Forbidden</h1>";
	else if (_status_code == 405)
		return "<h1>405 Method Not Allowed</h1>";
	else if (_directory_listing == true)
		return get_html_of_directory_listing(path);
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
	if (_status_code != 200 && _status_code != 301)
		content = _error_pages[_status_code];
	_response = "HTTP/1.1 " + std::to_string(_status_code) + " " + _response_message[_status_code] + "\r\n";
	_response += "Content-Type: text/html\r\n";
	_response += "Content-Length: " + std::to_string(content.size()) + "\r\n";
	if (_status_code == 301)
		_response += "Location: " + _location + "\r\n";
	if (this->_request.get_connection() == "close")
		_response += "Connection: close\r\n";
	_response += "\r\n";
	_response += content;
}

void Response::handle_response(Request &request)
{
	std::string content = "";
	std::string path = request.get_path();
	std::string location = _vserver->location_match(path);
	std::string root;

	if (location == "none") {
		_status_code = 404;
		content = "<h1>404 Not found</h1>";
		format_response(content);
		return ;
	}
	std::map<std::string, std::string> const & \
		location_map = _vserver->get_locations().at(location);
	_status_code = 200;
	location = location_map.at("location");
	if (location_map.find("redirect") != location_map.end())
	{
		_status_code = 301;
		_location = location_map.at("redirect") + path.erase(0, location.length() - 1);
		content = "<h1>301 Moved Permanently</h1>";
		format_response(content);
		return ;
	}

	if (location_map.find("upload_pass") != location_map.end())
	{
		std::string upload_path = location_map.at("upload_pass");
		upload_path += path.erase(0, location.length() - 2);
		if (_request.get_method() == "GET")
		{
			std::ifstream file(upload_path);
			std::getline(file, content, '\0');
			file.close();
			format_response(content);
			return ;
		}
		std::ofstream file(upload_path);
		file << request.get_body();
		if (!file.good())
		{
			_status_code = 500;
			content = "<h1>500 Internal Server Error</h1>";
			format_response(content);
			return ;
		}
		file.close();
		content = "<h1>Upload Success</h1>";
		format_response(content);
		return ;
	}

	if (location_map.find("root") != location_map.end())
		root = location_map.at("root");
	else
		root = _vserver->get_root();
	path = root + path.erase(0, location.length() - 2);
	set_status_code(path, location_map);
	content = get_content_of_path(path, location_map);
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

Response::Response(Request &request, std::vector<VirtualServer> const &vservers, std::map<int, std::string> &error_pages)
	: _request(request), _vservers(vservers), _error_pages(error_pages)
{
	(void) _error_pages;
	match_virtual_server();
	init_response_code_message();
	handle_response(request);
}



std::string Response::operator*() const
{
	return this->_response;
}