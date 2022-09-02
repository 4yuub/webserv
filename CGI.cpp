/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akarafi <akarafi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/06 23:23:00 by akarafi           #+#    #+#             */
/*   Updated: 2022/09/02 18:19:16 by akarafi          ###   ########.fr       */
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

void    CGI::_set_env_variables() {
    _env["GATEWAY_INTERFACE"] = "CGI/1.1";
    _env["REDIRECT_STATUS"] = "200";
    _env["CONTENT_TYPE"] = "text/html"; //! GET FROM REQUEST
    _env["SERVER_SOFTWARE"] = "webserv";
    _env["SERVER_NAME"] = "webserv";
    _env["SERVER_PROTOCOL"] = "HTTP/1.1";
    _env["REQUEST_METHOD"] = _request.get_method();
    _env["SCRIPT_FILENAME"] = _file;
    _env["SCRIPT_NAME"] = _cgi_path;
}

char    **CGI::_get_env_array() const {
    char **envp = new char*[_env.size() + 1];
    int i = 0;
    for (map_iterator it = _env.begin(); it != _env.end(); it++, i++) {
        std::string _env_var = it->first + "=" + it->second;
        envp[i] = new char[_env_var.size() + 1];
        strcpy(envp[i], _env_var.c_str());
    }
    envp[i] = NULL;
    return envp;
}

void    CGI::_set_content() {
    int     Ifd[2];
    int     Ofd[2];
    char    *cmd_list[3];

    if (pipe(Ifd) < 0 || pipe(Ofd) < 0) {
        std::cerr << "CGI ERROR" << ": can't open pipe" << std::endl;
        return ;
    }
    cmd_list[0] = const_cast<char *>(_cgi_path.c_str());
    cmd_list[1] = const_cast<char *>(_file.c_str());
    cmd_list[2] = NULL;
    
    _set_env_variables();
    if (_request.get_method() == "GET") {
        //TODO... set QUERY_STRING ( GET_REQUEST )
        // _env["QUERY_STRING"] = _request.get_query();
        _env["CONTENT_LENGHT"] = "0";
    }

    if (_request.get_method() == "POST") {
        //TODO... set CONTENT_LENGHT ( POST_REQUEST )
        _env["CONTENT_LENGHT"] = std::to_string(_request.get_body().length());
    }

    int pid = fork();
    if (pid < 0) {
        std::cerr << "CGI ERROR" << ": can't fork" << std::endl;
        return ;
    }
    if (pid == 0) {
        close(Ifd[1]);
        close(Ofd[0]);
        if (_request.get_method() == "POST") {
            if (dup2(Ifd[0], 0) < 0) {
                std::cerr << "CGI ERROR" << ": fatal" << std::endl; 
                exit(1);
            }
            else
                close(Ifd[0]);
        }

        if (dup2(Ofd[1], 1) < 0) {
            std::cerr << "CGI ERROR" << ": fatal" << std::endl;
            exit(1);
        }

        char **envp = _get_env_array();
        if (execve(cmd_list[0], cmd_list, envp) < 0) {
            std::cerr << "CGI ERROR" << ": can't execute bainary file" << std::endl;
            exit(2);
        }
    }
    else {
        close(Ifd[0]);
        close(Ofd[1]);

        if (_request.get_method() == "POST") {
            if (write(Ifd[1], _request.get_body().c_str(), _request.get_body().length()) < 0) {
                std::cerr << "CGI ERROR" << ": can't write" << std::endl;
                return ;
            }
        }
        close(Ifd[1]);
        char buffer[1024];
        bzero(buffer, 1024);

        do {
            _content += buffer;
            bzero(buffer, 1024);
        } while (read(Ofd[0], buffer, 1024) > 0);
        close(Ofd[0]);
        int status;
        wait(&status);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            _status = 500;
        }
    }
}
