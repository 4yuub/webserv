#include "Client.hpp"

Client::Client() : _request(), _last_time(std::time(nullptr)) { }

Request             &Client::get_request()
{
    return this->_request;
}

std::time_t         Client::get_last_time() const
{
    return this->_last_time;
}

void                Client::update_last_time()
{
    this->_last_time = std::time(nullptr);
}

void                Client::set_new_request()
{
    this->_request = Request();
}