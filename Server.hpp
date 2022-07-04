/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zoulhafi <zakariaa@oulhafiane.me>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/23 14:03:13 by zoulhafi          #+#    #+#             */
/*   Updated: 2022/05/25 18:32:26 by zoulhafi         ###   ########.fr       */
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

class Server
{
	private:
		std::map<std::pair<std::string, int>, int>	_host_port_socket_map;
		std::map<int, std::vector<VirtualServer> >	_vservers;
		std::vector<pollfd>							_pollfds;
		int											_start_vserver(const VirtualServer &vserver);
		void										_clear_pollfds();

	public:
		Server(const Config &conf);
		~Server();
		void										start();
		void										accept_clients(const std::vector<int> &connections);
		void										receive(struct pollfd &poll) const;
};

#endif
