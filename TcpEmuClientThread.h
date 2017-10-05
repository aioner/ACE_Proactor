#ifndef TcpEmuClientThreadH
#define TcpEmuClientThreadH

#include <ace/OS.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>
#include <ace/Task.h>

namespace igame
{
	#define TCP_CLIENT_THREAD_SEND	0x777

	const int CLIENT_CONNECTION_NUM_OF_PER_THREAD = 1; //< 客户端每个线程的连接数
	/**
	* @class TTcpClientThread
	* @brief TCP客户端测试线程
	*/
	class TTcpClientThread : public ACE_Task<ACE_MT_SYNCH>
	{
		ACE_SOCK_Connector connector[CLIENT_CONNECTION_NUM_OF_PER_THREAD]; //< 连接器
		ACE_SOCK_Stream peerStream[CLIENT_CONNECTION_NUM_OF_PER_THREAD]; //< 流对象

	public:
		/// ctor
		~TTcpClientThread();
		
		/// 运行
		int open();

		/// 停止运行
		int close();
	private:
		/// 线程函数
		virtual int svc();
	};

} // namespace

#endif // TcpEmuClientThreadH