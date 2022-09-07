/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akarafi <akarafi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/04 18:54:42 by atouhami          #+#    #+#             */
/*   Updated: 2022/09/06 22:12:58 by akarafi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>

class Request
{
    private:
        std::string                                         _method;
        std::string                                         _path;
        std::string                                         _version;
        std::vector<std::pair<std::string, std::string> >   _headers;
        std::string                                         _body;
        std::string                                         _raw_request;
        unsigned long                                       _content_length;
        std::string                                         _connection;
        bool                                                _is_body_set;
        bool                                                _is_headers_ended;
        bool                                                _is_request_ended;
        bool                                                _is_chanked;
        
    public:
        Request();
        void                                                parse_first_line(std::stringstream &ss);
        void                                                parse_body(std::stringstream &ss);
        void                                                parse_headers(std::stringstream &ss);
        void                                                parse_request();
        void                                                debug_print() const;
        std::string                                         get_method() const;
        std::string                                         get_path() const;
        std::string                                         get_version() const;
        std::vector<std::pair<std::string, std::string> >   get_headers() const;
        std::string                                         get_body() const;
        std::string                                         get_raw_request() const;
        void                                                update_raw_request(std::string buff);
        std::string                                         get_connection() const;
        std::string                                         get_content_length() const;
        bool                                                is_body_setted() const;
        bool                                                is_headers_ended() const;
        bool                                                is_request_ended() const;
};

#endif
