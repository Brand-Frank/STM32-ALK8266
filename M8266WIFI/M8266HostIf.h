/********************************************************************
 *	����Ƭ�������ӿ��ļ���
 * ���ӿڣ���SPI����ʹ���ĸ�GPIO������ΪSPI���ź��ߣ�M8266HostIf_[�����ͺ�].c�ļ�������M8266HostIf_STM32xx.c)
 ********************************************************************/
#ifndef _M8266_HOST_IF_H_
#define _M8266_HOST_IF_H_

void M8266HostIf_Init(void);								// M8266HostIf_STM32xx.c
void M8266HostIf_SPI_SetSpeed(u32 SPI_BaudRatePrescaler);	// M8266HostIf_STM32xx.c

#endif
