#include "TcpUnitTest.h"

#include <iostream>
using namespace std;

using namespace igame;

/*
void client()
{
	// client
	{
		ACE_INET_Addr svrAddr(7878, "127.0.0.1");

		ACE_SOCK_Connector connector;
		ACE_SOCK_Stream peerStream;

		if (connector.connect(peerStream, svrAddr) == -1)
		{
			ACE_ERROR((LM_ERROR, ACE_TEXT("Failed to connect to server, errno=%i\n"), errno));
		}
		else
		{
			struct TPack
			{
#pragma push(pack)
#pragma pack(1)
				unsigned int seq;
				unsigned short len;
				char data [10];
#pragma pop(pack)
			};

			ACE_DEBUG((LM_INFO, ACE_TEXT("Sizeof TPack=%i\n"), sizeof(TPack)));

			ACE_DEBUG((LM_INFO, ACE_TEXT("Game starts\n")));
			
			{
				TPack data;

				data.seq = 0;
				data.len = 6;
				strcpy(data.data, "hello");

				char tmp[20];

				memcpy(tmp, &data, 12);

				peerStream.send(tmp, 5);
				Sleep(1000);
				peerStream.send(tmp + 5, 7);
			}
			{
				TPack pack;

				pack.len = 6;

				strcpy(pack.data, "hell1");

				char tmp[20];

				memcpy(tmp, &pack, 12);

				peerStream.send(tmp, 2);

				Sleep(1000);

				peerStream.send(tmp + 2, 10);

			}

			{
				TPack data1;
				TPack data2;

				data1.len = 6;
				data2.len = 6;

				strcpy(data1.data, "hellx");
				strcpy(data2.data, "helly");
				
				char tmp[40];

				memcpy(tmp, &data1, 12);
				memcpy(tmp + 12, &data2, 12);

				peerStream.send(tmp, 24);
			}

			peerStream.close();
		}

	}
}

*/

int ACE_TMAIN(int, ACE_TCHAR*[])
{
	TTcpUnitTest unitTest;
	
	unitTest.test(); // ≤‚ ‘

	return 0;
}