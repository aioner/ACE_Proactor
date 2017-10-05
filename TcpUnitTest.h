#ifndef TcpUnitTestH
#define TcpUnitTestH

/**
* @defgroup 测试
* @author igame
* @date 2009-1-5
* @version 1.0
* revision
* -
* @{
*/

#include <ace/OS.h>
#include <ace/Task.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>
#include "TObject.h"
#include "Tcp.h"

namespace igame
{
	const int CLIENT_THREAD_NUM = 1; //< 模拟客户端线程数

	/**
	* @class TTcpUnitTest
	* @brief 简单的单元测试
	*/
	class TTcpUnitTest : public TObject // 要使用事件，就得从TObject继承
	{
		/**
		* @name 事件处理方法
		* @{
		*/
		void OnClientConnect(ACE_UINT32 ip, ACE_UINT16 port, TTcpHandler* handler);

		void OnClientDisconnect(ACE_UINT32 ip, ACE_UINT16 port);

		bool OnClientValidate(ACE_UINT32 ip, ACE_UINT16 port);

		void OnDataReceive(ACE_UINT32 ip, ACE_UINT16 port, unsigned int seq, const char* data, unsigned short size);

		void OnDataSendSucceeded(ACE_UINT32 ip, ACE_UINT16 port, unsigned int seq, const char* data, unsigned short size);

		void OnDataSendFailed(ACE_UINT32 ip, ACE_UINT16 port, unsigned int seq, const char* data, unsigned short size);
		/**
		* @}
		*/
	private:
		TTcp* m_Tcp; //< TCP对象
		
		
	public:
		/// ctor
		TTcpUnitTest();

		/// 测试方法
		int test();
	};
} // namespace igame

/**
* @}
*/

#endif // TcpUnitTestH