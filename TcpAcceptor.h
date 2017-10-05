#ifndef TTCPAcceptorH
#define TTCPAcceptorH

/**
* @defgroup TcpAcceptor
* @author igame
* @date 2009-2-5
* @version 1.0
* revision
* -
* @{
*/

#include <ace/Asynch_Acceptor.h>
#include "TcpHandler.h"

namespace igame
{
	/**
	* @class TTcpAcceptor
	* @brief TCP接受器
	* @see ACE_Asynch_Acceptor
	* @see TTcpHandler
	*/
	class TTcpAcceptor : public ACE_Asynch_Acceptor<TTcpHandler>
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
	protected:
		/**
		* @brief 连接验证
		* @note 激发 OnClientValidate 事件 @see TOnClientValidate
		* @see ACE_Asynch_Acceptor
		*/
		virtual int validate_connection (const ACE_Asynch_Accept::Result& result,
								   const ACE_INET_Addr &remote,
								   const ACE_INET_Addr& local);
		/**
		* @brief 创建连接句柄事件
		* @see ACE_Asynch_Acceptor
		*/
		virtual TTcpHandler* make_handler(void);
	}; // class TTcpAcceptor
} // namespace igame

/**
* @}
*/
#endif // TTCPAcceptorH