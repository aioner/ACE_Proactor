#ifndef TTCPDefH
#define TTCPDefH

/**
* @defgroup TCP用到的一些常量和参数
* @author igame
* @date 2009-1-5
* @version
* revision
* -
* @{
*/

namespace igame
{
	/**
	* @name TCP的ACE_Message_Block类型定义 @see ACE_Message_Block
	* @{
	*/
	
	/// @brief TCP数据接收
	#define TCP_DATA_RECEIVE			0x5505
	/// @brief TCP客户端连接
	#define TCP_CLIENT_CONNECT			0x5506
	/// @brief TCP客户端断线
	#define TCP_CLIENT_DISCONNECT		0x5507
	/// @brief TCP数据发送
	#define TCP_DATA_SEND				0x5508
	/// @brief TCP数据发送成功
	#define TCP_DATA_SEND_SUCCEEDED		0x5509
	/// @brief TCP数据发送失败
	#define TCP_DATA_SEND_FAILED		0x550A
	/**
	* @}
	*/

/// 默认监听地址:偶的车牌号
#define DEF_LISTENING_PORT	777

#pragma pack(push)
#pragma pack(1)
	/**
	* @brief Tcp包头结构
	*/
	typedef struct tag_TTcpPackHeader
	{
		unsigned int seq; //< 包序号
		unsigned short len; //< 包长度
	}TTcpPackHeader;
#pragma pack(pop)

/// 包头尺寸宏
#define TCP_PACK_HEADER_SIZE sizeof(tag_TTcpPackHeader)

} // namespace igame

/**
* @}
*/

#endif // TTCPDefH