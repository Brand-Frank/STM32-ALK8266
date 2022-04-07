/********************************************************************
 *	《单片机主机接口文件》
 * （接口）：SPI具体使用哪个GPIO来复用为SPI的信号线，M8266HostIf_[主机型号].c文件（例如M8266HostIf_STM32xx.c)
 ********************************************************************/
#ifndef _M8266_HOST_IF_H_
#define _M8266_HOST_IF_H_

void M8266HostIf_Init(void);								// M8266HostIf_STM32xx.c
void M8266HostIf_SPI_SetSpeed(u32 SPI_BaudRatePrescaler);	// M8266HostIf_STM32xx.c

#endif
