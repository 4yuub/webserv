#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "Request.hpp"
#include "fstream"
#include <unistd.h>
#include <sys/stat.h>


class Response
{
	private:
		std::string						_response;
		int								_status_code;
		std::map<int, std::string>		_response_message;
	public:
		void init_response_code_message();
	  	void set_status_code(std::string &path);
		std::string get_content_of_path(std::string path) const;	
		void format_response(std::string content);
		void handle_response(Request &request);
		Response(Request &request);	
		std::string		operator*() const;
};

#endif