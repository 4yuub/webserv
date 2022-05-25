/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zoulhafi <zakariaa@oulhafiane.me>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 11:22:40 by zoulhafi          #+#    #+#             */
/*   Updated: 2022/05/25 14:15:31 by zoulhafi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Config.hpp"
#include "Server.hpp"

int		main(int argc, char **argv) {
	if (argc > 2) {
		std::cerr << "Usage: ./webserv [configuration file]" << std::endl;
		return 1;
	}

	std::string path = "";
	if (argc == 2)
		path = std::string(argv[1]);
	else
		path = std::string("webserv.conf");

	try {
			Config conf(path);

			/* Temporarely Printing Parsed Data 
			std::cout << "====Global====" << std::endl;
			for (string_string_map::const_iterator it=conf.get_global_config().begin(); it!=conf.get_global_config().end(); ++it) {
				std::cout << it->first << " => " << it->second << std::endl;
			}
			std::cout << "====Http====" << std::endl;
			for (string_string_map::const_iterator it=conf.get_http_config().begin(); it!=conf.get_http_config().end(); ++it) {
				std::cout << it->first << " => " << it->second << std::endl;
			}
			 ================ */

			Server server(conf);
			server.start();
	} catch (std::exception &ex) {
		std::cerr << ex.what() << std::endl;
	}
	return 0;
}
