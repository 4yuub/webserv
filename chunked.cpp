/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   chunked.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akarafi <akarafi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/06 20:27:39 by akarafi           #+#    #+#             */
/*   Updated: 2022/09/06 22:17:13 by akarafi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <sstream>
#include <iostream>

std::string decode_body(std::string const &encoded_body) {
    std::string body = "";
    std::stringstream sin(encoded_body);
    std::string line;
    while (std::getline(sin, line)) {
        size_t size;
        try {
            size = std::stoi(line, 0, 16);
        }
        catch (std::exception &e) {
            size = 0;
        }
        if (size == 0) {
            break ;
        }
        std::string buff = "";
        while (buff.size() < size) {
            std::getline(sin, line);
            buff += line + '\n';
        }
        buff.resize(size);
        body += buff;
    }
    return body;
}

std::string encode_body(std::string const &body) {
    std::string encoded_body = "";
    std::stringstream sin(body);
    std::string line;
    while (std::getline(sin, line)) {
        if (line[line.size() - 1] != '\r') line += '\r';
        line += '\n';
        encoded_body += std::to_string(line.size()) + "\r\n";
        encoded_body += line + "\r\n";
    }
    return encoded_body;
}
