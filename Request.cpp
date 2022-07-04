/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atouhami <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/04 18:54:34 by atouhami          #+#    #+#             */
/*   Updated: 2022/07/04 18:54:35 by atouhami         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

std::string remove_spaces_from_beginning(std::string &str) {
    while (str[0] == ' ') {
        str.erase(0, 1);
    }
    return str;
}

void Request::init(std::string raw_request)
{
    is_body_set = 0;
    _body = "";
    _raw_request = raw_request;
}

void Request::parse_first_line(std::string first_line)
{
    std::stringstream first_line_ss(first_line);
    first_line_ss >> _method;
    first_line_ss >> _path;
    first_line_ss >> _version;
}

void Request::parse_body(std::stringstream &ss)
{
    std::string line;
    while (std::getline(ss, line))
    {
        is_body_set = 1;
        _body += line + "\n";
    }
}

void Request::parse_headers(std::stringstream &ss)
{
    std::string line;
    while(std::getline(ss, line))
    {
        if (line.find(":") == std::string::npos)
            return;
        std::stringstream line_ss(line);
        std::string key;
        std::string value;
        std::getline(line_ss, key, ':');
        std::getline(line_ss, value);
        std::pair<std::string, std::string> header;
        header.first = key;
        header.second = remove_spaces_from_beginning(value);
        _headers.push_back(header);
    }
}

void Request::parse_request(std::string request_string)
{
    std::stringstream ss(request_string);
    std::string line;
    std::getline(ss, line);
    parse_first_line(line);
    parse_headers(ss);
    parse_body(ss);
}

void Request::debug_print()
{
    std::cout << "\033[32m" << "----------------------\033[0m" << std::endl;
    std::cout << "\033[33mMETHOD: \033[0m" << _method << " \033[33mPATH: \033[0m" << _path << "  \033[33mVERSION: \033[0m" << _version << std::endl;
    for (size_t i = 0; i < _headers.size(); i++)
    {
        std::cout << "\033[32mHEADERS |-> \033[34m" << _headers[i].first << "\033[0m:\033[35m" << _headers[i].second << "\033[0m" << std::endl;
    }
    if (is_body_set)
        std::cout << "\033[31mBODY |-> \033[36m" << _body << "\033[0m" << std::endl;
    std::cout << "\033[33m" << "----------------------\033[0m" << std::endl;
}

Request::Request(std::string request_string)
{
    init(request_string);
    parse_request(request_string);
}
