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

void Response::set_status_code(std::string &path)
{
	if (access(path.c_str(), F_OK) == -1)
		_status_code = 404;
	else if (access(path.c_str(), R_OK) == -1 || isDirectory(path.c_str()))
		_status_code = 403;
	else
		_status_code = 200;
}

std::string Response::get_content_of_path(std::string path) const
{
	if (_status_code == 404)
		return "<h1>404 Not found</h1>";
	else if (_status_code == 403)
		return "<h1>403 Forbidden</h1>";
	else
	{
		// std::ifstream file(path);
		// std::string content;
		// std::getline(file, content, '\0');
		// file.close();
		std::string content = CGI(_request, path, "/usr/bin/php")._get_content();
		return content;
	}
}

void Response::format_response(std::string content)
{
	_response = "HTTP/1.1 " + std::to_string(_status_code) + " " + _response_message[_status_code] + "\r\n";
	_response += "Content-Type: text/html\r\n";
	_response += "Content-Length: " + std::to_string(content.size()) + "\r\n";
	_response += "Connection: close\r\n";
	_response += "\r\n";
	_response += content;
}

void Response::handle_response(Request &request)
{
	std::string content;
	std::string path = request.get_path();
	if (path == "/")
		path = "index.php";
	else
		path.erase(0, 1);
	set_status_code(path);
	content = get_content_of_path(path);
	format_response(content);
}

Response::Response(Request &request) : _request(request)
{
	init_response_code_message();
	handle_response(request);
}

std::string Response::operator*() const
{
	return this->_response;
}