#ifndef HTTP_H
#define HTTP_H

#include <string>
#include <vector>
#include <netinet/in.h>

class Http
{
public:
    void dnsRight(const sockaddr_in6 &sIPv6);
    void dnsError();
    void dnsWrong();
};

#endif // HTTP_H
