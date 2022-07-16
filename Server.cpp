/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akarafi <akarafi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/23 16:32:03 by zoulhafi          #+#    #+#             */
/*   Updated: 2022/07/14 23:19:24 by akarafi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"

Server::Server(const Config &conf) {
	int														count = 0;
	int														socket;
	std::map<int, std::vector<VirtualServer> >::iterator	it_vserver;

	for (std::vector<string_string_map>::const_iterator it=conf.get_servers_config().begin(); it!=conf.get_servers_config().end(); ++it) {
		std::pair <string_map_multimap::const_iterator, string_map_multimap::const_iterator> locations;
		locations = conf.get_locations_config().equal_range("server_"+std::to_string(count++));
		VirtualServer vserver(*it, locations, conf.get_http_config());
		socket = this->_start_vserver(vserver);
		it_vserver = this->_vservers.find(socket);
		if (it_vserver != this->_vservers.end()) {
			this->_vservers[socket].push_back(vserver);
		} else {
			std::vector<VirtualServer> tmp_vector;
			tmp_vector.push_back(vserver);
			this->_vservers.insert(std::pair<int, std::vector<VirtualServer> >(socket, tmp_vector));
		}
	}
}

Server::~Server() {

}

int		Server::_start_vserver(const VirtualServer &vserver) {
	std::pair<std::string, int>								host_port_key;
	std::map<std::pair<std::string, int>, int>::iterator	it;

	host_port_key = std::pair<std::string, int>(vserver.get_host(), vserver.get_port());
	it = this->_host_port_socket_map.find(host_port_key);
	if (it != this->_host_port_socket_map.end()) {
		return it->second;
	} else {
		int						fd;
		struct sockaddr_in		addr;
		struct pollfd			pollfd;

		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(vserver.get_port());
		addr.sin_addr.s_addr = inet_addr(vserver.get_host().c_str());
		if (addr.sin_addr.s_addr == INADDR_NONE) {
			throw std::runtime_error("Bad Address IP: " + vserver.get_host());
		}
		fd = socket(addr.sin_family, SOCK_STREAM, 0);
		if (fd < 0) {
			throw std::runtime_error("Socket Creation Failed.");
		}
		if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
			throw std::runtime_error("Cannot make file descriptor to non-blocking mode.");
		}
		if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
			throw std::runtime_error("Binding "+vserver.get_host()+":"+std::to_string(vserver.get_port())+ " Failed.");
		}
		if (listen(fd, 100) < 0) {
			throw std::runtime_error("Listening on "+vserver.get_host()+":"+std::to_string(vserver.get_port())+ " Failed.");
		}
		memset(&pollfd, 0, sizeof(pollfd));
		pollfd.fd = fd;
		pollfd.events = POLLSTANDARD;
		this->_pollfds.push_back(pollfd);
		this->_host_port_socket_map.insert(std::pair<std::pair<std::string, int>, int>(std::pair<std::string, int>(vserver.get_host(), vserver.get_port()), fd));
		std::cout << "Server listening on "+vserver.get_host()+":"+std::to_string(vserver.get_port()) << std::endl;
		return fd;
	}
}

void	Server::_clear_pollfds() {
	std::vector<int>	disconnected;

	for (size_t i = 0; i < this->_pollfds.size(); i++) {
		if (this->_pollfds[i].fd == -1) {
			disconnected.push_back(i);
		}
	}
	for (size_t i = 0; i < disconnected.size(); i++) {
		this->_pollfds.erase(this->_pollfds.begin() + disconnected[i]);
	}
}

void	Server::start() {
	std::map<int, std::vector<VirtualServer> >::iterator	it;
	bool													is_running = true;
	std::vector<int>										new_connections;

	while (is_running) {
		new_connections.clear();
		_clear_pollfds();
		int r = poll(this->_pollfds.data(), this->_pollfds.size(), 0);
		if (r > 0) {
			for (size_t i = 0; i < this->_pollfds.size(); i++) {
				struct pollfd &current_poll = this->_pollfds.at(i);
				if (current_poll.revents == 0) {
					continue;
				} else if (current_poll.revents & POLLHUP) {
					close(current_poll.fd);
					current_poll.fd = -1;
					std::cout << "Client disconnected." << std::endl;
					continue;
				} else if (!(current_poll.revents & POLLIN)) {
					std::cerr << "Poll of fd : " << current_poll.fd << " - Error! revenets = " << current_poll.revents << std::endl;
					close(current_poll.fd);
					current_poll.fd = -1;
					continue;
				}
				it = this->_vservers.find(current_poll.fd);
				if (it != this->_vservers.end()) {
					new_connections.push_back(current_poll.fd);
				} else {
					this->receive(current_poll);
				}
			}
			this->accept_clients(new_connections);
		}
	}
}

void	Server::accept_clients(const std::vector<int> &connections) {
	int				new_socket;
	struct pollfd	new_poll;

	for (std::vector<int>::const_iterator it=connections.begin(); it!=connections.end(); ++it) {
		new_socket = accept(*it, NULL, NULL);
		if (new_socket < 0) {
			std::cerr << "Could not accept new connection." << std::endl;
		} else {
			new_poll.fd = new_socket;
			new_poll.events = POLLIN;
			this->_pollfds.push_back(new_poll);
			_clientSocket_hostSocket_map[new_socket] = *it;
			std::cout << "New connection established." << std::endl;
		}
	}
}

void	Server::receive(struct pollfd &poll) const {
	int		rc;
	char	buff[1000];

	rc = recv(poll.fd, buff, sizeof(buff), 0);
	if (rc < 0) {
		std::cerr << "Could not receive data from client." << std::endl;
		return ;
	} else if (rc == 0) {
		close(poll.fd);
		poll.fd = -1;
		std::cout << "Client disconnected." << std::endl;
		return ;
	}
	buff[rc] = '\0';
	std::cout << "Data received" << std::endl;
	Request req(buff);
	req.debug_print();
	int _socket = _clientSocket_hostSocket_map.at(poll.fd);
	Response res(req, _vservers.at(_socket));
	std::string response = *res;
	send(poll.fd, response.c_str(), response.length(), 0);
}
