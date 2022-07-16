/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akarafi <akarafi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/23 16:18:22 by zoulhafi          #+#    #+#             */
/*   Updated: 2022/07/15 00:35:30 by akarafi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VirtualServer.hpp"

VirtualServer::VirtualServer(const string_string_map &server_config, const string_string_range_multimap &locations, const string_string_map &http_config) {
	string_string_map::const_iterator	it;
	std::smatch							base_match;

	it = server_config.find("listen");
	if (it != server_config.end()) {
		if (std::regex_match(it->second, base_match, std::regex(pattern_host_port))) {
			this->_host = base_match[1];
			this->_port = std::stoi(base_match[2]);
		} else if (std::regex_match(it->second, base_match, std::regex(pattern_only_ip))) {
			this->_host = base_match[1];
			this->_port = 8080;
		} else if (std::regex_match(it->second, base_match, std::regex(pattern_only_port))) {
			this->_host = "127.0.0.1";
			this->_port = std::stoi(base_match[1]);
		} else {
			throw std::runtime_error("Invalid Syntax => " + it->first + ": " + it->second + ".");
		}
	} else {
		this->_host = "127.0.0.1";
		this->_port = 8080;
	}

	it = server_config.find("server_name");
	if (it != server_config.end()) {
		std::stringstream	tmp_stream(it->second);
		std::string			name_server;

		while (std::getline(tmp_stream, name_server, ' ')) {
			this->_server_names.push_back(name_server);
		}
	} else {
		this->_server_names.push_back("");
	}

	it = server_config.find("root");
	if (it != server_config.end()) {
		this->_root = it->second;
	} else {
		it = http_config.find("root");
		if (it != http_config.end())
			this->_root = it->second;
		else
			this->_root = "/var/www/html";
	}

	for (string_map_multimap::const_iterator it=locations.first; it!=locations.second; ++it) {
		std::string							location;
		string_string_map					tmp_location_rules;
		string_string_map::const_iterator	it2 = it->second.find("location");

		if (it2 != it->second.end()) {
			if (std::regex_match(it2->second, base_match, std::regex(pattern_location_modifier))) {
				if (base_match[1] == "=") {
					location = base_match[2];
				} else {
					location = base_match[2];
					location += ".*";
				}
			} else {
				throw std::runtime_error("Invalid Syntax of location: " + it2->second);
			}
		} else {
			location = "";
		}
		for (it2=it->second.begin(); it2!=it->second.end(); ++it2) {
			tmp_location_rules.insert(std::pair<std::string, std::string>(it2->first, it2->second));
		}
		this->_locations.insert(std::pair<std::string, string_string_map>(location, tmp_location_rules));
	}
}

VirtualServer::~VirtualServer() {

}

const std::string					&VirtualServer::get_host() const {
	return this->_host;
}

int									VirtualServer::get_port() const {
	return this->_port;
}

const std::string					&VirtualServer::get_root() const {
	return this->_root;
}

const std::vector<std::string>		&VirtualServer::get_server_names() const {
	return this->_server_names;
}

bool								VirtualServer::location_match(const std::string &location) const {
	std::smatch													base_match;
	std::map<std::string, string_string_map>::const_iterator	it;

	for (it = this->_locations.begin(); it != this->_locations.end(); ++it) {
		if (std::regex_match(location, base_match, std::regex(it->first))) {
			return true;
		}
	}
	return false;
}
