#include "ZinxTimer.h"
#include <sys/timerfd.h>

ZinxTimerChannel::ZinxTimerChannel() {}

ZinxTimerChannel::~ZinxTimerChannel() {}

bool ZinxTimerChannel::Init() {
	bool bRet = false;
	int iFd = timerfd_create(CLOCK_MONOTONIC, 0);
	if (iFd >= 0) {
		struct itimerspec period = {
			{3, 0}, {1, 0}  // 间隔10s,程序启动1s后开始计时
		};
			if (timerfd_settime(iFd, 0, &period, NULL) == 0) {
				bRet = true;
				m_TimerFd = iFd;
			}
	}
	return bRet;
}

bool ZinxTimerChannel::ReadFd(std::string & _input) {
	bool bRet = false;
	char buff[8] = { 0 };
	if (sizeof(buff) == read(m_TimerFd, buff, sizeof(buff))) { // 未到定时时间前，read会被阻塞，返回值为超时的数目
		bRet = true;
		_input.assign(buff, sizeof(buff));
	}
	return bRet;

}
 

bool ZinxTimerChannel::WriteFd(std::string& _output)
{
	return false;
}

/*关闭文件描述符*/
void ZinxTimerChannel::Fini()
{
	close(m_TimerFd);
	m_TimerFd = -1;
}

int ZinxTimerChannel::GetFd()
{
	return this->m_TimerFd;
}

std::string ZinxTimerChannel::GetChannelInfo() {
	return "TimerFd";
}

//class output_hello : public AZinxHandler {
//	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override {
//		Ichannel* pchannel = ZinxKernel::Zinx_GetChannel_ByInfo("stdout");
//		std::string outputData = "hello";
//		ZinxKernel::Zinx_SendOut(outputData, *pchannel); // 输出内容到标准输出
//		return nullptr;
//	}
//
//	virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
//	{
//		return nullptr;
//	}
//} *poutHello = new output_hello();

 AZinxHandler * ZinxTimerChannel::GetInputNextStage(BytesMsg& _oInput) {
	return &TimerOutMng::getInstance(); // 下一个handler
}

 //TimerOutMng TimerOutMng::single; // 初始化静态变量，调用构造函数
 TimerOutMng::TimerOutMng() {
	 for (int i = 0; i < 10; ++i) { // 置入10个空的链表元素
		 std::list<TimerOutProc*> tmp; 
		 this->m_timer_wheel.push_back(tmp);

	 }
  }

 IZinxMsg* TimerOutMng::InternelHandle(IZinxMsg& _oInput) {
	 unsigned long iTimeoutCount = 0;
	 GET_REF2DATA(BytesMsg, obytes, _oInput);
	 obytes.szData.copy((char*)&iTimeoutCount, sizeof(iTimeoutCount), 0); // 超时时间事件的个数存了在char里，
	 while (iTimeoutCount-- > 0) {
		 // 移动刻度
		 ++cur_index; // 每有一次超时事件，刻度+1
		 cur_index %= 10; // 刻度轮大小为10
		 std::list<TimerOutProc*> m_cache;


		 for (auto itr = m_timer_wheel[cur_index].begin(); itr != m_timer_wheel[cur_index].end();) { // 遍历单个刻度上的list
			 if ((*itr)->iCount <= 0) {
				 m_cache.push_back(*itr);
				 auto ptmp = *itr;
				 itr = m_timer_wheel[cur_index].erase(itr); // 将事件取出来，并加入回阻塞队列缓存里,并且因为erase了，并获取下一个迭代器，erase的时候会执行构析函数
				 addTask(ptmp);
			 }
			 else {
				 --(*itr)->iCount;
				 ++itr; // 下一个事件
			 }
		 }
		 for (TimerOutProc* task : m_cache) {
			 task->Proc();
		 }

	 }
	 return nullptr;
 }

 AZinxHandler* TimerOutMng::GetNextHandler(IZinxMsg& _oNextMsg)
 {
	 return nullptr;
 }

 void TimerOutMng::addTask(TimerOutProc* _ptask) {
	 int index = (_ptask->GetTimeSec() + cur_index) % 10;
	 m_timer_wheel[index].push_back(_ptask); // 取余散列到n个刻度的其中一个刻度上的list
	 _ptask->iCount = _ptask->GetTimeSec() / 10; // 设置圈数，圈数等于0，且刻度到了，则调用事件

 }

 void TimerOutMng::delTask(TimerOutProc* _ptask) {
	 
	 for (std::list<TimerOutProc*>& chi : m_timer_wheel) {
		 for (auto task : chi) {
			 if (task == _ptask) {
				 chi.remove(_ptask);
				 return;
			 }
		 }
	}
 }
