#ifndef TcpHandlerH
#define TcpHandlerH

/**
* @defgroup TcpHandler
* @author igame
* @date 2009-2-5
* @version 1.0
* revision
* -
* @{
*/
#include <ace/OS.h>
#include <ace/SOCK_Stream.h>
#include <ace/INET_Addr.h>
#include <ace/Asynch_IO.h>
#include <ace/Message_Block.h>
#include <ace/Recursive_Thread_Mutex.h>
#include "TCPDef.h"
#include "TObject.h"
#include "TEvent.h"

namespace igame
{
	/**
	* @class TTcpHandler
	* @brief Tcp连接句柄
	*/
	class TTcpHandler : public ACE_Service_Handler
	{
	public:
		/**
		* @brief 客户端连接事件类型定义
		* @param [in] ACE_UINT32 客户端地址
		* @param [in] ACE_UINT16 客户端端口
		* @param [in] TTcpHandler* 连接句柄
		*/
		typedef TEvent<void, ACE_UINT32, ACE_UINT16, TTcpHandler *> TOnClientConnect;

		/**
		* @brief 客户端断开连接事件类型定义
		* @param [in] ACE_UINT32 客户端地址
		* @param [in] ACE_UINT16 客户端端口
		*/
		typedef TEvent<void, ACE_UINT32, ACE_UINT16> TOnClientDisconnect;

		/**
		* @brief 客户端连接验证事件
		* @param [in] ACE_UINT32 客户端地址
		* @param [in] ACE_UINT16 客户端端口
		* @return bool
		* - true 验证通过
		* - false 验证失败
		*/
		typedef TEvent<bool, ACE_UINT32, ACE_UINT16> TOnClientValidate;

		/**
		* @brief 接收到客户端数据事件类型定义
		* @param [in] ACE_UINT32 客户端地址
		* @param [in] ACE_UINT16 客户端端口
		* @param [in] unsigned int 数据包序列号
		* @param [in] const char* 数据区域指针
		* @param [in] size_t 数据长度
		*/
		typedef TEvent<void, ACE_UINT32, ACE_UINT16, unsigned int, const char*, unsigned short> TOnDataReceive;
		
		/**
		* @brief 成功发送客户端数据事件类型定义
		* @param [in] ACE_UINT32 客户端地址
		* @param [in] ACE_UINT16 客户端端口
		* @param [in] unsigned int 数据包序列号
		* @param [in] const char* 数据区域指针
		* @param [in] size_t 数据长度
		*/
		typedef TEvent<void, ACE_UINT32, ACE_UINT16, unsigned int, const char*, unsigned short> TOnDataSendSucceeded;
		
		/**
		* @brief 失败发送客户端数据事件类型定义
		* @param [in] ACE_UINT32 客户端地址
		* @param [in] ACE_UINT16 客户端端口
		* @param [in] unsigned int 数据包序列号
		* @param [in] const char* 数据区域指针
		* @param [in] size_t 数据长度
		*/
		typedef TEvent<void, ACE_UINT32, ACE_UINT16, unsigned int, const char*, unsigned short> TOnDataSendFailed;


	private:
		ACE_Asynch_Read_Stream m_Reader; //< 异步读数据流
		ACE_Asynch_Write_Stream m_Writer; //< 异步写数据流
		ACE_Message_Block* m_CurDataMB; //< 当前读取数据
		ACE_INET_Addr m_ClientAddr; //< 客户端地址

	public:
		/**
		* @name 事件句柄
		* @{
		*/
		DECL_PROP(TOnClientConnect, OnClientConnect) //< 客户端连接事件句柄
		DECL_PROP(TOnClientDisconnect, OnClientDisconnect) //< 客户端断开事件句柄
		DECL_PROP(TOnDataReceive, OnDataReceive) //< 接收到数据的事件句柄
		DECL_PROP(TOnDataSendSucceeded, OnDataSendSucceeded) //< 成功发送数据的事件句柄
		DECL_PROP(TOnDataSendFailed, OnDataSendFailed) //< 发送数据失败的事件句柄
		/**
		* @}
		*/
	public:
		/// ctor
		TTcpHandler();
		
		/// dtor
		~TTcpHandler();
		
		/**
		* @brief 发送数据
		* @param [in] unsigned int 数据包序列号
		* @param [in] const char* 要发送的数据区域指针
		* @param [in] size_t 要发送的数据长度
		* @return int
		* - 0 成功
		* - 1 失败
		*/
		int send(unsigned int seq, const char* data, unsigned short size);
		
		/**
		* @brief 打开句柄
		* @see ACE_Service_Handler
		*/
		virtual void open(ACE_HANDLE h, ACE_Message_Block& mb);
	
		/**
		* @brief 获取地址
		* @see ACE_Service_Handler
		*/
		virtual void addresses (const ACE_INET_Addr &remote_address,
							  const ACE_INET_Addr &local_address);
		
		/**
		* @brief 读取流数据事件
		* @see ACE_Service_Handler
		*/
		virtual void handle_read_stream(const ACE_Asynch_Read_Stream::Result& result);

		/**
		* @brief 写入流数据事件
		* @see ACE_Service_Handler
		*/
		virtual void handle_write_stream(const ACE_Asynch_Write_Stream::Result& result);

		/**
		* @brief 初始化当前数据接收缓冲事件
		*/
		void initCurDataMB();
	}; // class TTcpHandler

} // namespace

/**
* @}
*/
#endif // TcpHandlerH