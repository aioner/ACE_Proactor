#include <ace/Proactor.h>
#include "TCPNetThread.h"

namespace igame
{
	int TTcpNetThread::open() { return this->activate(); }

	int TTcpNetThread::close()
	{
		ACE_Proactor::instance()->proactor_end_event_loop(); // 终止ACE_Proactor循环

		this->wait(); // 等待清理现场

		return 0;
	}
	
	int TTcpNetThread::svc()
	{
		ACE_INET_Addr listenAddr(DEF_LISTENING_PORT); // 默认监听地址
		TTcpAcceptor tcpAcceptor; // 接收器

		// 设置事件
		tcpAcceptor.setOnClientConnect(m_OnClientConnect);
		tcpAcceptor.setOnClientDisconnect(m_OnClientDisconnect);
		tcpAcceptor.setOnClientValidate(m_OnClientValidate);
		tcpAcceptor.setOnDataReceive(m_OnDataReceive);
		tcpAcceptor.setOnDataSendFailed(m_OnDataSendFailed);
		tcpAcceptor.setOnDataSendSucceeded(m_OnDataSendSucceeded);
		
		// 演出开始
		if (tcpAcceptor.open(listenAddr, 0, 1, 5, 1, 0, 0) != 0)
			ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("failed to open TcpAcceptor errno=%i\n"), errno), -1);

		// Proactor的事件循环开始
		ACE_Proactor::instance()->proactor_run_event_loop();

		ACE_DEBUG((LM_DEBUG, ACE_TEXT("Network fin\n")));

		return 0;
	}
} // namespace igame