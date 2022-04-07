/********************************************************************
 *	TCP/UDP等数据传输测试
 * test_m8266wifi.c		在main.c中调用M8266WIFI_Test()函数
 * @brief Source file of M8266WIFI testing application (M8266WIFI测试应用源文件)
 ********************************************************************/

#include "stdio.h"
#include "brd_cfg.h"
#include "M8266WIFIDrv.h"
#include "M8266HostIf.h"
#include "M8266WIFI_ops.h"
#include "led.h"

//#define USE_LED_AND_KEY_FOR_TEST	//使用LED和按键进行测试,默认关闭

void M8266WIFI_Test(void)
{
	u16 i;	//在开启led显示的for()闪烁时的索引值
	u16 status = 0;	//链接到配置的数量
	u8  link_no = 0;	//指向失败状态的指针

	// 单片机主板上的两颗LED灯闪烁4次，用于提示代码执行到这里的方便调试。和模组通信无关，非必须，可略。
	// 若没有定义宏USE_LED_AND_KEY_FOR_TEST，此处不会被编译进来。
	#ifdef USE_LED_AND_KEY_FOR_TEST		//非必须
	{
		for(i=0; i<3; i++)
		{                    
			LED_set(0, 0); LED_set(1, 1);  M8266WIFI_Module_delay_ms(100);
			LED_set(0, 1); LED_set(1, 0);  M8266WIFI_Module_delay_ms(100);
			LED_set(0, 0); LED_set(1, 1);  M8266WIFI_Module_delay_ms(100);
			LED_set(0, 1); LED_set(1, 0);  M8266WIFI_Module_delay_ms(100);
		}
		LED_set(0, 1); LED_set(1, 1);
	}
	#endif	 
	//



	//----------------------------------------------------------------------------------
	//		1. 创建套接字(socket)连接的一些宏 (有关测试和链接的几个宏)
	//----------------------------------------------------------------------------------
	/*		测试使用程序
		├─1.STM32F1-M8266WIFI
		├─5.1.WiFi模块AP模式，PC作为STA模式（单向通讯、单链路、服务器链接1个客户端）
		├─5.2WiFi模块AP模式，PC作为STA模式（单向通讯、单链路、服务器链接多个客户端）
		├─5.3.WiFi模块STA模式，PC作为AP模式（PC开热点、单向通讯、单链路、服务器链接1个客户端）
		├─5.4.WiFi模块STA模式（TCP客户端）---手机（路由器）---PC（TCP服务器）
		└─5.5.WiFi模块双向通讯（WiFi模块AP模式）
	*/
	// 测试时WiFi模块的工作方式
	#define TEST_M8266WIFI_TYPE 1 //1=向外不停地发送数据 | 2=不停地接收数据 | 3=将接收到的数据发送给发送方 | 4=多客户端测试

	//测试时WiFi模块的角色(TCD/UDP-Server/Client)
	#define TEST_CONNECTION_TYPE 2	// 0 = WIFI模组做UDP | 1 = WIFI模组做TCP客户端 | 2 = WIFI模组做TCP服务器

	/*	local port (套接字的本地端口)	*/
	#if (TEST_CONNECTION_TYPE == 1)	//模组作为TCP客户端
		// 套接字的本地端口
		#define TEST_LOCAL_PORT 0	//当local port传递的参数为0时，本地端口会随机产生。这一点对于模组做客户端反复连接服务器时很有用。因为随机产生的端口每次会不一样，从而避免连续两次采用同样的地址和端口链接时被服务器拒绝。           

	#elif (TEST_CONNECTION_TYPE == 0) || (TEST_CONNECTION_TYPE == 2)
		// 套接字的目标地址和目标端口
		#define TEST_LOCAL_PORT 4321  //模组作为UDP或TCP服务器，则需要指定该套接字的本地端口

	#else
		#error WRONG TEST_CONNECTION_TYPE defined !
	#endif

		/*	remote port (套接字的远程目标地址和目标端口)	*/
	#if (TEST_CONNECTION_TYPE == 0)	 //模组作为UDP，可以指定目标地址和端口，也可以随便填充，在发送数据时再设置或更改
		#define TEST_REMOTE_ADDR	"192.168.4.2"
		#define TEST_REMOTE_PORT 	1234

	#elif (TEST_CONNECTION_TYPE == 1)	//模组作为TCP客户端，则必须指定目标ip和port，即模组所要去连接的TCP服务器的ip和port
		#define TEST_REMOTE_ADDR	"192.168.43.233"	// "www.baidu.com"
		#define TEST_REMOTE_PORT	1234	// 80

	#elif (TEST_CONNECTION_TYPE == 2) 	//模组作为TCP服务器,不需要指定目标地址和端口，这里的数据只是一个格式填充，随便填写。
		#define TEST_REMOTE_ADDR	"1.1.1.1"
		#define TEST_REMOTE_PORT	1234

	#else
		#error WRONG TEST_CONNECTION_TYPE defined !
	#endif
	// Note:以上：WIFI模组做TCP服务器，测试本地端口为4321，不需要指定目标地址和端口，(ip:port)只需填充即可



	//----------------------------------------------------------------------------------
	//	 	2. 创建套接字，以及必要时对套接字的一些配置(模组建立UDP或TCP)
	//----------------------------------------------------------------------------------	 

	// Note:步骤1：如果是TCP类型的套接字，可以配置调整窗口参数	(step 0: config tcp windows number)
	#if ( 1 && ((TEST_CONNECTION_TYPE==1)||(TEST_CONNECTION_TYPE==2)) ) //如果想改变套接字的窗口数，可以将#if语句中的0改成1，这个配置需要在创建套接字之前执行
	if(M8266WIFI_SPI_Config_Tcp_Window_num(link_no, 4, &status)==0)
	{
		while(1) 
		{
			#ifdef USE_LED_AND_KEY_FOR_TEST	 // 错误时以 1Hz 的频率闪烁
			LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(100);
			LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(100);
			#endif
		}
	}
	#endif

	// Note:步骤2：创建套接字连接
	if(M8266WIFI_SPI_Setup_Connection(TEST_CONNECTION_TYPE, TEST_LOCAL_PORT, TEST_REMOTE_ADDR, TEST_REMOTE_PORT, link_no, 20, &status)==0)
	{		
		while(1)
		{
			#ifdef USE_LED_AND_KEY_FOR_TEST	 // 如果创建套接字失败，则进入死循环和1Hz闪烁主板上的灯
			LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(500);
			LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(500);
			#endif
		}
	}
	else  // 创建套接字成功，就可以配置套接字
	{
		// 如果这个套接字是UDP，那么可以配置成组播模式。如果需要配置成组播，可以将#if语句中的第一个'0'改成'1'
		#if ( 0 && (TEST_CONNECTION_TYPE == 0) )	//(#1)
		//u8 M8266WIFI_SPI_Set_Multicuast_Group(u8 join_not_leave, char multicust_group_ip[15+1], u16* status)
		if(M8266WIFI_SPI_Op_Multicuast_Group(0, "224.6.6.6", &status)==0)
		{
			while(1)
			{
			#ifdef USE_LED_AND_KEY_FOR_TEST   // 如果失败，则进入死循环和闪烁主板上的灯
				LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(1000);
				LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(1000);
			#endif				 
			}
		}
		else	//!!! {} ???

		//  如果TCP服务器，那么可以设置这个TCP服务器(因长时间无通信而)断开客户端的超时时间
		//(If TCP server, then tcp server auto disconnection timeout, and max clients allowed could be set)
		#elif (TEST_CONNECTION_TYPE == 2)	//(#2)
		
			#if 1	//(#2.1a)	//(TEST_CONNECTION_TYPE == 2) == 1
			//下方函数的原型：u8 M8266WIFI_SPI_Set_TcpServer_Auto_Discon_Timeout(u8 link_no, u16 timeout_in_s, u16* status)	
			//(#IF)
			if( M8266WIFI_SPI_Set_TcpServer_Auto_Discon_Timeout(link_no, 120, &status) == 0)
			{
				while(1)
				{
					#ifdef USE_LED_AND_KEY_FOR_TEST	// led flash in 1Hz when error
					LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(1000);
					LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(1000);
					#endif		 
				}
			}
			#endif	//(#2.1b)
	
			#if 0	//(#2.2a)	//(TEST_CONNECTION_TYPE == 2) == 0
			//u8 M8266WIFI_SPI_Config_Max_Clients_Allowed_To_A_Tcp_Server(u8 server_link_no, u8 max_allowed, u16* status);
			else if( M8266WIFI_SPI_Config_Max_Clients_Allowed_To_A_Tcp_Server(link_no, 5, &status)==0)
			{
				while(1)
				{
					#ifdef USE_LED_AND_KEY_FOR_TEST	// led flash in 1Hz when error
					LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(1000);
					LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(1000);
					#endif		 
				}
			}
			#endif	//(#2.2b)

			else	//(#ELSE)
		#endif	//(#3)		//Setup Connection successfully (设置套接字链接成功)
		{
			#ifdef USE_LED_AND_KEY_FOR_TEST			 
		   	// led 1 flash 4 times upon success 
		   	LED_set(1, 0); M8266WIFI_Module_delay_ms(50); LED_set(1, 1); M8266WIFI_Module_delay_ms(50);
		   	LED_set(1, 0); M8266WIFI_Module_delay_ms(50); LED_set(1, 1); M8266WIFI_Module_delay_ms(50);
		   	LED_set(1, 0); M8266WIFI_Module_delay_ms(50); LED_set(1, 1); M8266WIFI_Module_delay_ms(50);
		   	LED_set(1, 0); M8266WIFI_Module_delay_ms(50); LED_set(1, 1); M8266WIFI_Module_delay_ms(50);
			#endif			 
		}
	}



	//----------------------------------------------------------------------------------
	//	3. WIFI套接字的数据收发通信测试 (TCP/UDP的Server/Client通信测试)
	//（1）模组只向远端节点连续发送数据包，来测试模组只发送的速率；	Yes
	//（2）模组连续接收来自远端节点的数据，来测试接只接收的速率；	No
	//（3）模组等待接收来自远端节点的数据包，在接收到将该数据包直接返回给发送方，来测试同时收发的各自速率；No
	//（4）多客户端测试。	No
	//----------------------------------------------------------------------------------

	/*	1. 发送测试，不断地向目标发送数据	*/
	#if (TEST_M8266WIFI_TYPE == 1) // Transmission Test: to test send data to remote constantly 
	{
		//如果使用较大的数组，记得确保有足够大的系统堆栈来容纳这个大数组变量. 否则，单片机程序可能会因为堆栈溢出越界而跳入“hardware fault"系统异常
		#define TEST_SEND_DATA_SIZE  200 //2920 //5840 //2048 // 1024		
		u8 snd_data[TEST_SEND_DATA_SIZE];
		volatile u32 sent = 0;
		volatile u32 total_sent = 0, MBytes = 0; 
		u16 batch;	 
		volatile u8 debug_point;
		u16 TEXT_Buffer[40]={0};
		int at=0;
		u16 i,j;

		for(i = 0; i < 40; i++)	{ TEXT_Buffer[i]=i; }

		for(j = 0; j < 40; j += 2)
		{
			snd_data[10*at+0]=(char)(((TEXT_Buffer[j])/1000)+48);
			snd_data[10*at+1]=(char)((((TEXT_Buffer[j])/100)%10)+48);
			snd_data[10*at+2]=(char)((((TEXT_Buffer[j])%100)/10)+48);
			snd_data[10*at+3]=(char)(((TEXT_Buffer[j])%10)+48);
			snd_data[10*at+4]=' ';
			snd_data[10*at+5]=(char)(((TEXT_Buffer[j+1])/1000)+48);
			snd_data[10*at+6]=(char)((((TEXT_Buffer[j+1])/100)%10)+48);
			snd_data[10*at+7]=(char)((((TEXT_Buffer[j+1])%100)/10)+48);
			snd_data[10*at+8]=(char)(((TEXT_Buffer[j+1])%10)+48);
			snd_data[10*at+9]=' ';
			at++;
			//j += 2;	//修改到for()里边了
		}
		
		//生成一个连续数据的数组作为待发送的数据 (Generate an array of data to transmit) 
		// for(i=0; i<TEST_SEND_DATA_SIZE; i++) { snd_data[i]=i; }
		
		link_no = 0;
		
		for(batch = 0; ; batch++)
		{
			// below used to convenient reception end to differentiate packets when test and evaluate the reliability of transmission. Not necessary.
			// (下面三句话是在改造发送的包，这样前后连续发送的包会不一样，这样在做传输的可靠性分析时，方便接收端分析数据。非必须。)
			
			/*	包开始两个字节设定为不同的序号，以便区分不同的包(first two bytes using batch to differentiate the continuos packets) 	*/
			//snd_data[0]= batch>>8;
			//snd_data[1]= batch&0xFF; 
			
			/*	包里的最后两个字节设定为一个特殊的字节，以方便接收端搜索包的结尾。这里举例用的是 FF FF。因为产生的发送数据时顺序递增的本身不可能出现FF FF，所以这样改造后收到的FF FF必然代表结尾。也可以用其他的模式匹配。	*/
			/*	last byte using customized to label end of a packet for the sake of searching for data verification. Here FF FF as an example	*/ 
			//snd_data[TEST_SEND_DATA_SIZE-2]=0xFF;
			//snd_data[TEST_SEND_DATA_SIZE-1]=0xFF;

			if(total_sent> 1024*1024)  // watch MBytes*1024*1024+total_sent, which is the count of data module sends, compared with the received count at the reception end, to determin the packet loss etc
			{                          // (持续发送一段时间后，观察表达式 MBytes*1024*1024+total_sent 的值，和接收端接收到的数据个数进行比较，可以粗略衡量模组的丢包率。)
				MBytes++;
				total_sent -= 1024*1024;
			}

			// 调用M8266WIFI_SPI_Send_BlockData()来发送大块数据 
			#if 1  //(#1)	//前面#if (TEST_M8266WIFI_TYPE == 1)的子判断
			{
				//u32 M8266WIFI_SPI_Send_BlockData(u8 Data[], u32 Data_len, u16 max_loops, u8 link_no, char* remote_ip, u16 remote_port, u16* status);
				
				// 对于那些TI/IAP提供的平台编译器，例如MSP430, K60，TMS28335,等等，注意这里的Data_len参数是32位的，所以，请注意，传递一个不超过2^16的常值的长度参数时，一定要标注其位u32，比如(u32)TEST_SEND_DATA_SIZE，或者 2048UL 否则，可能会出现参数传递错位的情形。这个问题可能只存在于TI或IAR的某些编译器环境下。
				// PLEASE add (u32) to mandatorily convert a const to u32, or, the parameter transmission will be 16-bit and bring about function calling failure
				sent = M8266WIFI_SPI_Send_BlockData(snd_data, (u32)TEST_SEND_DATA_SIZE, 5000, link_no, NULL, 0, &status); 
				
				total_sent += sent;

				if( (sent==TEST_SEND_DATA_SIZE) && ((status&0xFF)==0x00) ) //发送成功
				{
					
				}
				else if( (status&0xFF) == 0x1E)	// 0x1E = too many errors encountered during sending and can not fixed, or transsmission blocked heavily(发送阶段遇到太多的错误或阻塞了，可以考虑加大max_loops)
				{
					debug_point = 1;
					//add some process here (可以在此处加一些处理，比如增加max_loops的值)
				}
				/*	0x14 = connection of link_no not present (该套接字不存在)
					0x15 = connection of link_no closed(该套接字已经关闭或断开)	*/
				else if( ((status&0xFF) == 0x14) || ((status&0xFF) == 0x15) )			
				{
					debug_point = 2;
					//need to re-establish the socket connection (需要重建建立套接字)
				}
				else if( (status&0xFF) == 0x18 )	// 0x18 = TCP server in listening states and no tcp clients have connected. (这个TCP服务器还没有客户端连接着它)
				{
					debug_point = 3;
					M8266HostIf_delay_us(99);
				}
				else {
					debug_point = 4;
					M8266HostIf_delay_us(101);
				}
			}//end of #if 1
		
			// 调用M8266WIFI_SPI_Send_Data() 来一个一个包的发送数据，实际几乎就是M8266WIFI_SPI_Send_BlockData()的函数实现代码
			#else	//(#2)	//前面#if (TEST_M8266WIFI_TYPE == 1)的子判断
			{
				u16 tcp_packet_size = 1024;
				u16 loops     = 0;
				u16 max_loops = 5000;
				u32 len       = TEST_SEND_DATA_SIZE; 
					
				for(sent=0, loops=0; (sent<len)&&(loops<=max_loops); loops++)
				{		
					sent += M8266WIFI_SPI_Send_Data(snd_data+sent, ((len-sent)>tcp_packet_size)?tcp_packet_size:(len-sent), link_no, &status);
					if(sent>=len)  break;
					if((status&0xFF) == 0x00) { loops = 0; }
					else
					{
						/*	0x14 = connection of link_no not present (该套接字不存在)
							0x15 = connection of link_no closed(该套接字已经关闭或断开)	*/
						if(   ((status&0xFF) == 0x14) || ((status&0xFF) == 0x15) )
						{
							M8266HostIf_delay_us(99);
							//need to re-establish the socket connection (需要重建建立套接字)
						}
						// 0x18 = TCP server in listening states and no tcp clients have connected. (这个TCP服务器还没有客户端连接着它)
						else if( (status&0xFF) == 0x18 ) { M8266HostIf_delay_us(100); }
						else { M8266HostIf_delay_us(250); }
					}
				} // end of for(...		
				total_sent += sent;
			}//end of #else

			#endif	//(#3)	
		} //end of for()
	}


	/*	2. 接收测试，不断地接收来自远端节点发送的数据	*/
	#elif (TEST_M8266WIFI_TYPE == 2)  // to test constant reception from remote
	{
		/*	如果使用较大的数组，记得确保有足够大的系统堆栈来容纳这个大数组变量. 否则，单片机程序可能会因为堆栈溢出越界而跳入“hardware fault"系统异常	*/
		#define RECV_DATA_MAX_SIZE  2048
		u8  RecvData[RECV_DATA_MAX_SIZE];
		u16 received = 0;                   
		u32 total_received = 0;
		u32 MBytes = 0;
		
		for(i = 0; i < RECV_DATA_MAX_SIZE; i++)	{ RecvData[i] = 0xFF-i; }

		// (向远端节点发送一个包，以便在远端查看模组的IP地址和端口，非必须) 
		M8266WIFI_SPI_Send_Data(RecvData, 1024, link_no, &status);	//只需发送以检查TCP测试者上的 IP 地址。 非必要

		while(1)
		{
			if(M8266WIFI_SPI_Has_DataReceived())
			{
				//u16 M8266WIFI_SPI_RecvData(u8 data[], u16 max_len, uint16_t max_wait_in_ms, u8* link_no, u16* status);
				received = M8266WIFI_SPI_RecvData(RecvData, RECV_DATA_MAX_SIZE, 5*1000, &link_no, &status);

				if(  (status&0xFF)!= 0 )  
				{
					if( (status&0xFF)==0x22 )	// 0x22 = Module buffer has no data received
					{  
						M8266HostIf_delay_us(250); 
						//M8266WIFI_Module_delay_ms(1);
					}
					else if( (status&0xFF)==0x23 )   
					{ 
						/* the packet had not been received completed in the last call of M8266WIFI_SPI_RecvData()
						and has continued to be received in this call of M8266WIFI_SPI_RecvData()
						do some work here if necessary
						上次调用接收函数M8266WIFI_SPI_RecvData()时，并未收完整上次那个包，于是这次调用继续接受之前的包。
						可以在这里做一些工作，比如将一次接收缓冲区和做大长度上限加大。*/
					}
					else if( (status&0xFF)==0x24 )   
					{ 
						/*the packet is large in size than max_len specified and received only the max_len. 
						normally caused by the burst transmission by the routers after some block. 
						Suggest to stop the TCP transmission for some time.	do some work here if necessary
						模组所接收到而正在被读取的这个包的长度，超过了这里的max_len参数所指定的长度。
						通常是因为远端阵发发送或路由器等阻塞时出现了大面积粘包导致到达模块的包过长，
						或者远端实际发送的就是一个长包，其长度超过了这里所指定的最大长度上限。
						如果是前者的原因，建议暂停远端TCP通信一段时间。
						如果是后者，建议加大max_len的数值或者不做任何处理，不做处理时，单片机侧这边接收到的长包会被拆成多个小包需要自行再次破解。
						必要时可以做相应的处理。*/
					}
					else {	
						/*	do some work here if necessary(其他异常，必要时可以做一些处理。)	*/ 
					}
				}

				#define  TEST_RECV_UNIT (1024*1024)
				total_received += received;
				if( total_received >= (TEST_RECV_UNIT) )
				{
					LED_set(0, MBytes & 0x01);	
					total_received = total_received % (TEST_RECV_UNIT);
					MBytes++;
				}
			} // end of if(M8266WIFI_SPI_Has_DataReceived())				 
		} // end of while(1)
	} //end of #elif


	/*	3. 收发测试，模组将接收到的数据立刻返回给发送方	*/
	#elif (TEST_M8266WIFI_TYPE == 3)  // Echo test: to receive data from remote and then echo back to remote
	{
		// 如果使用较大的数组，记得确保有足够大的系统堆栈来容纳这个大数组变量. 否则，单片机程序可能会因为堆栈溢出越界而跳入“hardware fault"系统异常
		#define  RECV_DATA_MAX_SIZE  2048
		u8  RecvData[RECV_DATA_MAX_SIZE];
		u16 received = 0;
		u16 sent;
		
		for(i = 0; i < RECV_DATA_MAX_SIZE; i++)	{ RecvData[i] = i; }

		link_no = 0;
		sent = M8266WIFI_SPI_Send_Data(RecvData, 1024, link_no, &status);

		while(1)
		{
			if(M8266WIFI_SPI_Has_DataReceived()) // 如果接收到数据
			{
				//u16 M8266WIFI_SPI_RecvData(u8 data[], u16 max_len, uint16_t max_wait_in_ms, u8* link_no, u16* status);
				received = M8266WIFI_SPI_RecvData(RecvData, RECV_DATA_MAX_SIZE, 5*1000, &link_no, &status);

				if(received!=0) //如果单片机收到的数据长度不等于0，即接收到了数据
				{
					u16 tcp_packet_size = 1024;
					u16 loops     = 0;
					u16 max_loops = 5000;
					u32 len       = received; 
				
					for(sent=0, loops=0; (sent<len)&&(loops<=max_loops); loops++)
					{		
						sent += M8266WIFI_SPI_Send_Data(RecvData+sent, ((len-sent)>tcp_packet_size)?tcp_packet_size:(len-sent), link_no, &status);
						if(sent>=len)  break;
						if((status&0xFF) == 0x00)	{ loops = 0; }
						else
						{
							/*	0x14 = connection of link_no not present (该套接字不存在)
								0x15 = connection of link_no closed(该套接字已经关闭或断开) */
							if( ((status&0xFF) == 0x14) || ((status&0xFF) == 0x15) )
							{
								M8266HostIf_delay_us(99);
								//need to re-establish the socket connection (需要重建建立套接字连接)
							}
							else if( (status&0xFF) == 0x18 )	{ M8266HostIf_delay_us(100); }	//0x18-TCP服务器处于监听状态，并且没有客户端连接着它
							else {
								M8266HostIf_delay_us(250);
							}
						} // end of else
					} // end of for(...
				} // end of if(received!=0)
			}
		} // end of while(1)
	}// end of #elif


	/*	4. 多客户端收发测试：模组作为TCP服务器或UDP，接收来自多个远端节点的数据，并将接收到的数据立刻发送回给对应的发送方	*/
	#elif (TEST_M8266WIFI_TYPE == 4) // multi-clients transimission test: TCP Server or UDP receive data from multilple sender and echo back the data to the coresponding source

	// 前面3种测试后，若测试模式不是0或2，则报错#error
	#if ( (TEST_CONNECTION_TYPE!=0) && (TEST_CONNECTION_TYPE!=2) )
	#error multiple clients test could be supported only when socket connection is UDP or TCP Server
	#endif

	{	// 还处于(TEST_M8266WIFI_TYPE == 4)的测试块中
		#define  RECV_DATA_MAX_SIZE  2048
		/*	如果使用较大的数组，记得确保有足够大的系统堆栈来容纳这个大数组变量. 否则，单片机程序可能会因为堆栈溢出越界而跳入“hardware fault"系统异常	*/
		/*	If using large size of array, ensure system stack is large enough for the array variable。 Or stack over-bound leakage might bring about the mcu into "hardware fault exception" 	*/
		u8  RecvData[RECV_DATA_MAX_SIZE];
		u16 received = 0;
		u16 sent;
		u8  remote_ip[4];
		u16 remote_port;
		
		for(i=0; i<RECV_DATA_MAX_SIZE; i++)	{ RecvData[i]=i; }

		link_no = 0;

		while(1)
		{
			if(M8266WIFI_SPI_Has_DataReceived()) // 如果接收到数据
			{
				/*	单片机使用函数M8266WIFI_SPI_RecvData_ex()来接收数据，这个函数可以返回发送方的地址和端口	*/
				//u16 M8266WIFI_SPI_RecvData_ex(u8 Data[], u16 max_len, uint16_t max_wait_in_ms, u8* link_no, u8 remote_ip[4], u16* remote_port, u16* status)
				received = M8266WIFI_SPI_RecvData_ex(RecvData, RECV_DATA_MAX_SIZE, 5*1000, &link_no, remote_ip, &remote_port, &status);

				if(received != 0)	//如果单片机收到的数据长度不等于0，即接收到了数据
				{
					// 将16进制格式的ip地址转化字符串形式的ip地址，因为下面的函数传递的是字符串形似的地址参数
					char dest_addr[15+1]={0};
					sprintf(dest_addr, "%d.%d.%d.%d", remote_ip[0], remote_ip[1], remote_ip[2], remote_ip[3]);

					u16 tcp_packet_size = 1024;
					u16 loops     = 0;
					u16 max_loops = 5000;
					u32 len       = received; 
				
					for(sent=0, loops=0; (sent<len)&&(loops<=max_loops); loops++)
					{		
						#if ( TEST_CONNECTION_TYPE==0 )  //if UDP：UDP实现向多个对等的UDP服务发送数据
							//如果是UDP，单片机使用函数M8266WIFI_SPI_Send_Udp_Data()来发送数据，这个函数可以传递目标地址和端口，也就是可以指定发送到哪个对等的UDP服务节点)
							//u16 M8266WIFI_SPI_Send_Udp_Data(u8 Data[], u16 Data_len, u8 link_no, char* udp_dest_addr, u16 udp_dest_port, u16* status)
							sent += M8266WIFI_SPI_Send_Udp_Data(RecvData+sent, ((len-sent)>tcp_packet_size)?tcp_packet_size:(len-sent), link_no, dest_addr, remote_port, &status);
							
							if(sent>=len)  break;
							if((status&0xFF) == 0x00) { loops = 0; }
							else
							{
								if((status&0xFF) == 0x14)	// 0x14 = connection of link_no not present (该套接字不存在)
								{
									M8266HostIf_delay_us(99);
									//need to re-establish the socket connection (需要重建建立套接字)
								}
								else { M8266HostIf_delay_us(250); }
							}         
						#elif ( TEST_CONNECTION_TYPE==2 )	//如果是TCP服务器，单片机使用函数M8266WIFI_SPI_Send_Data_to_TcpClient()来发送数据，这个函数可以传递目标地址和端口，也就是可以指定发送到哪个客户端
							//u16 M8266WIFI_SPI_Send_Data_to_TcpClient(u8 Data[], u16 Data_len, u8 server_link_no, char* tcp_client_dest_addr, u16 tcp_client_dest_port, u16* status)
							sent += M8266WIFI_SPI_Send_Data_to_TcpClient(RecvData+sent, ((len-sent)>tcp_packet_size)?tcp_packet_size:(len-sent), link_no, dest_addr, remote_port, &status);
							
							if(sent>=len)  break;
							if((status&0xFF) == 0x00) { loops = 0; }
							else
							{
								/*	0x14 = connection of link_no not present (该套接字不存在)
									0x15 = connection of link_no closed(该套接字已经关闭或断开)	*/
								if( ((status&0xFF) == 0x14) || ((status&0xFF) == 0x15) )
								{
									M8266HostIf_delay_us(99);
									//need to re-establish the socket connection (需要重建建立套接字)
								}
								// 0x18 = TCP server in listening states and no tcp clients have connected. (这个TCP服务器还没有客户端连接着它)
								else if( (status&0xFF) == 0x18 ) 
								{ 
									M8266HostIf_delay_us(100);
								}
								else
								{ 
									M8266HostIf_delay_us(250); 
								}
							}
						#endif						
					} // for(...
				} // if(received!=0)
			}
		} // while(1)
	}	 

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	#else	//TEST_M8266WIFI_TYPE != 1 2 3 4
	#error NOT Supported Test Type! should be 1~4!		
	#endif

} // end of M8266WIFI_Test()
