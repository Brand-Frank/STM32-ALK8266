/********************************************************************
 *	TCP/UDP�����ݴ������
 * test_m8266wifi.c		��main.c�е���M8266WIFI_Test()����
 * @brief Source file of M8266WIFI testing application (M8266WIFI����Ӧ��Դ�ļ�)
 ********************************************************************/

#include "stdio.h"
#include "brd_cfg.h"
#include "M8266WIFIDrv.h"
#include "M8266HostIf.h"
#include "M8266WIFI_ops.h"
#include "led.h"

//#define USE_LED_AND_KEY_FOR_TEST	//ʹ��LED�Ͱ������в���,Ĭ�Ϲر�

void M8266WIFI_Test(void)
{
	u16 i;	//�ڿ���led��ʾ��for()��˸ʱ������ֵ
	u16 status = 0;	//���ӵ����õ�����
	u8  link_no = 0;	//ָ��ʧ��״̬��ָ��

	// ��Ƭ�������ϵ�����LED����˸4�Σ�������ʾ����ִ�е�����ķ�����ԡ���ģ��ͨ���޹أ��Ǳ��룬���ԡ�
	// ��û�ж����USE_LED_AND_KEY_FOR_TEST���˴����ᱻ���������
	#ifdef USE_LED_AND_KEY_FOR_TEST		//�Ǳ���
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
	//		1. �����׽���(socket)���ӵ�һЩ�� (�йز��Ժ����ӵļ�����)
	//----------------------------------------------------------------------------------
	/*		����ʹ�ó���
		����1.STM32F1-M8266WIFI
		����5.1.WiFiģ��APģʽ��PC��ΪSTAģʽ������ͨѶ������·������������1���ͻ��ˣ�
		����5.2WiFiģ��APģʽ��PC��ΪSTAģʽ������ͨѶ������·�����������Ӷ���ͻ��ˣ�
		����5.3.WiFiģ��STAģʽ��PC��ΪAPģʽ��PC���ȵ㡢����ͨѶ������·������������1���ͻ��ˣ�
		����5.4.WiFiģ��STAģʽ��TCP�ͻ��ˣ�---�ֻ���·������---PC��TCP��������
		����5.5.WiFiģ��˫��ͨѶ��WiFiģ��APģʽ��
	*/
	// ����ʱWiFiģ��Ĺ�����ʽ
	#define TEST_M8266WIFI_TYPE 1 //1=���ⲻͣ�ط������� | 2=��ͣ�ؽ������� | 3=�����յ������ݷ��͸����ͷ� | 4=��ͻ��˲���

	//����ʱWiFiģ��Ľ�ɫ(TCD/UDP-Server/Client)
	#define TEST_CONNECTION_TYPE 2	// 0 = WIFIģ����UDP | 1 = WIFIģ����TCP�ͻ��� | 2 = WIFIģ����TCP������

	/*	local port (�׽��ֵı��ض˿�)	*/
	#if (TEST_CONNECTION_TYPE == 1)	//ģ����ΪTCP�ͻ���
		// �׽��ֵı��ض˿�
		#define TEST_LOCAL_PORT 0	//��local port���ݵĲ���Ϊ0ʱ�����ض˿ڻ������������һ�����ģ�����ͻ��˷������ӷ�����ʱ�����á���Ϊ��������Ķ˿�ÿ�λ᲻һ�����Ӷ������������β���ͬ���ĵ�ַ�Ͷ˿�����ʱ���������ܾ���           

	#elif (TEST_CONNECTION_TYPE == 0) || (TEST_CONNECTION_TYPE == 2)
		// �׽��ֵ�Ŀ���ַ��Ŀ��˿�
		#define TEST_LOCAL_PORT 4321  //ģ����ΪUDP��TCP������������Ҫָ�����׽��ֵı��ض˿�

	#else
		#error WRONG TEST_CONNECTION_TYPE defined !
	#endif

		/*	remote port (�׽��ֵ�Զ��Ŀ���ַ��Ŀ��˿�)	*/
	#if (TEST_CONNECTION_TYPE == 0)	 //ģ����ΪUDP������ָ��Ŀ���ַ�Ͷ˿ڣ�Ҳ���������䣬�ڷ�������ʱ�����û����
		#define TEST_REMOTE_ADDR	"192.168.4.2"
		#define TEST_REMOTE_PORT 	1234

	#elif (TEST_CONNECTION_TYPE == 1)	//ģ����ΪTCP�ͻ��ˣ������ָ��Ŀ��ip��port����ģ����Ҫȥ���ӵ�TCP��������ip��port
		#define TEST_REMOTE_ADDR	"192.168.43.233"	// "www.baidu.com"
		#define TEST_REMOTE_PORT	1234	// 80

	#elif (TEST_CONNECTION_TYPE == 2) 	//ģ����ΪTCP������,����Ҫָ��Ŀ���ַ�Ͷ˿ڣ����������ֻ��һ����ʽ��䣬�����д��
		#define TEST_REMOTE_ADDR	"1.1.1.1"
		#define TEST_REMOTE_PORT	1234

	#else
		#error WRONG TEST_CONNECTION_TYPE defined !
	#endif
	// Note:���ϣ�WIFIģ����TCP�����������Ա��ض˿�Ϊ4321������Ҫָ��Ŀ���ַ�Ͷ˿ڣ�(ip:port)ֻ����伴��



	//----------------------------------------------------------------------------------
	//	 	2. �����׽��֣��Լ���Ҫʱ���׽��ֵ�һЩ����(ģ�齨��UDP��TCP)
	//----------------------------------------------------------------------------------	 

	// Note:����1�������TCP���͵��׽��֣��������õ������ڲ���	(step 0: config tcp windows number)
	#if ( 1 && ((TEST_CONNECTION_TYPE==1)||(TEST_CONNECTION_TYPE==2)) ) //�����ı��׽��ֵĴ����������Խ�#if����е�0�ĳ�1�����������Ҫ�ڴ����׽���֮ǰִ��
	if(M8266WIFI_SPI_Config_Tcp_Window_num(link_no, 4, &status)==0)
	{
		while(1) 
		{
			#ifdef USE_LED_AND_KEY_FOR_TEST	 // ����ʱ�� 1Hz ��Ƶ����˸
			LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(100);
			LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(100);
			#endif
		}
	}
	#endif

	// Note:����2�������׽�������
	if(M8266WIFI_SPI_Setup_Connection(TEST_CONNECTION_TYPE, TEST_LOCAL_PORT, TEST_REMOTE_ADDR, TEST_REMOTE_PORT, link_no, 20, &status)==0)
	{		
		while(1)
		{
			#ifdef USE_LED_AND_KEY_FOR_TEST	 // ��������׽���ʧ�ܣ��������ѭ����1Hz��˸�����ϵĵ�
			LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(500);
			LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(500);
			#endif
		}
	}
	else  // �����׽��ֳɹ����Ϳ��������׽���
	{
		// �������׽�����UDP����ô�������ó��鲥ģʽ�������Ҫ���ó��鲥�����Խ�#if����еĵ�һ��'0'�ĳ�'1'
		#if ( 0 && (TEST_CONNECTION_TYPE == 0) )	//(#1)
		//u8 M8266WIFI_SPI_Set_Multicuast_Group(u8 join_not_leave, char multicust_group_ip[15+1], u16* status)
		if(M8266WIFI_SPI_Op_Multicuast_Group(0, "224.6.6.6", &status)==0)
		{
			while(1)
			{
			#ifdef USE_LED_AND_KEY_FOR_TEST   // ���ʧ�ܣ��������ѭ������˸�����ϵĵ�
				LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(1000);
				LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(1000);
			#endif				 
			}
		}
		else	//!!! {} ???

		//  ���TCP����������ô�����������TCP������(��ʱ����ͨ�Ŷ�)�Ͽ��ͻ��˵ĳ�ʱʱ��
		//(If TCP server, then tcp server auto disconnection timeout, and max clients allowed could be set)
		#elif (TEST_CONNECTION_TYPE == 2)	//(#2)
		
			#if 1	//(#2.1a)	//(TEST_CONNECTION_TYPE == 2) == 1
			//�·�������ԭ�ͣ�u8 M8266WIFI_SPI_Set_TcpServer_Auto_Discon_Timeout(u8 link_no, u16 timeout_in_s, u16* status)	
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
		#endif	//(#3)		//Setup Connection successfully (�����׽������ӳɹ�)
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
	//	3. WIFI�׽��ֵ������շ�ͨ�Ų��� (TCP/UDP��Server/Clientͨ�Ų���)
	//��1��ģ��ֻ��Զ�˽ڵ������������ݰ���������ģ��ֻ���͵����ʣ�	Yes
	//��2��ģ��������������Զ�˽ڵ�����ݣ������Խ�ֻ���յ����ʣ�	No
	//��3��ģ��ȴ���������Զ�˽ڵ�����ݰ����ڽ��յ��������ݰ�ֱ�ӷ��ظ����ͷ���������ͬʱ�շ��ĸ������ʣ�No
	//��4����ͻ��˲��ԡ�	No
	//----------------------------------------------------------------------------------

	/*	1. ���Ͳ��ԣ����ϵ���Ŀ�귢������	*/
	#if (TEST_M8266WIFI_TYPE == 1) // Transmission Test: to test send data to remote constantly 
	{
		//���ʹ�ýϴ�����飬�ǵ�ȷ�����㹻���ϵͳ��ջ������������������. ���򣬵�Ƭ��������ܻ���Ϊ��ջ���Խ������롰hardware fault"ϵͳ�쳣
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
			//j += 2;	//�޸ĵ�for()�����
		}
		
		//����һ���������ݵ�������Ϊ�����͵����� (Generate an array of data to transmit) 
		// for(i=0; i<TEST_SEND_DATA_SIZE; i++) { snd_data[i]=i; }
		
		link_no = 0;
		
		for(batch = 0; ; batch++)
		{
			// below used to convenient reception end to differentiate packets when test and evaluate the reliability of transmission. Not necessary.
			// (�������仰���ڸ��췢�͵İ�������ǰ���������͵İ��᲻һ����������������Ŀɿ��Է���ʱ��������ն˷������ݡ��Ǳ��롣)
			
			/*	����ʼ�����ֽ��趨Ϊ��ͬ����ţ��Ա����ֲ�ͬ�İ�(first two bytes using batch to differentiate the continuos packets) 	*/
			//snd_data[0]= batch>>8;
			//snd_data[1]= batch&0xFF; 
			
			/*	�������������ֽ��趨Ϊһ��������ֽڣ��Է�����ն��������Ľ�β����������õ��� FF FF����Ϊ�����ķ�������ʱ˳������ı������ܳ���FF FF����������������յ���FF FF��Ȼ�����β��Ҳ������������ģʽƥ�䡣	*/
			/*	last byte using customized to label end of a packet for the sake of searching for data verification. Here FF FF as an example	*/ 
			//snd_data[TEST_SEND_DATA_SIZE-2]=0xFF;
			//snd_data[TEST_SEND_DATA_SIZE-1]=0xFF;

			if(total_sent> 1024*1024)  // watch MBytes*1024*1024+total_sent, which is the count of data module sends, compared with the received count at the reception end, to determin the packet loss etc
			{                          // (��������һ��ʱ��󣬹۲���ʽ MBytes*1024*1024+total_sent ��ֵ���ͽ��ն˽��յ������ݸ������бȽϣ����Դ��Ժ���ģ��Ķ����ʡ�)
				MBytes++;
				total_sent -= 1024*1024;
			}

			// ����M8266WIFI_SPI_Send_BlockData()�����ʹ������ 
			#if 1  //(#1)	//ǰ��#if (TEST_M8266WIFI_TYPE == 1)�����ж�
			{
				//u32 M8266WIFI_SPI_Send_BlockData(u8 Data[], u32 Data_len, u16 max_loops, u8 link_no, char* remote_ip, u16 remote_port, u16* status);
				
				// ������ЩTI/IAP�ṩ��ƽ̨������������MSP430, K60��TMS28335,�ȵȣ�ע�������Data_len������32λ�ģ����ԣ���ע�⣬����һ��������2^16�ĳ�ֵ�ĳ��Ȳ���ʱ��һ��Ҫ��ע��λu32������(u32)TEST_SEND_DATA_SIZE������ 2048UL ���򣬿��ܻ���ֲ������ݴ�λ�����Ρ�����������ֻ������TI��IAR��ĳЩ�����������¡�
				// PLEASE add (u32) to mandatorily convert a const to u32, or, the parameter transmission will be 16-bit and bring about function calling failure
				sent = M8266WIFI_SPI_Send_BlockData(snd_data, (u32)TEST_SEND_DATA_SIZE, 5000, link_no, NULL, 0, &status); 
				
				total_sent += sent;

				if( (sent==TEST_SEND_DATA_SIZE) && ((status&0xFF)==0x00) ) //���ͳɹ�
				{
					
				}
				else if( (status&0xFF) == 0x1E)	// 0x1E = too many errors encountered during sending and can not fixed, or transsmission blocked heavily(���ͽ׶�����̫��Ĵ���������ˣ����Կ��ǼӴ�max_loops)
				{
					debug_point = 1;
					//add some process here (�����ڴ˴���һЩ������������max_loops��ֵ)
				}
				/*	0x14 = connection of link_no not present (���׽��ֲ�����)
					0x15 = connection of link_no closed(���׽����Ѿ��رջ�Ͽ�)	*/
				else if( ((status&0xFF) == 0x14) || ((status&0xFF) == 0x15) )			
				{
					debug_point = 2;
					//need to re-establish the socket connection (��Ҫ�ؽ������׽���)
				}
				else if( (status&0xFF) == 0x18 )	// 0x18 = TCP server in listening states and no tcp clients have connected. (���TCP��������û�пͻ�����������)
				{
					debug_point = 3;
					M8266HostIf_delay_us(99);
				}
				else {
					debug_point = 4;
					M8266HostIf_delay_us(101);
				}
			}//end of #if 1
		
			// ����M8266WIFI_SPI_Send_Data() ��һ��һ�����ķ������ݣ�ʵ�ʼ�������M8266WIFI_SPI_Send_BlockData()�ĺ���ʵ�ִ���
			#else	//(#2)	//ǰ��#if (TEST_M8266WIFI_TYPE == 1)�����ж�
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
						/*	0x14 = connection of link_no not present (���׽��ֲ�����)
							0x15 = connection of link_no closed(���׽����Ѿ��رջ�Ͽ�)	*/
						if(   ((status&0xFF) == 0x14) || ((status&0xFF) == 0x15) )
						{
							M8266HostIf_delay_us(99);
							//need to re-establish the socket connection (��Ҫ�ؽ������׽���)
						}
						// 0x18 = TCP server in listening states and no tcp clients have connected. (���TCP��������û�пͻ�����������)
						else if( (status&0xFF) == 0x18 ) { M8266HostIf_delay_us(100); }
						else { M8266HostIf_delay_us(250); }
					}
				} // end of for(...		
				total_sent += sent;
			}//end of #else

			#endif	//(#3)	
		} //end of for()
	}


	/*	2. ���ղ��ԣ����ϵؽ�������Զ�˽ڵ㷢�͵�����	*/
	#elif (TEST_M8266WIFI_TYPE == 2)  // to test constant reception from remote
	{
		/*	���ʹ�ýϴ�����飬�ǵ�ȷ�����㹻���ϵͳ��ջ������������������. ���򣬵�Ƭ��������ܻ���Ϊ��ջ���Խ������롰hardware fault"ϵͳ�쳣	*/
		#define RECV_DATA_MAX_SIZE  2048
		u8  RecvData[RECV_DATA_MAX_SIZE];
		u16 received = 0;                   
		u32 total_received = 0;
		u32 MBytes = 0;
		
		for(i = 0; i < RECV_DATA_MAX_SIZE; i++)	{ RecvData[i] = 0xFF-i; }

		// (��Զ�˽ڵ㷢��һ�������Ա���Զ�˲鿴ģ���IP��ַ�Ͷ˿ڣ��Ǳ���) 
		M8266WIFI_SPI_Send_Data(RecvData, 1024, link_no, &status);	//ֻ�跢���Լ��TCP�������ϵ� IP ��ַ�� �Ǳ�Ҫ

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
						�ϴε��ý��պ���M8266WIFI_SPI_RecvData()ʱ����δ�������ϴ��Ǹ�����������ε��ü�������֮ǰ�İ���
						������������һЩ���������罫һ�ν��ջ����������󳤶����޼Ӵ�*/
					}
					else if( (status&0xFF)==0x24 )   
					{ 
						/*the packet is large in size than max_len specified and received only the max_len. 
						normally caused by the burst transmission by the routers after some block. 
						Suggest to stop the TCP transmission for some time.	do some work here if necessary
						ģ�������յ������ڱ���ȡ��������ĳ��ȣ������������max_len������ָ���ĳ��ȡ�
						ͨ������ΪԶ���󷢷��ͻ�·����������ʱ�����˴����ճ�����µ���ģ��İ�������
						����Զ��ʵ�ʷ��͵ľ���һ���������䳤�ȳ�����������ָ������󳤶����ޡ�
						�����ǰ�ߵ�ԭ�򣬽�����ͣԶ��TCPͨ��һ��ʱ�䡣
						����Ǻ��ߣ�����Ӵ�max_len����ֵ���߲����κδ�����������ʱ����Ƭ������߽��յ��ĳ����ᱻ��ɶ��С����Ҫ�����ٴ��ƽ⡣
						��Ҫʱ��������Ӧ�Ĵ���*/
					}
					else {	
						/*	do some work here if necessary(�����쳣����Ҫʱ������һЩ����)	*/ 
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


	/*	3. �շ����ԣ�ģ�齫���յ����������̷��ظ����ͷ�	*/
	#elif (TEST_M8266WIFI_TYPE == 3)  // Echo test: to receive data from remote and then echo back to remote
	{
		// ���ʹ�ýϴ�����飬�ǵ�ȷ�����㹻���ϵͳ��ջ������������������. ���򣬵�Ƭ��������ܻ���Ϊ��ջ���Խ������롰hardware fault"ϵͳ�쳣
		#define  RECV_DATA_MAX_SIZE  2048
		u8  RecvData[RECV_DATA_MAX_SIZE];
		u16 received = 0;
		u16 sent;
		
		for(i = 0; i < RECV_DATA_MAX_SIZE; i++)	{ RecvData[i] = i; }

		link_no = 0;
		sent = M8266WIFI_SPI_Send_Data(RecvData, 1024, link_no, &status);

		while(1)
		{
			if(M8266WIFI_SPI_Has_DataReceived()) // ������յ�����
			{
				//u16 M8266WIFI_SPI_RecvData(u8 data[], u16 max_len, uint16_t max_wait_in_ms, u8* link_no, u16* status);
				received = M8266WIFI_SPI_RecvData(RecvData, RECV_DATA_MAX_SIZE, 5*1000, &link_no, &status);

				if(received!=0) //�����Ƭ���յ������ݳ��Ȳ�����0�������յ�������
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
							/*	0x14 = connection of link_no not present (���׽��ֲ�����)
								0x15 = connection of link_no closed(���׽����Ѿ��رջ�Ͽ�) */
							if( ((status&0xFF) == 0x14) || ((status&0xFF) == 0x15) )
							{
								M8266HostIf_delay_us(99);
								//need to re-establish the socket connection (��Ҫ�ؽ������׽�������)
							}
							else if( (status&0xFF) == 0x18 )	{ M8266HostIf_delay_us(100); }	//0x18-TCP���������ڼ���״̬������û�пͻ�����������
							else {
								M8266HostIf_delay_us(250);
							}
						} // end of else
					} // end of for(...
				} // end of if(received!=0)
			}
		} // end of while(1)
	}// end of #elif


	/*	4. ��ͻ����շ����ԣ�ģ����ΪTCP��������UDP���������Զ��Զ�˽ڵ�����ݣ��������յ����������̷��ͻظ���Ӧ�ķ��ͷ�	*/
	#elif (TEST_M8266WIFI_TYPE == 4) // multi-clients transimission test: TCP Server or UDP receive data from multilple sender and echo back the data to the coresponding source

	// ǰ��3�ֲ��Ժ�������ģʽ����0��2���򱨴�#error
	#if ( (TEST_CONNECTION_TYPE!=0) && (TEST_CONNECTION_TYPE!=2) )
	#error multiple clients test could be supported only when socket connection is UDP or TCP Server
	#endif

	{	// ������(TEST_M8266WIFI_TYPE == 4)�Ĳ��Կ���
		#define  RECV_DATA_MAX_SIZE  2048
		/*	���ʹ�ýϴ�����飬�ǵ�ȷ�����㹻���ϵͳ��ջ������������������. ���򣬵�Ƭ��������ܻ���Ϊ��ջ���Խ������롰hardware fault"ϵͳ�쳣	*/
		/*	If using large size of array, ensure system stack is large enough for the array variable�� Or stack over-bound leakage might bring about the mcu into "hardware fault exception" 	*/
		u8  RecvData[RECV_DATA_MAX_SIZE];
		u16 received = 0;
		u16 sent;
		u8  remote_ip[4];
		u16 remote_port;
		
		for(i=0; i<RECV_DATA_MAX_SIZE; i++)	{ RecvData[i]=i; }

		link_no = 0;

		while(1)
		{
			if(M8266WIFI_SPI_Has_DataReceived()) // ������յ�����
			{
				/*	��Ƭ��ʹ�ú���M8266WIFI_SPI_RecvData_ex()���������ݣ�����������Է��ط��ͷ��ĵ�ַ�Ͷ˿�	*/
				//u16 M8266WIFI_SPI_RecvData_ex(u8 Data[], u16 max_len, uint16_t max_wait_in_ms, u8* link_no, u8 remote_ip[4], u16* remote_port, u16* status)
				received = M8266WIFI_SPI_RecvData_ex(RecvData, RECV_DATA_MAX_SIZE, 5*1000, &link_no, remote_ip, &remote_port, &status);

				if(received != 0)	//�����Ƭ���յ������ݳ��Ȳ�����0�������յ�������
				{
					// ��16���Ƹ�ʽ��ip��ַת���ַ�����ʽ��ip��ַ����Ϊ����ĺ������ݵ����ַ������Ƶĵ�ַ����
					char dest_addr[15+1]={0};
					sprintf(dest_addr, "%d.%d.%d.%d", remote_ip[0], remote_ip[1], remote_ip[2], remote_ip[3]);

					u16 tcp_packet_size = 1024;
					u16 loops     = 0;
					u16 max_loops = 5000;
					u32 len       = received; 
				
					for(sent=0, loops=0; (sent<len)&&(loops<=max_loops); loops++)
					{		
						#if ( TEST_CONNECTION_TYPE==0 )  //if UDP��UDPʵ�������Եȵ�UDP����������
							//�����UDP����Ƭ��ʹ�ú���M8266WIFI_SPI_Send_Udp_Data()���������ݣ�����������Դ���Ŀ���ַ�Ͷ˿ڣ�Ҳ���ǿ���ָ�����͵��ĸ��Եȵ�UDP����ڵ�)
							//u16 M8266WIFI_SPI_Send_Udp_Data(u8 Data[], u16 Data_len, u8 link_no, char* udp_dest_addr, u16 udp_dest_port, u16* status)
							sent += M8266WIFI_SPI_Send_Udp_Data(RecvData+sent, ((len-sent)>tcp_packet_size)?tcp_packet_size:(len-sent), link_no, dest_addr, remote_port, &status);
							
							if(sent>=len)  break;
							if((status&0xFF) == 0x00) { loops = 0; }
							else
							{
								if((status&0xFF) == 0x14)	// 0x14 = connection of link_no not present (���׽��ֲ�����)
								{
									M8266HostIf_delay_us(99);
									//need to re-establish the socket connection (��Ҫ�ؽ������׽���)
								}
								else { M8266HostIf_delay_us(250); }
							}         
						#elif ( TEST_CONNECTION_TYPE==2 )	//�����TCP����������Ƭ��ʹ�ú���M8266WIFI_SPI_Send_Data_to_TcpClient()���������ݣ�����������Դ���Ŀ���ַ�Ͷ˿ڣ�Ҳ���ǿ���ָ�����͵��ĸ��ͻ���
							//u16 M8266WIFI_SPI_Send_Data_to_TcpClient(u8 Data[], u16 Data_len, u8 server_link_no, char* tcp_client_dest_addr, u16 tcp_client_dest_port, u16* status)
							sent += M8266WIFI_SPI_Send_Data_to_TcpClient(RecvData+sent, ((len-sent)>tcp_packet_size)?tcp_packet_size:(len-sent), link_no, dest_addr, remote_port, &status);
							
							if(sent>=len)  break;
							if((status&0xFF) == 0x00) { loops = 0; }
							else
							{
								/*	0x14 = connection of link_no not present (���׽��ֲ�����)
									0x15 = connection of link_no closed(���׽����Ѿ��رջ�Ͽ�)	*/
								if( ((status&0xFF) == 0x14) || ((status&0xFF) == 0x15) )
								{
									M8266HostIf_delay_us(99);
									//need to re-establish the socket connection (��Ҫ�ؽ������׽���)
								}
								// 0x18 = TCP server in listening states and no tcp clients have connected. (���TCP��������û�пͻ�����������)
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
