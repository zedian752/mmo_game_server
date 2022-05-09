#include "EchoRole.h"
#include "CmdMsg.h"
#include "CmdCheck.h"

EchoRole::EchoRole()
{
}


EchoRole::~EchoRole()
{
}

bool EchoRole::Init()
{
    return true;
}

UserData* EchoRole::ProcMsg(UserData&
    _poUserData)
{
    // UserData 下转 Cmdmsg
    GET_REF2DATA(CmdMsg, input, _poUserData);
    CmdMsg* pout = new CmdMsg(input);
    ZinxKernel::Zinx_SendOut(*pout, *(CmdCheck::GetInstance()));
    return nullptr;

}

void EchoRole::Fini()
{
}

bool OutputCtrl::Init() {
    Irole* pRetRole = nullptr;
    for (auto pRole : ZinxKernel::Zinx_GetAllRole()) {
        auto pDate = dynamic_cast<DatePreRole*> (pRole);
        if (pDate != nullptr) {
            pRetRole = pDate;
            break;
        }
    }
    if (pRetRole != nullptr) {
        SetNextProcessor(*pRetRole);
    }
    return true;
}

UserData* OutputCtrl::ProcMsg(UserData& _poUserData) {
    GET_REF2DATA(CmdMsg, input, _poUserData);
    if (input.isOpen) {
        if (pOut != nullptr) {
            // 开启输出
            ZinxKernel::Zinx_Add_Channel(*pOut);
            pOut = nullptr;
        }
    }
    else {
        // 关闭输出
        auto pchannel = ZinxKernel::Zinx_GetChannel_ByInfo("stdout");
        ZinxKernel::Zinx_Del_Channel(*pchannel);
        pOut = pchannel;
    }
    return new CmdMsg(input);
}


void OutputCtrl::Fini() {

}

bool DatePreRole::Init() {
    Irole* pRetRole = nullptr;
    for (auto pRole : ZinxKernel::Zinx_GetAllRole()) {
        auto pDate = dynamic_cast<EchoRole*> (pRole);
        if (pDate != nullptr) {
            pRetRole = pDate;
            break;
        }
    }
    if (pRetRole != nullptr) {
        SetNextProcessor(*pRetRole);
    }
    return true;
}

UserData* DatePreRole::ProcMsg(UserData& _poUserData) {
    CmdMsg* pret = nullptr;
    GET_REF2DATA(CmdMsg, input, _poUserData);
    if (input.isCmd) {
        needAdd = input.needDatePre;
    }
    else {
        if (needAdd) {
            time_t tmp;
            time(&tmp);
            std::string szNew = std::string(ctime(&tmp)) + input.szUserData;
            pret = new CmdMsg(input);
            pret->szUserData = szNew;
        }
        else {
            pret = new CmdMsg(input);
        }
    }
}

void DatePreRole::Fini() {

}