#ifndef TTcpnetThreadH
#define TTcpnetThreadH

/**
* @defgroup Tcp网络线程
* @author igame
* @date 2009-2-5
* @version 1.0
* revision
* -
* @{
*/
#include <ace/OS.h>
#include <ace/Task.h>

#include "TcpAcceptor.h"

namespace igame
{
	/**
	* @class TTcpNetThread
	* @brief TCP网络线程
	* @see ACE_Task_Base
	* @see ACE_Proactor
	*/
	class TTcpNetThread : public ACE_Task_Base
	{
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

		/// 运行
		int open();

		/// 停止运行
		int close();
	protected:
		/// 线程函数
		virtual int svc();
	};

} // namespace

/**
* @}
*/ // defgroup

#endif // TTcpnetThreadH