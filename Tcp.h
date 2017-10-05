#ifndef TTcpH
#define TTcpH

/**
* @defgroup TTcp TCP包装
* @author igame
* @date 2009-2-5
* @version 1.0
* revision
* -
* @{
*/

#include <ace/Thread_Mutex.h>
#include "TcpHandler.h"
#include "TcpNetThread.h"

#include <utility>
#include <hash_map>
using namespace std;
using namespace stdext;

namespace igame
{
	/**
	* @class TTcp
	* @brief TCP接收和事件处理代理线程
	*/
	class TTcp : public TObject, public ACE_Task<ACE_MT_SYNCH>
	{
	public:
		/**
		* @name 重定义事件类型
		* @see TTcpHandler
		* @{
		*/
		typedef TTcpHandler::TOnClientConnect TOnClientConnect;
		typedef TTcpHandler::TOnClientDisconnect TOnClientDisconnect;
		typedef TTcpHandler::TOnClientValidate TOnClientValidate;
		typedef TTcpHandler::TOnDataReceive TOnDataReceive;
		typedef TTcpHandler::TOnDataSendSucceeded TOnDataSendSucceeded;
		typedef TTcpHandler::TOnDataSendFailed TOnDataSendFailed;
		/**
		* @}
		*/
	private:
		/**
		* @name 成员变量
		* @{
		*/
		ACE_Recursive_Thread_Mutex m_Lock; //< 线程锁
		hash_map<unsigned __int64, TTcpHandler *> m_AddrMap; //< 地址/句柄映射
		TTcpNetThread* m_TcpNetThd;
		/**
		* @}
		*/
	public:
		/**
		* @name TCP事件句柄
		* @see TTcpHandler
		* @{
		*/
		DECL_PROP(TTcpHandler::TOnClientConnect, OnClientConnect)
		DECL_PROP(TTcpHandler::TOnClientDisconnect, OnClientDisconnect)
		DECL_PROP(TTcpHandler::TOnClientValidate, OnClientValidate)
		DECL_PROP(TTcpHandler::TOnDataReceive, OnDataReceive)
		DECL_PROP(TTcpHandler::TOnDataSendSucceeded, OnDataSendSucceeded)
		DECL_PROP(TTcpHandler::TOnDataSendFailed, OnDataSendFailed)
		/**
		* @}
		*/

	public:
		/// ctor
		TTcp();

		/// dtor
		~TTcp();

		/// 运行
		void open();

		/// 停止
		void close();
		
		/// 发送数据
		int send(ACE_UINT32 ip, ACE_UINT16 port, unsigned int seq, const char* buf, unsigned short len);

	private:
		/// 线程函数
		virtual int svc();

	private:
		/**
		* @name TTcpNetThread 事件处理方法
		* 关于事件原型的定义，请参考 @see TTcpHandler
		* @{
		*/
		void tcpNetThread_OnClientConnect(ACE_UINT32 ip, ACE_UINT16 port, TTcpHandler* handler);

		void tcpNetThread_OnClientDisconnect(ACE_UINT32 ip, ACE_UINT16 port);

		void tcpNetThread_OnDataReceive(ACE_UINT32 ip, ACE_UINT16 port, unsigned int seq, const char* data, unsigned short size);

		void tcpNetThread_OnDataSendSucceeded(ACE_UINT32 ip, ACE_UINT16 port, unsigned int seq, const char* data, unsigned short size);

		void tcpNetThread_OnDataSendFailed(ACE_UINT32 ip, ACE_UINT16 port, unsigned int seq, const char* data, unsigned short size);
		/**
		* @}
		*/
	}; // class TTcp


} // namespace

/**
* @}
*/
#endif // TTcpH