/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akarafi <akarafi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/04 18:54:34 by atouhami          #+#    #+#             */
/*   Updated: 2022/09/07 19:31:59 by akarafi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "chunked.hpp"

Request::Request() :    _body(""),
                        _content_length(0),
                        _is_body_set(false),
                        _is_headers_ended(false),
                        _is_request_ended(false),
                        _is_chanked(false) { }

std::string remove_spaces_from_beginning(std::string &str) {
    while (str[0] == ' ') {
        str.erase(0, 1);
    }
    return str;
}

void Request::parse_first_line(std::stringstream &ss)
{
    std::string line;
    std::getline(ss, line);
    std::stringstream first_line_ss(line);
    first_line_ss >> _method;
    first_line_ss >> _path;
    first_line_ss >> _version;
}

void Request::parse_body(std::stringstream &ss)
{
    std::getline(ss, this->_body, '\0');
    if (this->_body.length() > 0) {
        this->_is_body_set = true;
    }
    if (!_is_chanked && this->_body.length() >= this->_content_length) {
        this->_body.resize(this->_content_length);
        this->_is_request_ended = true;
    }
    if (_is_chanked) {
        if (_body.find("0\r\n\r\n")) {
            this->_is_request_ended = true;
            _body = decode_body(_body);
            _content_length = _body.length();
        }
    }
}

void Request::parse_headers(std::stringstream &ss)
{
    std::string line;
    while(std::getline(ss, line))
    {
        if (line.find(":") == std::string::npos) {
            this->_is_headers_ended = true;
            return;
        }
        std::stringstream line_ss(line);
        std::string key;
        std::string value;
        std::getline(line_ss, key, ':');
        std::getline(line_ss, value);
        std::pair<std::string, std::string> header;
        header.first = key;
        header.second = remove_spaces_from_beginning(value);
        if (header.first == "Transfer-Encoding" && header.second == "chunked\r") _is_chanked = true;
        if (header.first == "Content-Length") {
            try {
                this->_content_length = std::stoul(header.second, nullptr, 10);
            } catch (std::exception &ex) {
                std::cerr << "Content-Length is Invalid" << std::endl;
            }
        } else if (header.first == "Connection") {
            this->_connection = header.second;
        }
        _headers.push_back(header);
    }
}

void Request::parse_request()
{
    this->_headers.clear();
    std::stringstream ss(this->_raw_request);
    parse_first_line(ss);
    parse_headers(ss);
    if (this->_is_headers_ended) {
        if (this->_method == "GET" || (this->_content_length == 0 && !_is_chanked)) {
            this->_is_request_ended = true;
        } else { 
            parse_body(ss);
        }
    }
}

void Request::debug_print() const
{
    std::cout << "\033[32m" << "----------------------\033[0m" << std::endl;
    std::cout << "\033[33mMETHOD: \033[0m" << _method << " \033[33mPATH: \033[0m" << _path << "  \033[33mVERSION: \033[0m" << _version << std::endl;
    std::cout << "\033[33mCONTENT_LENGTH: \033[0m" << _content_length << std::endl;
    for (size_t i = 0; i < _headers.size(); i++)
    {
        std::cout << "\033[32mHEADERS |-> \033[34m" << _headers[i].first << "\033[0m:\033[35m" << _headers[i].second << "\033[0m" << std::endl;
    }
    if (_is_body_set)
        std::cout << "\033[31mBODY |-> \033[36m" << _body << "\033[0m" << std::endl;
    std::cout << "\033[33m" << "----------------------\033[0m" << std::endl;
}

std::string                                         Request::get_method() const
{
    return _method;
}

std::string                                         Request::get_path() const
{
    return _path;
}

std::string                                         Request::get_version() const
{
    return _version;
}

std::string                                         Request::get_body() const
{
    return _body;
}

std::string                                         Request::get_raw_request() const
{
    return _raw_request;
}

void                                                Request::update_raw_request(std::string buff)
{
    _raw_request += buff;
}

std::string                                         Request::get_connection() const
{
    return _connection;
}

std::string                                         Request::get_content_length() const
{
    return std::to_string(_content_length);
}

std::vector<std::pair<std::string, std::string> >   Request::get_headers() const
{
    return _headers;
}

bool                                                Request::is_body_setted() const
{
    return _is_body_set;
}

bool                                                Request::is_headers_ended() const
{
    return _is_headers_ended;
}

bool                                                Request::is_request_ended() const
{
    return _is_request_ended;
}