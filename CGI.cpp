/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akarafi <akarafi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/06 23:23:00 by akarafi           #+#    #+#             */
/*   Updated: 2022/09/19 14:13:34 by akarafi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI(Request const &_req, std::string const &_file_path, std::string const &_fast_cgi_path) 
    : _request(_req) , _cgi_path(_fast_cgi_path), _file(_file_path)
{
    _content = "";
    _status = 200;
    _set_content();
}

CGI::~CGI() {

}

int CGI::_get_status() const {
    return _status;
}

std::string CGI::_get_content() const {
    return _content;
}

std::string CGI::_get_header(std::string const &header_name) const {
    vector_string_string headers = _request.get_headers();
    vector_string_string::iterator it;
    for (it = headers.begin(); it < headers.end(); ++it) {
        if (it->first == header_name) {
			std::string header_value = it->second;
			header_value.resize(header_value.length() - 1);
			return header_value;
		}
    }
    return "";
}

void    CGI::_set_env_variables() {
    _env["GATEWAY_INTERFACE"] = "CGI/1.1";
    _env["REDIRECT_STATUS"] = "true";
    _env["CONTENT_TYPE"] = _request.get_method() == "POST" ?
                "application/x-www-form-urlencoded" : "text/html";
    _env["SERVER_SOFTWARE"] = "webserv";
    _env["SERVER_NAME"] = "webserv";
    _env["SERVER_PROTOCOL"] = "HTTP/1.1";
    _env["REQUEST_METHOD"] = _request.get_method();
    _env["SCRIPT_FILENAME"] = _file;
    _env["SCRIPT_NAME"] = _file.substr(_file.rfind('/')+1);
    _env["HTTP_COOKIE"] = _get_header("Cookie");
    _env["CONTENT_LENGTH"] = _request.get_content_length();
    if (_request.get_method() != "POST")
        _env["CONTENT_LENGTH"] = "0";
    _env["QUERY_STRING"] = _request.get_query_string();
}

char    **CGI::_get_env_array() const {
    char **envp = new char*[_env.size() + 1];
    int i = 0;
    for (map_iterator it = _env.begin(); it != _env.end(); it++, i++) {
        std::string _env_var = it->first + "=" + it->second;
        envp[i] = new char[_env_var.size() + 1];
        strlcpy(envp[i], _env_var.c_str(), _env_var.length()+1);
    }
    envp[i] = NULL;
    return envp;
}

void    CGI::_set_content() {
    int     postFd[2];
    int     readFd[2];
    int     status;
    char    *cmd_list[3];

    _set_env_variables();
    if (pipe(postFd) < 0 || pipe(readFd) < 0) {
        std::cerr << "CGI ERROR" << ": can't open pipe" << std::endl;
        return ;
    }
    cmd_list[0] = const_cast<char *>(_cgi_path.c_str());
    cmd_list[1] = const_cast<char *>(_file.c_str());
    cmd_list[2] = NULL;
    int pid = fork();
    if (pid == 0) {
        if (_request.get_method() == "POST") {
            if (dup2(postFd[0], 0) < 0) {
                std::cerr << "GCI ERROR" << ": Fatal" << std::endl;
                exit(1);
            }
        }
        if (dup2(readFd[1], 1) < 0) {
            std::cerr << "GCI ERROR" << ": Fatal" << std::endl;
            exit(1);
        }
        close(readFd[0]);
        close(readFd[1]);
        close(postFd[0]);
        close(postFd[1]);
        char **envp = _get_env_array();
        execve(cmd_list[0], cmd_list, envp);
        exit(1);
    }
    else {
        close(readFd[1]);
        if (_request.get_method() == "POST") {
            write(postFd[1], _request.get_body().c_str(), _request.get_body().size());
        }
        char buff[1024];
        bzero(buff, 1024);
        do {
            _content += buff;
            bzero(buff, 1024);
        }
        while(read(readFd[0], buff, 1024) > 0);
        close(postFd[1]);
        close(readFd[0]);
        close(postFd[0]);
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            _status = 500;
    }
}
