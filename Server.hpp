/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akarafi <akarafi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/23 14:03:13 by zoulhafi          #+#    #+#             */
/*   Updated: 2022/07/14 22:43:52 by akarafi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <vector>
#include <poll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include "Config.hpp"
#include "VirtualServer.hpp"
#include "Client.hpp"

class Server
{
	private:
		std::string									_debug;
		std::map<std::pair<std::string, int>, int>	_host_port_socket_map;
		std::map<int, int>							_clientSocket_hostSocket_map;
		std::map<int, Client>						_clientSocket_client_map;
		std::map<int, std::vector<VirtualServer> >	_vservers;
		std::vector<pollfd>							_pollfds;
		int											_start_vserver(const VirtualServer &vserver);
		void										_clear_pollfds();

	public:
		Server(const Config &conf);
		~Server();
		void										start();
		void										accept_clients(const std::vector<int> &connections);
		void										receive(struct pollfd &poll);
};

#endif
