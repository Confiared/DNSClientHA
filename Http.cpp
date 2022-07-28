#include "Http.hpp"
#include <iostream>

#include <arpa/inet.h>
#include <sys/time.h>

void Http::dnsError()
{
    std::cerr << "Http::dnsError() " << __FILE__ << ":" << __LINE__ << " " << this << std::endl;
}

void Http::dnsWrong()
{
    std::cerr << "Http::dnsWrong() " << __FILE__ << ":" << __LINE__ << " " << this << std::endl;
}

void Http::dnsRight(const sockaddr_in6 &sIPv6)
{
    char str[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &sIPv6.sin6_addr, str, INET6_ADDRSTRLEN);
    std::cerr << "Http::dnsRight() " << str << " " << __FILE__ << ":" << __LINE__ << std::endl;
}
