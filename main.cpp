/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zoulhafi <zakariaa@oulhafiane.me>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 11:22:40 by zoulhafi          #+#    #+#             */
/*   Updated: 2022/05/22 15:53:15 by zoulhafi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Config.hpp"

int		main(int argc, char **argv) {
	if (argc > 2) {
		std::cerr << "Usage: ./webserv [configuration file]" << std::endl;
		return 1;
	}

	std::string path = "";
	if (argc == 2)
		path = (std::string)argv[1];
	else
		path = (std::string)"webserv.conf";

	try {
			Config conf(path);

			/* Temporarely Printing Parsed Data */
			std::cout << "====Global====" << std::endl;
			for (string_string_map::const_iterator it=conf.get_global_config().begin(); it!=conf.get_global_config().end(); ++it) {
				std::cout << it->first << " => " << it->second << std::endl;
			}
			std::cout << "====Http====" << std::endl;
			for (string_string_map::const_iterator it=conf.get_http_config().begin(); it!=conf.get_http_config().end(); ++it) {
				std::cout << it->first << " => " << it->second << std::endl;
			}
			int count = 0;
			for (std::vector<string_string_map>::const_iterator it=conf.get_servers_config().begin(); it!=conf.get_servers_config().end(); ++it) {
				std::cout << "\t====Server-" << count << "====" << std::endl;
				for (string_string_map::const_iterator it2=(*it).begin(); it2!=(*it).end(); ++it2) {
					std::cout << "\t" << it2->first << " => " << it2->second << std::endl;
				}
				std::pair <std::multimap<std::string, string_string_map>::const_iterator, std::multimap<std::string, string_string_map>::const_iterator> ret;
				ret = conf.get_locations_config().equal_range("server_"+std::to_string(count));
				int count_loc = 0;
				for (std::multimap<std::string, string_string_map>::const_iterator it3=ret.first; it3!=ret.second; ++it3) {
					std::cout << "\t\t====Location-" << count_loc << "====" << std::endl;
					for (string_string_map::const_iterator it4=it3->second.begin(); it4!=it3->second.end(); ++it4) {
						std::cout << "\t\t" << it4->first << " => " << it4->second << std::endl;
					}
					count_loc++;
				}
				count++;
			}
			/* ================ */

	} catch (std::exception &ex) {
		std::cerr << ex.what() << std::endl;
	}
	return 0;
}
