#pragma once
#include "zinx.h"
class CmdMsg :
    public UserData
{
public:
    // 成员变量表示要回显的字符串
    std::string szUserData;
    // 是否开启输出标志
    bool isOpen = true;
    // 是否读消息是命令
    bool isCmd = false;
    // 是否加前缀
    bool needDatePre = false;
    CmdMsg();
    virtual ~CmdMsg();
};

