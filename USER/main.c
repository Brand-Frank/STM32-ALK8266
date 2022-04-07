#include "stdio.h"
#include "sys.h"
#include "delay.h"
#include "misc.h"
#include "led.h"
#include "key.h"
#include "brd_cfg.h"
#include "M8266WIFIDrv.h"
#include "M8266WIFI_ops.h"
#include "M8266HostIf.h"

void M8266WIFI_Test(void);

int main(void)
{ 
	u8 success=0;
		
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置中断优先级 Group2
	delay_init();  	// 初始化延时函数
	
	#ifdef USE_LED_AND_KEY_FOR_TEST
		LED_Init();		// 初始化 LEDs
		KEY_Init();		// 初始化 KEYs
	#endif
	
	//--------------------------------------------------------------------------------------------------
	//	1. 初始化单片机主机和WIFI模组的主机接口，	包括 SPI初始化 和 nRESET 和 nCS对应的 GPIO 初始化
	//--------------------------------------------------------------------------------------------------
	M8266HostIf_Init();		//TODO: "M8266HostIf_STM32xx.c"
	
	#ifdef USE_LED_AND_KEY_FOR_TEST
	{
		u8 i;	
		for(i=0; i<5; i++)	//两个 LED 同时闪烁 5 次，表示下一步将初始化 M8266WIFI 模块
		{
			LED_set(0, 0); LED_set(1, 0);  M8266WIFI_Module_delay_ms(50);	//延迟50ms
			LED_set(0, 1); LED_set(1, 1);  M8266WIFI_Module_delay_ms(50);				
		}
	}
	#endif

	//--------------------------------------------------------------------------------------------------
	//	2. 配置WIFI模组	| - 硬复位WIFI模组 | - 模块选择 | - 设置模式和配网确保连网成功 | - 其他必要的操作
	//--------------------------------------------------------------------------------------------------
	success = M8266WIFI_Module_Init_Via_SPI(); //TODO: "M8266WIFI_Ops.c" 配置AP模式/配网/信号设置等
	
	if(success)	// 主板上两个LED交替闪烁3次表示M8266WIFI初始化成功
	{
		#ifdef USE_LED_AND_KEY_FOR_TEST			
		{
			for(i=0; i<3; i++)
			{
				LED_set(0, 1); LED_set(1, 0);  M8266WIFI_Module_delay_ms(100);
				LED_set(0, 0); LED_set(1, 1);  M8266WIFI_Module_delay_ms(100);
			}
			LED_set(0, 1); LED_set(1, 1);
		}
		#endif
	}
	else // 如果 M8266WIFI 模块初始化失败，两个 LED 以 2Hz 的频率持续闪烁
	{
		while(1)
		{
			#ifdef USE_LED_AND_KEY_FOR_TEST					
				LED_set(0, 1); LED_set(1, 1);  M8266WIFI_Module_delay_ms(250);
				LED_set(0, 0); LED_set(1, 0);  M8266WIFI_Module_delay_ms(250);
			#endif
		}
	}

	//--------------------------------------------------------------------------------------------------
	//	3. 建立 UDP或 TCP链接服务(建立套接字连接)
	//     通过 SPI 接口收发网络数据包(通过模组高速通信)
	//--------------------------------------------------------------------------------------------------
	M8266WIFI_Test(); //TODO: (用于输出传输的配置,TCP/UDP/收发测试等,在"test_m8266wifi.c"中)

	while(1)
	{
		#ifdef USE_LED_AND_KEY_FOR_TEST					
			LED_set(0, 1); LED_set(1, 1);  M8266WIFI_Module_delay_ms(1000);
			LED_set(0, 0); LED_set(1, 0);  M8266WIFI_Module_delay_ms(1000);
		#endif
	}
} //end of main 
