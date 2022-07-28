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
