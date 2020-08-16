#pragma once

#include <SFML/Network.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include "Command.h"
#include <vector>

#include <iostream>

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

class User {
public:
    static const unsigned int CHARACTER_INPUT_LIMIT;
    static const std::string LOCAL_DEBUG_LOG;

    User(unsigned short, std::string&, unsigned short);
    virtual ~User();

    // Called on every frame
    virtual void Update() = 0;

    // Draw UI
    virtual void Draw();

    std::string name; // Nickname for current chat

protected:
    // Create/Join room
    virtual void Connect() = 0;

    // Check for disconnected host/clients
    virtual void CheckDisconnected() = 0;

    // Listen to users and receive sent packets
    virtual void ReceivePacket() = 0;

    // Extract message from sf::Packet
    virtual std::string UnpackMessage(sf::Packet&);

    // Process command
    virtual void ProcessCommand(std::string&);

    /*
     * Send given string as message/command
     * Calls FormatMessage -> ValidateMessage -> PackMessage -> SendPacket
     */
    virtual void SendMessage(std::string&);

    // Convert plain text to command if required
    virtual void FormatMessage(std::string&);

    // Check if (command is valid)/(user has enough permissions)
    virtual bool ValidateMessage(std::string&);

    // Store all data in sf::Packet
    virtual sf::Packet PackMessage(std::string&);

    // Send packet to host/clients
    virtual void SendPacket(sf::Packet&) = 0;

    // Push message to chat_history
    virtual void PushMessage(std::string&, std::string&);

    // Local debug log
    virtual void Log(std::string, std::string);

    // Check if user is connected to given socket
    bool IsConnected(sf::TcpSocket&);

    // Check if you are the host
    virtual bool IsMaster();

    // Initialize local lua bot
    //virtual void InitLocalBot(std::string);

    // Check if lua state command successed
    virtual bool ValidateLuaState(lua_State*, int);

    unsigned short id; // Local chat id
    unsigned short port; // Used port for socket communications
    //ImVec4 main_color; // Chat color

    char* input_buffer; // Stores current message text

    std::vector <std::string> chat_history; // Stores all messages
};