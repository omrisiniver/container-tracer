#ifndef _NETWORKCLIENT_
#define _NETWORKCLIENT_

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

class network_client {
public:
    bool init(const std::string& ip, const uint16_t port) {
        m_sock = socket(AF_INET, SOCK_STREAM, 0);
        if (m_sock < 0) {
            return false;
        }

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);

        if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) {
            return false;
        }

        if (connect(m_sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
            return false;
        }

        return true;
    } 

    bool send_data(const std::string& data) {
        return (send(m_sock, data.c_str(), data.length(), 0) == data.length());
    }

private:
    int m_sock;
};

#endif