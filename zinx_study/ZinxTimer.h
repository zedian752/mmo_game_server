#pragma once
#include "zinx.h"
#include <vector>
#include <list>
class ZinxTimerChannel :
    public Ichannel
{
    int m_TimerFd = -1;
public:
	ZinxTimerChannel();
	virtual ~ZinxTimerChannel();

	virtual bool Init() override;

	virtual bool ReadFd(std::string& _input) override;
	virtual bool WriteFd(std::string& _output) override;
	virtual void Fini() override;
	virtual int GetFd() override;
	virtual std::string GetChannelInfo() override;
	virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};

class TimerOutProc {
public:
	virtual void Proc() = 0; // 定时任务处理函数,需要自设定
	virtual int GetTimeSec() = 0; // 此定时任务多少秒后执行
	// 所到圈数
	int iCount = -1;
};

class TimerOutMng : public AZinxHandler {
	TimerOutMng();
	std::vector<std::list<TimerOutProc*>> m_timer_wheel; // 定时器时间轮
	int cur_index = 0;
	static TimerOutMng single;
public:
	static TimerOutMng& getInstance() {
		return single;
	}
	/*处理超时事件,遍历所有超时任务*/
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override;

	virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override;
	void addTask(TimerOutProc* _ptask);
	void delTask(TimerOutProc* _ptask);
};
