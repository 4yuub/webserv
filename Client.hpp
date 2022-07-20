#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <ctime>
#include "Request.hpp"

class Client
{
    private:
        Request         _request;
        int             _last_time;

    public:
        Client();
        Request         &get_request();
        std::time_t     get_last_time() const;
        bool            is_timed_out() const;
        void            update_last_time();
        void            set_new_request();
};

#endif