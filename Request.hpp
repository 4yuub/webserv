/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atouhami <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/04 18:54:42 by atouhami          #+#    #+#             */
/*   Updated: 2022/07/04 18:54:43 by atouhami         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>

class Request
{
    private:
        std::string _method;
        std::string _path;
        std::string _version;
        std::vector<std::pair<std::string, std::string> > _headers;
        std::vector<std::string> _body;
        std::string _raw_request;
        bool is_body_set;
        
    public:
        void init(std::string raw_request);

        void parse_first_line(std::string first_line);

        void parse_body(std::stringstream &ss);

        void parse_headers(std::stringstream &ss);

        void parse_request(std::string request_string);

        void debug_print();
        
        Request(std::string request_string);
};

#endif
