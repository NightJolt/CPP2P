#pragma once

#include "User.h"

class Client : public User {
public:
    Client(std::string, unsigned short, std::string, unsigned short);
    ~Client() override;

private:
    void Connect() override;
    void Update() override;
    void CheckDisconnected() override;
    void ReceivePacket() override;
    void SendPacket(sf::Packet&) override;

    sf::TcpSocket master; // Host socket
    std::string ip; // Public/Local IP
};