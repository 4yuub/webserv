/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akarafi <akarafi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/23 16:32:03 by zoulhafi          #+#    #+#             */
/*   Updated: 2022/09/17 22:57:34 by akarafi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"

Server::Server(const Config &conf) {
	int														count = 0;
	int														socket;
	std::map<int, std::vector<VirtualServer> >::iterator	it_vserver;

	try {
		this->_debug = conf.get_global_config().at("debug");
	} catch (std::exception &ex) {
		this->_debug = "false";
	}
	_set_default_error_pages(conf);
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

std::string get_content_of_file(std::string const &path) {
	std::ifstream file(path);
	std::string content;
	std::getline(file, content, '\0');
	return content;
}

void	Server::_set_default_error_pages(const Config &conf) {
	std::map<std::string, std::string> g_config = conf.get_global_config();
	this->_error_pages[400] = "<H1>400 Bad Request</H1>";
	this->_error_pages[401] = "<H1>401 Unauthorized</H1>";
	this->_error_pages[403] = "<H1>403 Forbidden</H1>";
	this->_error_pages[404] = "<H1>404 Not Found</H1>";
	this->_error_pages[405] = "<H1>405 Method Not Allowed</H1>";
	this->_error_pages[500] = "<H1>500 Internal Server Error</H1>";
	if (g_config["error_400"] != "") {
		this->_error_pages[400] = get_content_of_file(g_config["error_400"]);
	}
	if (g_config["error_401"] != "") {
		this->_error_pages[401] = get_content_of_file(g_config["error_401"]);
	}
	if (g_config["error_403"] != "") {
		this->_error_pages[403] = get_content_of_file(g_config["error_403"]);
	}
	if (g_config["error_404"] != "") {
		this->_error_pages[404] = get_content_of_file(g_config["error_404"]);
	}
	if (g_config["error_405"] != "") {
		this->_error_pages[405] = get_content_of_file(g_config["error_405"]);
	}
	if (g_config["error_500"] != "") {
		this->_error_pages[500] = get_content_of_file(g_config["error_500"]);
	}
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
		int						enable;
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
		enable = 1;
		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
			throw std::runtime_error("Cannot set SO_REUSEADDR socket option.");
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
		std::cout << "Server listening on http://"+vserver.get_host()+":"+std::to_string(vserver.get_port()) << std::endl;
		return fd;
	}
}

void	Server::_clear_pollfds() {
	std::map<int, int>		disconnected;

	for (std::map<int, Client>::iterator it = this->_clientSocket_client_map.begin(); it != this->_clientSocket_client_map.end(); ++it) {
		if (it->second.is_timed_out()) {
			disconnected[it->first] = -1;
			close(it->first);
			std::cout << "Client timed out." << std::endl;
		}
	}
	for (size_t i = 0; i < this->_pollfds.size(); i++) {
		if (this->_pollfds[i].fd == -1) {
			disconnected[this->_pollfds[i].fd] = i;
		} else {
			std::map<int, int>::iterator it = disconnected.find(this->_pollfds[i].fd);
			if (it != disconnected.end()) {
				disconnected[this->_pollfds[i].fd] = i;
			}
		}
	}
	for (std::map<int, int>::iterator it = disconnected.begin(); it != disconnected.end(); ++it) {
		if (it->second != -1) {
			this->_pollfds.erase(this->_pollfds.begin() + it->second);
		}
		this->_clientSocket_client_map.erase(it->first);
		this->_clientSocket_hostSocket_map.erase(it->first);
	}
}

void	Server::_close_socket(struct pollfd &current_poll)
{
	std::map<int, Client>::iterator it = _clientSocket_client_map.find(current_poll.fd);
	if (it != _clientSocket_client_map.end()) {
		_clientSocket_client_map.erase(it);
	}
	std::map<int, int>::iterator it2 = _clientSocket_hostSocket_map.find(current_poll.fd);
	if (it2 != _clientSocket_hostSocket_map.end()) {
		_clientSocket_hostSocket_map.erase(it2);
	}
	close(current_poll.fd);
	current_poll.fd = -1;
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
					std::cout << "Client disconnected." << std::endl;
					_close_socket(current_poll);
					continue;
				} else if (!(current_poll.revents & POLLIN)) {
					std::cerr << "Poll of fd : " << current_poll.fd << " - Error! revenets = " << current_poll.revents << std::endl;
					_close_socket(current_poll);
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
			_clientSocket_client_map[new_socket].update_last_time();
			std::cout << "New connection established." << std::endl;
		}
	}
}

void	Server::receive(struct pollfd &poll) {
	int				rc;
	char			buff[1000];

	Client &client = _clientSocket_client_map.at(poll.fd);
	client.update_last_time();
	memset(buff, 0, 1000);
	while ((rc = recv(poll.fd, buff, sizeof(buff) - 1, 0)) > 0) {
		buff[rc] = '\0';
		client.get_request().update_raw_request(buff);
	}
	if (rc == -1 && buff[0] == '\0') {
		std::cerr << "Error Occured: Cannot receive data from client." << std::endl;
		_close_socket(poll);
		return ;
	} else if (rc == 0) {
		std::cout << "Client disconnected." << std::endl;
		_close_socket(poll);
		return ;
	}
	std::cout << "Data received" << std::endl;
	client.get_request().parse_request();
	if (this->_debug == "true") {
		client.get_request().debug_print();
	}
	if (client.get_request().is_request_ended()) {
		int _socket = _clientSocket_hostSocket_map.at(poll.fd);
		Response res(client.get_request(), _vservers.at(_socket), _error_pages);
		std::string response = *res;
		int ret = send(poll.fd, response.c_str(), response.length(), 0);
		if (ret < 0) {
			std::cout << "Error Occured: Cannot send data to client." << std::endl;
			_close_socket(poll);
		}
		else if (client.get_request().get_connection() == "close") {
			std::cout << "Client disconnected." << std::endl;
			_close_socket(poll);
		} else {
			client.set_new_request();
		}
	}
}