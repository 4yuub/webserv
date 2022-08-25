#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "Request.hpp"
#include "VirtualServer.hpp"
#include "fstream"
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>


class Response
{
	private:
		std::string						 	_response;
		Request							 	&_request;
		int									_status_code;
		std::map<int, std::string>			_response_message;
		std::vector<VirtualServer> const	&_vservers;
		VirtualServer const 				*_vserver;
		std::string							_location;
		std::map<int, std::string>			&_error_pages;
		bool								_directory_listing;
	public:
		void init_response_code_message();
		std::string get_html_of_directory_listing(std::string const &path);
	  	void set_status_code(std::string &path, std::map<std::string, std::string> const &_location);
		std::string get_content_of_path(std::string path, std::map<std::string, std::string> const &location);	
		void format_response(std::string content);
		void handle_response(Request &request);
		Response(Request &request, std::vector<VirtualServer> const &vservers, std::map<int, std::string> &error_pages);
		std::string		operator*() const;
	private:
		void match_virtual_server();
};

#endif