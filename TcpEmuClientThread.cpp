#include "TcpEmuClientThread.h"

namespace igame
{
	

	TTcpClientThread::~TTcpClientThread()
	{
		for(int i = 0; i < CLIENT_CONNECTION_NUM_OF_PER_THREAD; i++)
			peerStream[i].close();
	}

	int TTcpClientThread::open() { return this->activate(); }

	int TTcpClientThread::close()
	{
		ACE_TRACE("TTcpClientThread::close");

		ACE_Message_Block* termBlock;
		
		ACE_NEW_NORETURN(termBlock, ACE_Message_Block(0, ACE_Message_Block::MB_HANGUP));

		if (!termBlock)
			ACE_DEBUG((LM_ERROR, ACE_TEXT("Allocate failed %i"), errno));
		else
		{
			putq(termBlock);

			wait();
		}

		return 0;
	}

	int TTcpClientThread::svc()
	{
		ACE_INET_Addr srvAddr(7878, "127.0.0.1");

		for(int i = 0; i < CLIENT_CONNECTION_NUM_OF_PER_THREAD; i++)
		{
			if (connector[i].connect(peerStream[i], srvAddr) == -1)
			{
				ACE_ERROR((LM_ERROR, ACE_TEXT("%i Failed to connect server errno=%i\n"), i, errno));
			}

			Sleep(100);
		}

		struct TPack
		{
#pragma pack(push)
#pragma pack(1)
			unsigned int seq;
			unsigned short len;
			char data [128];
#pragma pack(pop)
		};
		
		ACE_Message_Block* msg = 0;
		
		ACE_INET_Addr localAddr;
		ACE_TCHAR localAddrStr[128];

		peerStream[0].get_local_addr(localAddr);

		localAddr.addr_to_string(localAddrStr, sizeof(localAddrStr) / sizeof(ACE_TCHAR));
		
		TPack data;
		int len = sizeof(unsigned int) + sizeof(unsigned short);

		data.seq = 0;
		data.len = strlen(localAddrStr) + 1;
		strcpy(data.data, localAddrStr);

		len += data.len;

		char tmp[sizeof(TPack)];
		
		char buf[256];

		memcpy(tmp, &data, len);

		while(true) // Ïß³ÌÑ­»·
		{
			if (getq(msg) != -1)
			{
				switch(msg->msg_type())
				{
				case ACE_Message_Block::MB_HANGUP:
					{
						msg->release();

						return 0;
					}
					break;
				default:
					{
						for(int i = 0; i < CLIENT_CONNECTION_NUM_OF_PER_THREAD; i++)
						{
							peerStream[i].send(tmp, 5);
							Sleep(100);
							peerStream[i].send(tmp + 5, len - 5);
							Sleep(100);

							ACE_Time_Value timeout(2);
							
							int recvLen =  peerStream[i].recv_n(buf, sizeof(unsigned int) + sizeof(unsigned short), 0, &timeout);

							if (recvLen == sizeof(unsigned int) + sizeof(unsigned short))
							{
								short dataLen = *(short *)(buf + 4);

								if (dataLen > 256)
									dataLen = 256;

								recvLen = peerStream[i].recv_n(buf, dataLen, 0, &timeout);

								if (recvLen != dataLen)
									ACE_DEBUG((LM_INFO, ACE_TEXT("Failed to recv data, length is %i, but only get %i\n"), dataLen, recvLen));
								else
									ACE_DEBUG((LM_INFO, ACE_TEXT("Client get data: len=%i data=%s\n"), recvLen, buf));
							} // if recvLen
						} // for
					} // default
					break;
				} // switch

				msg->release();
			} // if getq
		} // while

		ACE_DEBUG((LM_INFO, ACE_TEXT("Exit client thread")));

		return 0;
	}
} // namespace