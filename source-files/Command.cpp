/*
 * Example: /cmd name:smth message [spaced key]:[spaced value] [hello world]
 *
 * cmds:
 * msg - common message
 * sys - system command
 * onjoin - send first time data (clients only)
 * ondisc - send data on disconnect
 * joinpbot - add public bot (master only)
 * joinlbot - add local bot
 *
 * args:
 * name - specify id name
 * target - specify target name
 * notarget - specify name to skip
 *
 * todo
 * implement array
 */

#include "Command.h"



Command::Command() = default;

Command::Command(std::string cmd) {
    SetCommand(cmd);
}

Command::~Command() = default;



std::string Command::GetCommandRaw() {
    return command_raw;
}

std::string Command::GetCommand() {
    return command;
}

std::string Command::GetValue(std::string key) {
    return values.count(key) ? values[key] : "";
}

std::string Command::GetContent(int i) {
    return i < contents.size() && i > - 1 ? contents[i] : "";
}

int Command::GetContentCnt() {
    return content_count;
}

void Command::SetCommand(std::string cmd) {
    values.clear();
    contents.clear();
    content_count = 0;

    if (cmd[0] == '/') cmd.erase(cmd.begin());
    cmd += ' ';

    command_raw = cmd;
    command = "";


    std::string key = "";
    std::string val = "";
    bool locked = false;
    bool is_key = true;
    bool is_cmd = true;

    for (int i = 0; i < cmd.size(); i++) {
        if (is_cmd) {
            if (cmd[i] == ' ') {
                is_cmd = false;
            } else {
                command += cmd[i];
            }
        } else {
            if (cmd[i] == ':' && !locked) {
                is_key = false;
                continue;
            }

            if (cmd[i] == ' ' && !locked) {
                if (is_key) {
                    if (key != "") {
                        contents.push_back(key);
                        content_count++;
                    }
                } else {
                    is_key = true;
                    values[key] = val;
                }

                key = val = "";

                continue;
            }

            if (cmd[i] == '[') {
                locked = true;
                continue;
            }

            if (cmd[i] == ']') {
                locked = false;
                continue;
            }

            if (is_key) {
                key += cmd[i];
            } else {
                val += cmd[i];
            }
        }
    }
}