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

#include "magic.h"

MagicPacket::MagicPacket(std::string macAddress, std::string interfaceDevice) :
    m_mac(new unsigned int[6]),
    m_iface(interfaceDevice),
    m_packet(),
    m_sfd()
{
    m_sfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (m_sfd == -1) {
        std::cerr << "Could not create socket: " << std::strerror(errno) << std::endl;
        throw std::runtime_error("Could not create socket: " + std::string(std::strerror(errno)));
    }

    initAddresses();
    setMacAddress(macAddress);
    setInterface(interfaceDevice);
    createPacket();
}

MagicPacket::MagicPacket() :
    m_mac(new unsigned int[6]),
    m_iface(),
    m_packet(),
    m_sfd(),
    m_saddr(),
    m_ssaddr()
{
    initAddresses();
}

void MagicPacket::createPacket() {
    for (unsigned int i = 0; i < 6; i++) {
        m_packet.push_back(static_cast<unsigned char>(255));
    }

    for (unsigned char i = 0; i < 16; i++) {
        for (unsigned int i_ = 0; i_ < sizeof(*m_mac)/sizeof(m_mac[0]); i_++) {
            m_packet.push_back(m_mac[i_]);
        }
    }
}

void MagicPacket::setMacAddress(std::string macAddress) {
    unsigned int mac[6];
    int size = std::sscanf(macAddress.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);

    if (size != 6) {
        throw std::invalid_argument("Mac address is malformed.");
    }

    std::copy(&mac[0], &mac[6], m_mac);
}

void MagicPacket::setInterface(std::string interfaceDevice) {
    m_iface = interfaceDevice;
    if (setsockopt(m_sfd, SOL_SOCKET, SO_BINDTODEVICE, m_iface.c_str(), m_iface.size()) == -1) {
        std::cerr << "Failed to set socket option 'SO_BINDTODEVICE': " << std::strerror(errno) << std::endl;
        exit(EX_SOFTWARE);
    }
}

void MagicPacket::initAddresses() {

    m_sfd = socket(AF_INET, SOCK_DGRAM, 0);

    m_saddr.sin_family = AF_INET;

    m_saddr.sin_port = 0;

    m_saddr.sin_addr.s_addr = INADDR_ANY;

    m_ssaddr.sin_family = AF_INET;

    m_ssaddr.sin_addr.s_addr = inet_addr("255.255.255.255");

    m_ssaddr.sin_port = htons(9);

    int broadcast = 1;

    if (setsockopt(m_sfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) == -1) {
        std::cerr << "Failed to set socket option SO_BROADCAST" << std::strerror(errno) << std::endl;
        exit(EX_SOFTWARE);
    }

    if (bind(m_sfd, reinterpret_cast<sockaddr*>(&m_saddr), sizeof(m_saddr)) == -1) {
        std::cerr << "Failed to bind socket." << std::strerror(errno) << std::endl;
        exit(EX_SOFTWARE);
    }
}

int MagicPacket::send() {

    if (m_iface.empty()) {
        return -1;
    }
    int empty = 0;
    for (unsigned int i = 0; i < 6; i++) {
        if (m_mac[i] == 0) {
            empty++;
        }
    }

    if (empty == 6) {
        return -1;
    }

    if (sendto(m_sfd, reinterpret_cast<const void*>(m_packet.data()), m_packet.size(), 0, reinterpret_cast<sockaddr*>(&m_ssaddr), sizeof(m_ssaddr)) == -1) {
        std::cerr << "Failed to send wake up broadcast to socket: " << std::strerror(errno) << std::endl;
        return -1;
    }
    return 0;
}
