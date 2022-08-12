#include <vector>
#include <cstring>
#include <cstdio>
#include <signal.h>
#include <iostream>
#include <sys/stat.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>

#include "Dns.hpp"
#include "Http.hpp"
#include "Timer/DNSCache.hpp"
#include "Timer/DNSQuery.hpp"
#include <algorithm>

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    //the event loop
    struct epoll_event ev, events[255];
    memset(&ev,0,sizeof(ev));
    int nfds, epollfd;

    ev.events = EPOLLIN|EPOLLET;

    if ((epollfd = epoll_create1(0)) == -1) {
        printf("epoll_create1: %s", strerror(errno));
        return -1;
    }
    EpollObject::epollfd=epollfd;
    Dns::dns=new Dns();
    DNSCache dnsCache;
    dnsCache.start(3600*1000);
    DNSQuery dnsQuery;
    dnsQuery.start(10);
    
    #ifdef DEBUGDNS
    {
        FILE *stream;
        char *line = NULL;
        size_t len = 0;
        ssize_t nread;

        stream = fopen("dns.txt", "r");
        if (stream != NULL)
        {
            while ((nread = getline(&line, &len, stream)) != -1) {
                std::string str(line);
                std::size_t pos=str.find(" ");
                if (pos!=std::string::npos)
                {
                    std::string host=str.substr(0,pos);
                    std::string ipv6=str.substr(pos+1);
                    if(!ipv6.empty())
                    {
                        if(ipv6.at(ipv6.size()-1)=='\n')
                            ipv6=ipv6.substr(0,ipv6.size()-1);
                        Dns::dns->hardcodedDns[host]=ipv6;
                    }
                }
            }

            free(line);
            fclose(stream);
        }
    }
    #endif

    Http *http=new Http();
    Dns::dns->getAAAA(http,"confiared.com",true);

    for (;;) {
        if ((nfds = epoll_wait(epollfd, events, 255, -1)) == -1)
            printf("epoll_wait error %s", strerror(errno));
        for (int n = 0; n < nfds; ++n)
        {
            epoll_event &e=events[n];
            switch(static_cast<EpollObject *>(e.data.ptr)->getKind())
            {
                case EpollObject::Kind::Kind_Dns:
                {
                    DnsSocket * dnsSocket=static_cast<DnsSocket *>(e.data.ptr);
                    dnsSocket->parseEvent(e);
                }
                break;
                case EpollObject::Kind::Kind_Timer:
                {
                    static_cast<Timer *>(e.data.ptr)->exec();
                    static_cast<Timer *>(e.data.ptr)->validateTheTimer();
                }
                break;
                default:
                break;
            }
        }
    }

    return 0;
}
