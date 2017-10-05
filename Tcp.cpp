#include "Tcp.h"

namespace igame
{
	TTcp::TTcp()
		:m_TcpNetThd(0)
	{
		ACE_NEW_NORETURN(m_TcpNetThd, TTcpNetThread()); // 创建TTcpNetThread对象实例
	}

	TTcp::~TTcp()
	{
		if (m_TcpNetThd) // 释放
			delete m_TcpNetThd;
	}

	void TTcp::open()
	{
		ACE_TRACE("TTcp::open");
		
		// 所有TTcpNetThread的事件，交由TTcp来处理
		// TOnClientValidate除外，该事件需要特定的逻辑，且无法异步
		if (m_TcpNetThd)
		{
			m_TcpNetThd->setOnClientConnect(EVENT(TTcpHandler::TOnClientConnect, TTcp, this, tcpNetThread_OnClientConnect));
			m_TcpNetThd->setOnClientDisconnect(EVENT(TTcpHandler::TOnClientDisconnect, TTcp, this, tcpNetThread_OnClientDisconnect));
			m_TcpNetThd->setOnClientValidate(m_OnClientValidate);
			m_TcpNetThd->setOnDataReceive(EVENT(TTcpHandler::TOnDataReceive, TTcp, this, tcpNetThread_OnDataReceive));
			m_TcpNetThd->setOnDataSendFailed(EVENT(TTcpHandler::TOnDataSendFailed, TTcp, this, tcpNetThread_OnDataSendFailed));
			m_TcpNetThd->setOnDataSendSucceeded(EVENT(TTcpHandler::TOnDataSendSucceeded, TTcp, this, tcpNetThread_OnDataSendSucceeded));
		}

		if (activate() == -1)
			ACE_DEBUG((LM_ERROR, ACE_TEXT("Resume thread failed")));
	}

	void TTcp::close()
	{
		if (m_TcpNetThd)
			m_TcpNetThd->close();

		ACE_TRACE("TTcp::close");

		ACE_Message_Block* termBlock; // 结束信号
		
		ACE_NEW_NORETURN(termBlock, ACE_Message_Block(0, ACE_Message_Block::MB_HANGUP));

		if (!termBlock)
			ACE_DEBUG((LM_ERROR, ACE_TEXT("Allocate failed %i"), errno));
		else
		{
			putq(termBlock);

			wait();
		}
	}

	int TTcp::send(ACE_UINT32 ip, ACE_UINT16 port, unsigned int seq, const char* buf, unsigned short len)
	{
		ACE_Message_Block* mb = 0; // 数据包

		ACE_NEW_RETURN(mb, ACE_Message_Block(sizeof(ACE_UINT32) + sizeof(ACE_UINT16) + sizeof(unsigned int) + sizeof(unsigned short) + len, TCP_DATA_SEND), -1);
		
		// 格式：ip | port | seq | len | 数据...
		mb->copy((const char *)&ip, sizeof(ACE_UINT32));
		mb->copy((const char *)&port, sizeof(ACE_UINT16));
		mb->copy((const char *)&seq, sizeof(unsigned int));
		mb->copy((const char *)&len, sizeof(unsigned short));
		mb->copy(buf, len);

		return putq(mb);
	}

	int TTcp::svc()
	{
		ACE_TRACE("TTcp::svc");

		if (m_TcpNetThd->open() == -1)
			ACE_DEBUG((LM_ERROR, ACE_TEXT("Failed to pen TTcpNetThread: %i"), errno));

		ACE_Message_Block* msg = 0;

		while(true)
		{
			if (getq(msg) == -1)
			{
				ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Failed to getq %i"), errno), -1);
			}

			switch(msg->msg_type())
			{
			case ACE_Message_Block::MB_HANGUP: // 偶要退出
				{
					ACE_DEBUG((LM_DEBUG, ACE_TEXT("Quit")));

					msg->release();

					return 0;
				}
				break;
			case TCP_CLIENT_CONNECT: // 客户端连接
				{
					int len = msg->length();
					int hLen = sizeof(TTcpHandler *);

					if (msg->length() != TCP_PACK_HEADER_SIZE + sizeof(TTcpHandler *))
						ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Tcp connection message block invalid!")), -1);

					char* ptr = msg->rd_ptr();
					ACE_UINT32 ip = *(ACE_UINT32 *)ptr; ptr += sizeof(ACE_UINT32);
					ACE_UINT16 port = *(ACE_UINT16 *)ptr; ptr += sizeof(ACE_UINT16);
					TTcpHandler* handler = (TTcpHandler *)(*(int *)ptr);

					{
						ACE_Guard<ACE_Recursive_Thread_Mutex> lock(m_Lock);

						m_AddrMap.insert(make_pair<unsigned __int64, TTcpHandler *>((unsigned __int64)ip << 32 | port, handler)); 
					}				
					
					m_OnClientConnect(ip, port, handler);
				}
				break;
			case TCP_CLIENT_DISCONNECT: // 客户端断开连接
				{
					if (msg->length() != sizeof(ACE_UINT32) + sizeof(ACE_UINT16))
						ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Invalid tcp disconnect message block\n")), -1);

					char* ptr = msg->rd_ptr();
					ACE_UINT32 ip = *(ACE_UINT32 *)ptr; ptr += sizeof(ACE_UINT32);
					ACE_UINT16 port = *(ACE_UINT16 *)ptr;

					{
						ACE_Guard<ACE_Recursive_Thread_Mutex> lock(m_Lock);

						m_AddrMap.erase((unsigned __int64)ip << 32 | port);
					}

					m_OnClientDisconnect(ip, port);
				}
				break;
			case TCP_DATA_RECEIVE:
				{
					char* ptr = msg->rd_ptr();
					ACE_UINT32 ip = *(ACE_UINT32 *)ptr; ptr += sizeof(ACE_UINT32);
					ACE_UINT16 port = *(ACE_UINT16 *)ptr; ptr += sizeof(ACE_UINT16);
					TTcpPackHeader* header = (TTcpPackHeader *)ptr; ptr += TCP_PACK_HEADER_SIZE;
					const char* data = ptr;

					m_OnDataReceive(ip, port, header->seq, data, header->len);
				}
				break;
			case TCP_DATA_SEND:
				{
					if (msg->length() > sizeof(TTcpPackHeader))
					{
						char* ptr = msg->rd_ptr();
						ACE_UINT32 ip = *(ACE_UINT32 *)ptr; ptr += sizeof(ACE_UINT32);
						ACE_UINT16 port = *(ACE_UINT16 *)ptr; ptr += sizeof(ACE_UINT16);
						unsigned int seq = *(unsigned int *)ptr; ptr += sizeof(unsigned int);
						unsigned short len = *(unsigned short *)ptr; ptr += sizeof(unsigned short);
						const char* data = ptr;
						
						{
							ACE_Guard<ACE_Recursive_Thread_Mutex> _lock(m_Lock);

							hash_map<unsigned __int64, TTcpHandler *>::iterator it = m_AddrMap.find((unsigned __int64)ip << 32 | port);

							if (it != m_AddrMap.end())
							{
								(*it).second->send(seq, data, len);
							}
						}
					}
				}
				break;
			case TCP_DATA_SEND_SUCCEEDED:
				{
					char* ptr = msg->rd_ptr();
					ACE_UINT32 ip = *(ACE_UINT32 *)ptr; ptr += sizeof(ACE_UINT32);
					ACE_UINT16 port = *(ACE_UINT16 *)ptr; ptr += sizeof(ACE_UINT16);
					TTcpPackHeader* header = (TTcpPackHeader *)ptr; ptr += TCP_PACK_HEADER_SIZE;
					const char* data = ptr;

					m_OnDataSendSucceeded(ip, port, header->seq, data, header->len);
				}
				break;
			case TCP_DATA_SEND_FAILED:
				{
					char* ptr = msg->rd_ptr();
					ACE_UINT32 ip = *(ACE_UINT32 *)ptr; ptr += sizeof(ACE_UINT32);
					ACE_UINT16 port = *(ACE_UINT16 *)ptr; ptr += sizeof(ACE_UINT16);
					TTcpPackHeader* header = (TTcpPackHeader *)ptr; ptr += TCP_PACK_HEADER_SIZE;
					const char* data = ptr;

					m_OnDataSendFailed(ip, port, header->seq, data, header->len);
				}
				break;

			default:
				{
					ACE_DEBUG((LM_ERROR, ACE_TEXT("Unknown ACE_Message_Block type %i\n"), msg->msg_type()));
				}
				break;
			} // switch

			msg->release();
		} // while true

		return 0;
	}

	void TTcp::tcpNetThread_OnClientConnect(ACE_UINT32 ip, ACE_UINT16 port, TTcpHandler* handler)
	{
		ACE_Message_Block* mb = 0;

		ACE_NEW_NORETURN(mb, ACE_Message_Block(sizeof(ACE_UINT32) + sizeof(ACE_UINT16) + sizeof(TTcpHandler *), TCP_CLIENT_CONNECT));

		if (mb)
		{
			mb->copy((const char *)&ip, sizeof(ACE_UINT32));
			mb->copy((const char *)&port, sizeof(ACE_UINT16));
			mb->copy((const char *)&handler, sizeof(TTcpHandler *));

			this->putq(mb);
		}
	}

	void TTcp::tcpNetThread_OnClientDisconnect(ACE_UINT32 ip, ACE_UINT16 port)
	{
		ACE_Message_Block* mb = 0;

		ACE_NEW_NORETURN(mb, ACE_Message_Block(sizeof(ACE_UINT32) + sizeof(ACE_UINT16), TCP_CLIENT_DISCONNECT));

		if (mb)
		{
			mb->copy((const char *)&ip, sizeof(ACE_UINT32));
			mb->copy((const char *)&port, sizeof(ACE_UINT16));
			
			this->putq(mb);
		}
	}
	
	void TTcp::tcpNetThread_OnDataReceive(ACE_UINT32 ip, ACE_UINT16 port, unsigned int seq, const char* data, unsigned short size)
	{
		ACE_Message_Block* mb = 0;

		ACE_NEW_NORETURN(mb, ACE_Message_Block(sizeof(ACE_UINT32) + sizeof(ACE_UINT16) + TCP_PACK_HEADER_SIZE + size, TCP_DATA_RECEIVE));

		if (mb)
		{
			mb->copy((const char *)&ip, sizeof(ACE_UINT32));
			mb->copy((const char *)&port, sizeof(ACE_UINT16));
			mb->copy((const char *)&seq, sizeof(unsigned int));
			mb->copy((const char *)&size, sizeof(unsigned short));
			mb->copy(data, size);
			
			this->putq(mb);
		}
	}

	void TTcp::tcpNetThread_OnDataSendSucceeded(ACE_UINT32 ip, ACE_UINT16 port, unsigned int seq, const char* data, unsigned short size)
	{
		ACE_Message_Block* mb = 0;

		ACE_NEW_NORETURN(mb, ACE_Message_Block(sizeof(ACE_UINT32) + sizeof(ACE_UINT16) + TCP_PACK_HEADER_SIZE + size, TCP_DATA_SEND_SUCCEEDED));

		if (mb)
		{
			mb->copy((const char *)&ip, sizeof(ACE_UINT32));
			mb->copy((const char *)&port, sizeof(ACE_UINT16));
			mb->copy((const char *)&seq, sizeof(unsigned int));
			mb->copy((const char *)&size, sizeof(unsigned short));
			mb->copy(data, size);
			
			this->putq(mb);
		}
	}

	void TTcp::tcpNetThread_OnDataSendFailed(ACE_UINT32 ip, ACE_UINT16 port, unsigned int seq, const char* data, unsigned short size)
	{
		ACE_Message_Block* mb = 0;

		ACE_NEW_NORETURN(mb, ACE_Message_Block(sizeof(ACE_UINT32) + sizeof(ACE_UINT16) + TCP_PACK_HEADER_SIZE + size, TCP_DATA_SEND_FAILED));

		if (mb)
		{
			mb->copy((const char *)&ip, sizeof(ACE_UINT32));
			mb->copy((const char *)&port, sizeof(ACE_UINT16));
			mb->copy((const char *)&seq, sizeof(unsigned int));
			mb->copy((const char *)&size, sizeof(unsigned short));
			mb->copy(data, size);
			
			this->putq(mb);
		}
	}
} // namespace igame