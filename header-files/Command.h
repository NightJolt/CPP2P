#pragma once

#include <map>
#include <vector>

class Command {
public:
    Command();
    explicit Command(std::string);
    ~Command();

    void SetCommand(std::string);

    std::string GetCommandRaw();
    std::string GetCommand();
    std::string GetValue(std::string);
    std::string GetContent(int);
    int GetContentCnt();

private:
    std::string command_raw;
    int content_count;

    std::string command;
    std::map <std::string, std::string> values;
    std::vector <std::string> contents;
};