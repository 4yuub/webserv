/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zoulhafi <zakariaa@oulhafiane.me>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 12:13:50 by zoulhafi          #+#    #+#             */
/*   Updated: 2022/05/24 18:17:21 by zoulhafi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <map>
#include <vector>

typedef std::map<std::string, std::string>				string_string_map;
typedef std::multimap<std::string, string_string_map>	string_map_multimap;
const std::string										pattern_conf_filename = "^.*\\.conf$";
const std::string										pattern_comments = "#.*$";
const std::string										pattern_begin_accolade = "^\\s*(\\S*)\\s*([^\\{]*)\\{.*";
const std::string										pattern_end_accolade = "^\\s*[^\\}]*\\}.*";
const std::string										pattern_key_value = "^\\s*(\\S*)\\s*(.*);\\s*$";
const std::string										pattern_key_value_not_ended = "^\\s*(\\S*)\\s*(.*)";
const char												mask_http = 1;
const char												mask_server = 1 << 1;
const char												mask_location = 1 << 2;

class Config
{
	private:
		string_string_map								_global_config;
		string_string_map								_http_config;
		std::vector<string_string_map>					_servers_config;
		string_map_multimap								_locations_config;
		void											_parse_config(std::ifstream &);

	public:
		Config(std::string &path);
		~Config();
		const string_string_map							&get_global_config() const;
		const string_string_map							&get_http_config() const;
		const std::vector<string_string_map>			&get_servers_config() const;
		const string_map_multimap						&get_locations_config() const;
};

#endif
