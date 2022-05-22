/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zoulhafi <zakariaa@oulhafiane.me>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 12:13:32 by zoulhafi          #+#    #+#             */
/*   Updated: 2022/05/22 15:54:47 by zoulhafi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config(std::string path) {
	std::ifstream		conf_file(path);
	std::smatch			base_match;
	if (!std::regex_match((std::string)path, base_match, std::regex(pattern_conf_filename))) {
		throw std::runtime_error("Config File Name Must have '.conf' at the end.");
	}
	if (conf_file.fail() || conf_file.bad()) {
		throw std::runtime_error("No Config File Found.");
	}
	this->_parse_config(conf_file);
}

Config::~Config() {
}

const string_string_map					&Config::get_global_config() const {
	return this->_global_config;
}

const string_string_map					&Config::get_http_config() const {
	return this->_http_config;
}

const std::vector<string_string_map>	&Config::get_servers_config() const {
	return this->_servers_config;
}

const string_map_multimap				&Config::get_locations_config() const {
	return this->_locations_config;
}

void Config::_parse_config(std::ifstream &conf_file) {
	std::smatch			base_match;
	std::string			line;
	std::string			tmp_key = "";
	std::string			tmp_value = "";
	string_string_map	server_tmp;
	string_string_map	location_tmp;
	int					deep_level = 0;
	int					flags = 0;

	while (std::getline(conf_file, line)) {
		line = std::regex_replace(line, std::regex(pattern_comments), "");
		if (std::regex_match(line, base_match, std::regex(pattern_begin_accolade))) {
			if (base_match[1] == "http" && deep_level == 0) {
				flags |= mask_http;
			} else if (base_match[1] == "server" && deep_level == 1 && (flags & mask_http) == mask_http) {
				flags |= mask_server;
				server_tmp.clear();
			} else if (base_match[1] == "location" && deep_level == 2 && (flags & mask_server) == mask_server) {
				flags |= mask_location;
				location_tmp.clear();
				location_tmp.insert(std::pair<std::string, std::string>("location", base_match[2]));
			}
			deep_level++;
		} else if (std::regex_match(line, base_match, std::regex(pattern_end_accolade))) {
			if (deep_level == 3 && (flags & mask_location) == mask_location) {
				flags ^= mask_location;
				_locations_config.insert(std::pair<std::string, string_string_map>("server_"+std::to_string(this->_servers_config.size()), location_tmp));
			} else if (deep_level == 2 && (flags & mask_server) == mask_server) {
				flags ^= mask_server;
				_servers_config.push_back(server_tmp);
			} else if (deep_level == 1 && (flags & mask_http) == mask_http) {
				flags ^= mask_http;
			}
			deep_level--;
			if (deep_level < 0) {
				conf_file.close();
				throw std::runtime_error("Syntax of Config Not good, maybe you have some extra '}'.");
			}
		} else if (std::regex_match(line, base_match, std::regex(pattern_key_value))) {
			std::string key = base_match[1];
			std::string value = base_match[2];
			if (tmp_key != "") {
				key = tmp_key;
				value = tmp_value + (std::string)base_match[0];
				tmp_key = "";
				tmp_value = "";
			}
			if (deep_level == 3 && (flags & mask_location) == mask_location) {
				location_tmp.insert(std::pair<std::string, std::string>(key, value));
			} else if (deep_level == 2 && (flags & mask_server) == mask_server) {
				server_tmp.insert(std::pair<std::string, std::string>(key, value));
			} else if (deep_level == 1 && (flags & mask_http) == mask_http) {
				this->_http_config.insert(std::pair<std::string, std::string>(key, value));
			} else if (deep_level == 0) {
				this->_global_config.insert(std::pair<std::string, std::string>(key, value));
			}
		} else if (std::regex_match(line, base_match, std::regex(pattern_key_value_not_ended))) {
			if (tmp_key == "") {
				tmp_key = base_match[1];
				tmp_value = base_match[2];
			} else {
				tmp_value += base_match[0];
			}
		}
	}
	conf_file.close();
	if (deep_level != 0) {
		throw std::runtime_error("Syntax of Config Not good, maybe you have missed a '}'.");
	}
}
