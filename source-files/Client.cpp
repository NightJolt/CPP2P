#include "Client.h"



Client::Client(std::string ip, unsigned short port, std::string name, unsigned short id) : User(port, name, id), ip(ip) {
    Connect();
}

Client::~Client() {
    if (IsConnected(master)) master.disconnect();
}



void Client::Connect() {
    // SET BLOCKING

    if (master.connect(ip, port) != sf::Socket::Done) {
        Log(LOCAL_DEBUG_LOG, "Failed to connect");
    } else {
        Log(LOCAL_DEBUG_LOG, "Connected");

        master.setBlocking(false);
    }
}



void Client::Update() {
    CheckDisconnected();
    ReceivePacket();
}



void Client::CheckDisconnected() {
    //if (!IsConnected(master)) Log("Master Disconnected");
}

void Client::SendPacket(sf::Packet& packet) {
    master.send(packet);
}

void Client::ReceivePacket() {
    sf::Packet packet;
    std::string s;

    if (master.receive(packet) == sf::Socket::Done) {
        packet >> s;

        ProcessCommand(s);
    }
}