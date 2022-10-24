/*
MIT License

Copyright (c) 2022 Timothy Hutchins

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "main.h"

std::string macAddr;

int sendMagic(std::string macAddress) {
    std::vector<unsigned char> magic;

    unsigned int mac[6];

    int size = std::sscanf(macAddress.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
    if (size != 6) {
        std::cerr << "Error: Mac address is mallformed." << std::endl;
        exit(EX_USAGE);
    }

    for (unsigned int i = 0; i < 6; i++) {
        magic.push_back(static_cast<unsigned char>(255));
    }

    for (unsigned char i = 0; i < 16; i++) {
        for (unsigned int i_ = 0; i_ < sizeof(mac)/sizeof(mac[0]); i_++) {
            magic.push_back(mac[i_]);
        }
    }
    int broadcast = 1;

    sockaddr_in saddr, ssaddr;

    saddr.sin_family = AF_INET;

    saddr.sin_port = 0;

    saddr.sin_addr.s_addr = INADDR_ANY;

    ssaddr.sin_family = AF_INET;

    ssaddr.sin_addr.s_addr = inet_addr(macAddress.c_str());

    ssaddr.sin_port = htons(9);

    int sfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sfd == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        exit(EX_SOFTWARE);
    }

    if (setsockopt(sfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) == -1) {
        std::cerr << "Failed to set socket options." << std::endl;
        exit(EX_SOFTWARE);
    }

    if (bind(sfd, (sockaddr*)&saddr, sizeof(saddr)) == -1) {
        std::cerr << "Failed to bind socket." << std::endl;
        exit(EX_SOFTWARE);
    }

    if (sendto(sfd, reinterpret_cast<const void*>(magic.data()), magic.size(), 0, reinterpret_cast<sockaddr*>(&ssaddr), sizeof(ssaddr)) == -1) {
        std::cerr << "Failed to send wake up broadcast to socket: " << strerror(errno) << std::endl;
    }

    return 1;
}

void handleGET(web::http::http_request request) {
    const utility::string_t qs = request.request_uri().query();
    auto q = web::uri::split_query(qs);

    if (q.find(POWER_ON) != q.end()) {
        sendMagic("98:29:a6:7b:5c:d8");
    }
}


int main(int argc, char **argv) {

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <mac address>" << std::endl;
        exit(EX_USAGE);
    }

    web::http::experimental::listener::http_listener listener;

    sendMagic(argv[1]);

    return EXIT_SUCCESS;
}
