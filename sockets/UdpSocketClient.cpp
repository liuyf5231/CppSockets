/*
 * UdpSocketClient.cpp code for UDP client sockets
 *
 * Copyright (C) 2019 Simon D. Levy
 *
 * MIT License
 */

#include "UdpSocketClient.h"

#include <string.h>

UdpSocketClient::UdpSocketClient(const char * host, const short port)
{
    // Initialize Winsock, returning on failure
    if (!initWinsock()) return;

    // Create socket
    _sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_sock == SOCKET_ERROR) {
        sprintf_s(_message, "socket() failed");
        return;
    }

    // Setup address structure
    memset((char *)&_si_other, 0, sizeof(_si_other));
    _si_other.sin_family = AF_INET;
    _si_other.sin_port = htons(port);
    Socket::inetPton(host, _si_other);
}
