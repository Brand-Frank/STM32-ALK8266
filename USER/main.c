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
		
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�����ж����ȼ� Group2
	delay_init();  	// ��ʼ����ʱ����
	
	#ifdef USE_LED_AND_KEY_FOR_TEST
		LED_Init();		// ��ʼ�� LEDs
		KEY_Init();		// ��ʼ�� KEYs
	#endif
	
	//--------------------------------------------------------------------------------------------------
	//	1. ��ʼ����Ƭ��������WIFIģ��������ӿڣ�	���� SPI��ʼ�� �� nRESET �� nCS��Ӧ�� GPIO ��ʼ��
	//--------------------------------------------------------------------------------------------------
	M8266HostIf_Init();		//TODO: "M8266HostIf_STM32xx.c"
	
	#ifdef USE_LED_AND_KEY_FOR_TEST
	{
		u8 i;	
		for(i=0; i<5; i++)	//���� LED ͬʱ��˸ 5 �Σ���ʾ��һ������ʼ�� M8266WIFI ģ��
		{
			LED_set(0, 0); LED_set(1, 0);  M8266WIFI_Module_delay_ms(50);	//�ӳ�50ms
			LED_set(0, 1); LED_set(1, 1);  M8266WIFI_Module_delay_ms(50);				
		}
	}
	#endif

	//--------------------------------------------------------------------------------------------------
	//	2. ����WIFIģ��	| - Ӳ��λWIFIģ�� | - ģ��ѡ�� | - ����ģʽ������ȷ�������ɹ� | - ������Ҫ�Ĳ���
	//--------------------------------------------------------------------------------------------------
	success = M8266WIFI_Module_Init_Via_SPI(); //TODO: "M8266WIFI_Ops.c" ����APģʽ/����/�ź����õ�
	
	if(success)	// ����������LED������˸3�α�ʾM8266WIFI��ʼ���ɹ�
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
	else // ��� M8266WIFI ģ���ʼ��ʧ�ܣ����� LED �� 2Hz ��Ƶ�ʳ�����˸
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
	//	3. ���� UDP�� TCP���ӷ���(�����׽�������)
	//     ͨ�� SPI �ӿ��շ��������ݰ�(ͨ��ģ�����ͨ��)
	//--------------------------------------------------------------------------------------------------
	M8266WIFI_Test(); //TODO: (����������������,TCP/UDP/�շ����Ե�,��"test_m8266wifi.c"��)

	while(1)
	{
		#ifdef USE_LED_AND_KEY_FOR_TEST					
			LED_set(0, 1); LED_set(1, 1);  M8266WIFI_Module_delay_ms(1000);
			LED_set(0, 0); LED_set(1, 0);  M8266WIFI_Module_delay_ms(1000);
		#endif
	}
} //end of main 
