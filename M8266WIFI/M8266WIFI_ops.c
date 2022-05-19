/********************************************************************
 *	《操作WIFI模块--配置/操作ALK8266WIFI模块》
 * @brief  M8266WiFi HAL操作源文件
 ********************************************************************/
#include "stdio.h"
#include "string.h"
#include "brd_cfg.h"
#include "led.h"
#include "M8266WIFIDrv.h"
#include "M8266HostIf.h"
#include "M8266WIFI_ops.h"

/**
 * @brief WiFi模块毫秒级延时
 * @param nms n毫秒
 */
void M8266WIFI_Module_delay_ms(u16 nms)	//main.c(35)被调用
{
	 u16 i, j;
	 for(i = 0; i < nms; i++)
	   	for(j = 0; j < 4; j++)	//调用4次delay_us(250)来实现延迟1ms，因为M8266HostIf_delay_us(u8 nus)的形参是u8类型，nus最大255.
			M8266HostIf_delay_us(250);
}
//






//====================================================================================
//				模组的通信初始化部分 - M8266WIFI_Module_Init_Via_SPI()
//==================================================================================== 
// 调用：
// 		(1) M8266WIFI_Module_Hardware_Reset()
//		(2) M8266HostIf_SPI_Select()
//		(3) 配网
//		(4) M8266WIFI_SPI_wait_sta_connecting_to_ap_and_get_ip()
//		(5) M8266WIFI_SPI_Setup_Connection()

/***********************************************************************************************************
 * @brief 模组的硬复位
 * 			（1）通过 nReset 引脚对 M8266WIFI 模块执行硬件复位，并使 M8266WIFI 模块从外部 SPI 闪存启动
 * 			（2）为了确保M8266WIFI 模块从外部 SPI 闪存启动, 在通过 nRESET 引脚复位期间 nCS 应为低电平
 * @param void                                                                                                  
 * @return void                                                                                                  
 ***********************************************************************************************************/
void M8266WIFI_Module_Hardware_Reset(void) // 本例子中这个函数的总共执行时间大约800ms
{
	M8266HostIf_Set_SPI_nCS_Pin(0);	// 为了实现正常复位，模块的片选信号nCS(=ESP8266 GPIO15)在复位期间需要保持拉低
	M8266WIFI_Module_delay_ms(1);	// 延迟1毫秒，确保片选nCS设置后有足够的时间来稳定
	
	M8266HostIf_Set_nRESET_Pin(0);	// 拉低nReset管脚让模组进入复位状态
	// 延迟5毫秒，确保片选nRESERT设置后有足够的时间来稳定，也确保nCS和nRESET有足够的时间同时处于低电平状态.如果主板不是很好，
	// 导致上升下降过渡时间较长，或者因为失配存在较长的振荡时间，所以信号到轨稳定的时间较长，那么在这里可以多给一些延时	
	M8266WIFI_Module_delay_ms(5);	
	
	M8266HostIf_Set_nRESET_Pin(1);	// 再次拉高nReset管脚让模组退出复位状态
	M8266WIFI_Module_delay_ms(300);	// 至少需要18ms的延时来确保退出复位时足够的boottrap管脚采样时间
	/*
	* 在这里我们使用了300ms的延时来确保足够的富裕量，这是因为在某些主板上，他们的GPIO可能需要较多的时间来输出稳定，
	* 特别是对于nRESET所对应的GPIO输出。如果你的主机板在这里足够好，你可以缩短这里的延时来缩短复位周期；反之则需要加长这里的延时。
	* 总之，你可以调整这里的时间在你们的主机板上充分测试，找到一个合适的延时，确保每次复位都能成功。并适当保持一些富裕量，来兼容批量化时主板的个体性差异
	*/
	
	M8266HostIf_Set_SPI_nCS_Pin(1);		// 复位完成后释放/拉高（默认）片选信号
	//M8266WIFI_Module_delay_ms(1);		// 延迟1毫秒，确保片选nCS设置后有足够的时间来稳定
	
	M8266WIFI_Module_delay_ms(800-300-5-2); //M8266WIFI模块启动和初始化延迟约500ms以上，包括启动信息打印。不影响主机接口通信。必要时可以缩短。 但如果调整，则需要进行验证测试。
}
//

//
/***********************************************************************************
 * M8266WIFI_SPI_wait_sta_connecting_to_ap_and_get_ip                              *
 * @brief 确保连网成功- (确保模块联网成功，以便后续建立 TCP 或 UDP 链接)
 *    Wait M8266WIFI in STA mode connecting to AP/Router and get ip address        *
 *    via SPI API functions in M8266WIFIDrv.lib                                    *

 * @param sta_ip: if success, sta_ip return the ip address achieved                 *
 * @param max_wait_time_in_s: max timeout to wait in seconds                        *
 * @return                                                                         *
 *       1 = M8266WIFI module connects AP/router and gets a valid IP               *
 *           from AP/router before the timeout successfully                          *
 *       0 = M8266WIFI module fails to connect the AP/router and get               *
 *           a valid IP before the timeout                                         *
 ***********************************************************************************/
u8 M8266WIFI_SPI_wait_sta_connecting_to_ap_and_get_ip(char* sta_ip, u8 max_wait_time_in_s)
{
	u16  i;
	u16  status=0;
	 
	for(i=0; i < 1 * max_wait_time_in_s; i++)  // max wait
	{
		if( (M8266WIFI_SPI_Get_STA_IP_Addr(sta_ip , &status) ==1) && (strcmp(sta_ip, "0.0.0.0")!=0) )
			break;
		M8266WIFI_Module_delay_ms(1000);
		continue;
	}
	if(i>=1*max_wait_time_in_s)
		return 0; // false

	return 1;  // true
}
//

//
/***********************************************************************************
 * M8266WIFI_Module_Init_Via_SPI(void)
 * @brief 通过SPI初始化M8266WIFI模块
 *    To perform a Initialization sequency to M8266WIFI module via SPI I/F         *
 *    (1) Reboot the M8266WIFI module via nRESET pin                               *
 *    (2) Wait and Check if the M8266WIFI module gets an valid IP address          *
 *        if the module in STA or STA+AP mode                                      *
 *    Call this function after Host SPI Interface Initialised                      *
 *    if use SPI interface to config module                                        *
 * @return  0 = failed  1 = success 
 ***********************************************************************************/
u8 M8266WIFI_Module_Init_Via_SPI(void)	//TODO:配置相关模式等
{
	u32  	spi_clk = 40000000;
	u8   	sta_ap_mode = 0;
	u8   	connection_status = 0xFF;
	char 	sta_ip[15+1]={0};
	char 	ssid[32];
	s8   	rssi;
	u16  	status = 0;



	//-------------------------------------------------------------------------------------
	//Step 1: To hardware reset the module (with nCS=0 during reset) and wait up the module bootup
	//步骤1：对模组执行硬复位时序,在片选nCS拉低的时候对nRESET管脚输出低高电平)，并等待模组复位启动完毕
	M8266WIFI_Module_Hardware_Reset();	//前方定义



	//-------------------------------------------------------------------------------------
	//Step2: Try SPI clock in a fast one as possible up to 40MHz (M8266WIFI could support only upto 40MHz SPI) 
	// 第二步，在确保SPI底层通信可靠的前提下，调整SPI时钟尽可能的快，以免支持最快速度通信。本模组最大可以支持40MHz的SPI频率
	/* Note:
	* 对于STM32系列单片机来说，SPI1和SPI4通常是时来自于APB2时钟分频，所以，SPI1/4 CLOCK = APB2_CLOCK/SPI_BaudRatePrescaler
	* 而SPI2和SPI3通常是时来自于APB1时钟分频，所以，SPI2/3 CLOCK = APB1_CLOCK/SPI_BaudRatePrescaler
	* (1) For STM32F1, , please see system_stm32f10x.c, APB2_CLOCK = PCLK2 = HCLK = SYSCLK, 
	* 	APB1_CLOCK=PCLK1 = HCLK/4 = SYSCLK/2!		SYSCLK  = 72MHz, so, APB2_CLOCK = 72MHz
	* (2) For STM32F2, please see system_stm32f2xx.c, APB2_CLOCK = PCLK2 = HCLK/2 = SYSCLK/2, 
	* 	APB1_CLOCK=PCLK1 = HCLK/4 = SYSCLK/4!	 
	*	- STM32F207xx:  SYSCLK = 120MHz, so, APB2_CLOCK = 60MHz, APB1_CLOCK = 30MHz
	* (3) For STM32F4, please see system_stm32f4xx.c, APB2_CLOCK = PCLK2 = HCLK/2 = SYSCLK/2, 
	* 	APB1_CLOCK=PCLK1 = HCLK/4 = SYSCLK/4! 
	* 	Meanwhile, the PCLK2 should <= 90MHz, and SPI_CLK should <= 45MHz
	*		 - STM32F401xx:     SYSCLK  = 168MHz, so, APB2_CLOCK = 84MHz, APB1=42MHz
	*		 - STM32F40_41xxx:  SYSCLK  = 168MHz, so, APB2_CLOCK = 84MHz, APB1=42MHz
	*		 - STM32F427_437xx, STM32F429_439xx = 180MHz, so, APB2_CLOCK = 90MHz, APB1=45MHz
	* (4) For STM32F7, please see sys.c, APB2_CLOCK = PCLK2 = HCLK/2 = SYSCLK/2
	*	Meanwhile, the PCLK2 should <= 108MHz, and SPI_CLK should <=54MHz
	*		 - STM32F767xx:     SYSCLK = 216MHz, so, APB2_CLOCK = 108MHz, APB2_CLOCK = 54MHz
	* (5) For STM32L1, please see sys.c, APB1_CLK = APB2_CLOCK = HCLK = SYSCLK = 32MHz
	* (6) For STM32L4, please see sys.c, APB1_CLK = APB2_CLOCK = HCLK = SYSCLK = 80MHz
	* (7) For STM32H7, using reset default:
	*			SPI 1/2/3 kernel clock = pll1_q_ck = 200MHz
	*			SPI 4/5   kernel clock = APB clock = 100MHz
	*
	* For MCU_IS_NXP_LPC17XX
	* (1) SPI Clock =PCLK_SPI/SPI_BaudRatePrescaler
	* (2) PCLK_SPI  =  CPU_CLK = 96MHz, please see system_LPC17xx.c

	* For MCU_IS_NXP_MK27_28
	*	(1) SPI 0/1/2/3 SPI Clock = BUS_CLOCK= (75MHz)/SPI_BaudRateScaler(=2)/SPI_BaudRatePrecaler = 37.5MHz/SPI_BaudRatePrecaler
	*	(2) SPI 3  SPI Clock = SYS_CLOCK=(150MHz)/SPI_BaudRateScaler(=2)/SPI_BaudRatePrecaler = 75.0MHz/SPI_BaudRatePrecaler
	*
	* For MCU_IS_FREESCALE_MK60, please see system_MK60xxx.c
	*	(1) SPI Clock =PCLK_SPI/SPI_BaudRatePrescaler = 15MHz
	*	(2) BUS_SPI  =  CORE_CLK/2 = 120/2 = 60MHz
	*
   	* For MCU_IS_HT32F16XX
	*	(1) SPI Clock   =  APB_CLK_SPI / SPI_BaudRatePrescaler
	*	(2) APB_CLK_SPI =  SYS_CLK /2 = 144MHz /2 = 72MHz, please see system_ht32f165x.c
	**/

	#if defined(MCU_IS_STM32) && !defined(MCU_IS_STM32H7XX)
		#ifndef SPI_BaudRatePrescaler_2
			#define SPI_BaudRatePrescaler_2			((uint32_t)0x00000000U)
			#define SPI_BaudRatePrescaler_4         ((uint32_t)0x00000008U)
			#define SPI_BaudRatePrescaler_8         ((uint32_t)0x00000010U)
			#define SPI_BaudRatePrescaler_16        ((uint32_t)0x00000018U)
			#define SPI_BaudRatePrescaler_32        ((uint32_t)0x00000020U)
			#define SPI_BaudRatePrescaler_64        ((uint32_t)0x00000028U)
			#define SPI_BaudRatePrescaler_128       ((uint32_t)0x00000030U)
			#define SPI_BaudRatePrescaler_256       ((uint32_t)0x00000038U)
		#endif

	#elif defined(MCU_IS_STM32H7XX)
		#ifndef SPI_BAUDRATEPRESCALER_2
			#define SPI_BAUDRATEPRESCALER_2         (0x00000000U)
			#define SPI_BAUDRATEPRESCALER_4         (0x10000000U)
			#define SPI_BAUDRATEPRESCALER_8         (0x20000000U)
			#define SPI_BAUDRATEPRESCALER_16        (0x30000000U)
			#define SPI_BAUDRATEPRESCALER_32        (0x40000000U)
			#define SPI_BAUDRATEPRESCALER_64        (0x50000000U)
			#define SPI_BAUDRATEPRESCALER_128       (0x60000000U)
			#define SPI_BAUDRATEPRESCALER_256       (0x70000000U)
		#endif

	#elif defined(MCU_IS_NXP_LPC11XX) || defined(MCU_IS_NXP_LPC17XX)
		#ifndef SPI_BaudRatePrescaler_2
			#define SPI_BaudRatePrescaler_2         ((uint32_t)0x00000002U)
			#define SPI_BaudRatePrescaler_4         ((uint32_t)0x00000004U)
			#define SPI_BaudRatePrescaler_6         ((uint32_t)0x00000006U)
			#define SPI_BaudRatePrescaler_8         ((uint32_t)0x00000008U)
			#define SPI_BaudRatePrescaler_16        ((uint32_t)0x00000010U)
			#define SPI_BaudRatePrescaler_32        ((uint32_t)0x00000020U)
			#define SPI_BaudRatePrescaler_64        ((uint32_t)0x00000040U)
			#define SPI_BaudRatePrescaler_128       ((uint32_t)0x00000080U)
			#define SPI_BaudRatePrescaler_256       ((uint32_t)0x00000100U)
		#endif

	#elif defined (MCU_IS_NXP_MK27_28) || defined (MCU_IS_NXP_MKV58)
		#ifndef SPI_BaudRatePrescaler_2
			#define SPI_BaudRatePrescaler_2         ((uint32_t)0x00000000U)
			#define SPI_BaudRatePrescaler_4         ((uint32_t)0x00000001U)
			#define SPI_BaudRatePrescaler_6         ((uint32_t)0x00000002U)
			#define SPI_BaudRatePrescaler_8         ((uint32_t)0x00000003U)
			#define SPI_BaudRatePrescaler_16        ((uint32_t)0x00000004U)
			#define SPI_BaudRatePrescaler_32        ((uint32_t)0x00000005U)
			#define SPI_BaudRatePrescaler_64        ((uint32_t)0x00000006U)
			#define SPI_BaudRatePrescaler_128       ((uint32_t)0x00000007U)
			#define SPI_BaudRatePrescaler_256       ((uint32_t)0x00000008U)
		#endif

	#elif defined(MCU_IS_TI_TMS320F283XX)
		#define SPI_BaudRatePrescaler_4         ((uint32_t)0x00000003U)
		#define SPI_BaudRatePrescaler_5         ((uint32_t)0x00000004U)
		#define SPI_BaudRatePrescaler_6         ((uint32_t)0x00000005U)
		#define SPI_BaudRatePrescaler_7         ((uint32_t)0x00000006U)
		#define SPI_BaudRatePrescaler_8         ((uint32_t)0x00000007U)
		#define SPI_BaudRatePrescaler_9         ((uint32_t)0x00000008U)
		#define SPI_BaudRatePrescaler_10        ((uint32_t)0x00000009U)
		#define SPI_BaudRatePrescaler_16        ((uint32_t)0x0000000FU)

	#elif defined(MCU_IS_C8051F1XX)
		#define SPI_BaudRatePrescaler_2         ((uint32_t)0x00000000U)
		#define SPI_BaudRatePrescaler_4         ((uint32_t)0x00000001U)
		#define SPI_BaudRatePrescaler_6         ((uint32_t)0x00000002U)
		#define SPI_BaudRatePrescaler_8         ((uint32_t)0x00000003U)
		#define SPI_BaudRatePrescaler_10        ((uint32_t)0x00000004U)
		#define SPI_BaudRatePrescaler_12        ((uint32_t)0x00000005U)
		#define SPI_BaudRatePrescaler_14        ((uint32_t)0x00000006U)
		#define SPI_BaudRatePrescaler_16        ((uint32_t)0x00000007U)
		#define SPI_BaudRatePrescaler_18        ((uint32_t)0x00000008U)
		#define SPI_BaudRatePrescaler_20        ((uint32_t)0x00000009U)
		#define SPI_BaudRatePrescaler_32        ((uint32_t)0x0000000FU)
		#define SPI_BaudRatePrescaler_64        ((uint32_t)0x0000001FU)
		#define SPI_BaudRatePrescaler_128       ((uint32_t)0x0000003FU)
		#define SPI_BaudRatePrescaler_256       ((uint32_t)0x0000007FU)

	#elif defined(MCU_IS_TI_TMS320F283XX)
		#define SPI_BaudRatePrescaler_4         ((uint32_t)0x00000003U)
		#define SPI_BaudRatePrescaler_5         ((uint32_t)0x00000004U)
		#define SPI_BaudRatePrescaler_6         ((uint32_t)0x00000005U)
		#define SPI_BaudRatePrescaler_7         ((uint32_t)0x00000006U)
		#define SPI_BaudRatePrescaler_8         ((uint32_t)0x00000007U)
		#define SPI_BaudRatePrescaler_9         ((uint32_t)0x00000008U)
		#define SPI_BaudRatePrescaler_10        ((uint32_t)0x00000009U)
		#define SPI_BaudRatePrescaler_16        ((uint32_t)0x0000000FU)

	#elif defined (MCU_IS_NXP_iMXRT10xx)
		#ifndef SPI_BaudRatePrescaler_2
			#define SPI_BaudRatePrescaler_2         ((uint32_t)0x00000000U)
			#define SPI_BaudRatePrescaler_4         ((uint32_t)0x00000002U)
			#define SPI_BaudRatePrescaler_6         ((uint32_t)0x00000004U)
			#define SPI_BaudRatePrescaler_8         ((uint32_t)0x00000006U)
			#define SPI_BaudRatePrescaler_12        ((uint32_t)0x0000000AU)
			#define SPI_BaudRatePrescaler_16        ((uint32_t)0x0000000EU)
			#define SPI_BaudRatePrescaler_32        ((uint32_t)0x0000001EU)
			#define SPI_BaudRatePrescaler_64        ((uint32_t)0x0000003EU)
			#define SPI_BaudRatePrescaler_128       ((uint32_t)0x0000007EU)
			#define SPI_BaudRatePrescaler_256       ((uint32_t)0x000000FEU)
		#endif

	#else
	#endif


	#if defined(MCU_IS_STM32F1XX) || defined(MCU_IS_STM32F3XX) // F1 has SPI, SPI2 & SPI3
		#if (M8266WIFI_SPI_INTERFACE_NO == 1)
			M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_4);	// Setup SPI Clock. Here 72/4 = 18MHz for STM32F2xx SPI1 or SPI4, up to 18MHz, since SPI1/4 clock divided from faster APB2 clock
		#elif (M8266WIFI_SPI_INTERFACE_NO == 2) || (M8266WIFI_SPI_INTERFACE_NO == 3)
			M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_2);	// Setup SPI Clock. Here 36/2 = 18MHz for STM32F2xx SPI2 or SPI3, up to 18MHz, since SPI2/3 clock divided from lowver APB1 clock
		#endif
		spi_clk =  18000000;

	#elif  defined(MCU_IS_STM32F2XX)  // F2 has SPI1, SPI2 & SPI3
		#if (M8266WIFI_SPI_INTERFACE_NO == 1)
			M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_4);				// Setup SPI Clock. Here 60/4 = 15MHz for STM32F2xx SPI1 or SPI4, up to 30MHz, since SPI1/4 clock divided from faster APB2 clock
		#elif (M8266WIFI_SPI_INTERFACE_NO == 2) || (M8266WIFI_SPI_INTERFACE_NO == 3)
			M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_2);				// Setup SPI Clock. Here 30/2 = 15MHz for STM32F2xx SPI2 or SPI3, up to 15MHz, since SPI2/3 clock divided from lowver APB1 clock
		#endif
			spi_clk =  15000000;

	#elif  defined(MCU_IS_STM32F4XX) || defined(MCU_IS_STM32F4XX_HAL)  // F4 has SPI, SPI2 & SPI3, and possible SPI4
		#if (M8266WIFI_SPI_INTERFACE_NO == 1) || (M8266WIFI_SPI_INTERFACE_NO == 4)
			M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_4);				// Setup SPI Clock. Here 84/4 = 21.0MHz for STM32F4xx SPI1 or SPI4, up to 40MHz, since SPI1/4 clock divided from faster APB2 clock
		#elif (M8266WIFI_SPI_INTERFACE_NO == 2) || (M8266WIFI_SPI_INTERFACE_NO == 3)
			M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_2);				// Setup SPI Clock. Here 42/2 = 21.0MHz for STM32F4xx SPI2 or SPI3, up to 21MHz, since SPI2/3 clock divided from lowver APB1 clock
		#endif
			spi_clk = 21000000;

	#elif  defined(MCU_IS_STM32F7XX)
		#if (M8266WIFI_SPI_INTERFACE_NO == 1) || (M8266WIFI_SPI_INTERFACE_NO == 4)
		M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_4);					// Setup SPI Clock. Here 108/4 = 27MHz for STM32F7xx, up to 40MHz
		#elif (M8266WIFI_SPI_INTERFACE_NO == 2) || (M8266WIFI_SPI_INTERFACE_NO == 3)
		M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_2);					// Setup SPI Clock. Here 54/2  = 27MHz for STM32F7xx
		#endif
			spi_clk = 27000000;

	#elif  defined(MCU_IS_STM32L1XX)
		M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_2);				// Setup SPI Clock. Here 32/2  = 16MHz for STM32L1xx
			spi_clk = 16000000;

	#elif  defined(MCU_IS_STM32L4XX)
		M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_4);				// Setup SPI Clock. Here 80/4  = 20MHz for STM32L4xx
			spi_clk = 20000000;

	#elif  defined(MCU_IS_STM32H7XX)
		#if (M8266WIFI_SPI_INTERFACE_NO == 1) || (M8266WIFI_SPI_INTERFACE_NO == 2) || (M8266WIFI_SPI_INTERFACE_NO == 3)
		M8266HostIf_SPI_SetSpeed(SPI_BAUDRATEPRESCALER_8);			// Setup SPI Clock. Here 200/8 = 25MHz for STM32F7xx, up to 40MHz
		#elif	(M8266WIFI_SPI_INTERFACE_NO == 4) || (M8266WIFI_SPI_INTERFACE_NO == 5)
		M8266HostIf_SPI_SetSpeed(SPI_BAUDRATEPRESCALER_4);			// Setup SPI Clock. Here 100/4 = 25MHz for STM32F7xx, up to 40MHz
		#endif
			spi_clk = 25000000;

	#elif  defined(MCU_IS_NXP_LPC11XX)
		M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_4);					// Setup SPI Clock. Here 48/4 = 12MHz for LPC11XX
			spi_clk = 12000000;

	#elif  defined(MCU_IS_NXP_LPC17XX)
		M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_4);					// Setup SPI Clock. Here 96/4 = 24MHz for LPC17XX, upto 40MHz
			spi_clk = 24000000;

	#elif  defined(MCU_IS_NXP_MK60)
			//M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_4);				// Setup SPI Clock. Here 60/4 = 15 MHz for MK60 F15, and 48/2=24 MHz for MK60 Z10. MK60 Supports max 30MHz SPI. However, maximum 1/4 of CORE CLOCK :-(
		#if	defined(MK60F15)   
			spi_clk = 15000000;
		#elif defined(MK60DZ10)
			spi_clk = 12000000;
		#else
		#error   UKNOWN MK60 Subtype, F15 or Z10?
		#endif
			
	#elif  defined(MCU_IS_NXP_MKV58)
		M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_2);	// Setup SPI Clock. Here 37.5/2 = 18.75 MHz for for MK27/28.
		spi_clk = 18750000;
			
	#elif  defined(MCU_IS_NXP_MK27_28)
		M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_2);	// Setup SPI Clock. Here 37.5/2 = 18.75 MHz for for MK27/28.
		spi_clk = 18750000;

	#elif  defined(MCU_IS_FREESCALE_MK60)
		//M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_4);	// Setup SPI Clock. Here 60/4 = 15 MHz for MK60 F15, and 48/2=24 MHz for MK60 Z10. MK60 Supports max 30MHz SPI. However, maximum 1/4 of CORE CLOCK :-(
		#if  defined(MK60F15)   
			spi_clk = 15000000;
		#elif  defined(MK60DZ10)
			spi_clk = 12000000;
		#else
		#error   UKNOWN MK60 Subtype, F15 or Z10?
		#endif                                                  


	#elif  defined(MCU_IS_NXP_iMXRT10xx)
		M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_8);	// Setup SPI Clock. Here 192/8 = 24 MHz for for iMXRT10xx.
			spi_clk = 24000000;

	#elif  defined(MCU_IS_NuMicro_M451)
		M8266HostIf_SPI_SetSpeed(4);		// Setup SPI Clock. Here 72/4 = 18MHz for Nuvoton M451, upto 40MHz
			spi_clk =  18000000;

	#elif  defined(MCU_IS_HT32F16XX)
		M8266HostIf_SPI_SetSpeed(4);																// Setup SPI Clock. Here 72/4 = 18MHz for HT32F165x, upto 40MHz
			spi_clk =  18000000;

	#elif  defined(MCU_IS_TI_TMS320F283XX)
		M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_4); 				// Setup SPI Clock. Here 37.5/4 = 9。375MHz for TMS320F283XX, upto 40MHz
			spi_clk = 9375000;

	#elif  defined(MCU_IS_C8051F1XX)
		M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_8);// Setup SPI Clock. Here 98/8 = 12.25MHz for C8051F120
			spi_clk = 12250000;
			//spi_clk   =  6125000;

	#elif defined (MCU_IS_MSP430x552x)
		M8266HostIf_SPI_SetSpeed(2);																// Setup SPI Clock. Here 24/2 = 12MHz for MSP430x552x
			spi_clk =  12000000;         

	#elif defined (MCU_IS_MSP430x1xx)
		M8266HostIf_SPI_SetSpeed(8);																// Setup SPI Clock. Here 8/2 = 4MHz for MSP430x1xx
			spi_clk =  4000000;         

	#elif defined (MCU_IS_MSP430x471x)
		M8266HostIf_SPI_SetSpeed(2);																// Setup SPI Clock. Here 24/2 = 12MHz for MSP430x471x
			spi_clk =  8000000;                  

	#elif  defined(MCU_IS_TI_TMS320F283XX)
		M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_4); 				// Setup SPI Clock. Here 37.5/4 = 9。375MHz for TMS320F283XX, upto 40MHz
			spi_clk = 9375000;

	#else
	#error YOU SHOULD DEFINED MCU_IS_XXX near line 17 in brd_cfg.h
	#endif

	// 设置SPI时钟后，延时等待时钟稳定
	M8266WIFI_Module_delay_ms(1);


	//-------------------------------------------------------------------------------------
	// Step3: It is very mandatory to call M8266HostIf_SPI_Select() to tell the driver which SPI you used and how faster the SPI clock you used. The function must be called before SPI access
	// 第三步：调用M8266HostIf_SPI_Select()。 在正式调用驱动API函数和模组进行通信之前，调用M8266HostIf_SPI_Select()来告诉
	//		  驱动使用哪个SPI以及SPI的时钟有多快，这一点非常重要。 如果没有调用这个API，单片机主机和模组之间将可能将无法通信
	if(M8266HostIf_SPI_Select((uint32_t)M8266WIFI_INTERFACE_SPI, spi_clk, &status)==0)
	{
     	// If M8266HostIf_SPI_Select() fails here, then check your host interface wiring and initialization
     	// 如果你在执行时 M8266HostIf_SPI_Select()失败而进入了这里，请仔细检查主机接口的接线是否正确和可靠，SPI主机接口初始化是否正确。
	   	//可参考《主机集成说明》之章节“底层调试技巧--主机接口的硬件接线、初始化和匹配以及验证技巧（选阅）” 进行快速梳理和定位。
		while(1)
		{
			#ifdef USE_LED_AND_KEY_FOR_TEST	 // MB LEDs flash in 2Hz upon errors
			LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(250);
			LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(250);
			#endif
		}
   }



	//-------------------------------------------------------------------------------------
	// Step 4: Used to evaluate the high-speed spi communication. Changed to #if 0 to comment it for formal release
	// 第四步：开发阶段和测试阶段，用于测试评估主机板在当前频率下进行高速SPI读写访问时的可靠性。
	//       如果足够可靠，则可以适当提高SPI频率；如果不可靠，则可能需要检查主机板连线或者降低SPI频率。
	//		 (Note:产品研发完毕进入正式产品化发布阶段后，因为在研发阶段已经确立了最佳稳定频率，建议这里改成 #if 0，不必再测试)
	#if 1  // 开发阶段和测试阶段
	{   
		volatile u32  i, j;
		u8   byte;

		if(M8266WIFI_SPI_Interface_Communication_OK(&byte)==0) 	  									//	if SPI logical Communication failed
		{
		   while(1)
			 {
				#ifdef USE_LED_AND_KEY_FOR_TEST	 // MB LEDs flash in 1Hz upon errors
				LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(500);
				LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(500);
				#endif
			 }
		}

		i = 100000;
		j = M8266WIFI_SPI_Interface_Communication_Stress_Test(i);
		if( (j<i)&&(i-j>5)) 		//  if SPI Communication stress test failed (Chinese: SPI底层通信压力测试失败，表明你的主机板或接线支持不了当前这么高的SPI频率设置)
		{
		   while(1)
			{
				#ifdef USE_LED_AND_KEY_FOR_TEST	 // MB LEDs flash in 1Hz upon errors
				LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(500);
				LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(500);
				#endif
			}
		}
 	}
	#endif



	//-------------------------------------------------------------------------------------
	// Step 5: Configuration to module
	// 第五步：配置模组
	
	//-------------------------------------------------------------------------------------
	// 5.1 If you hope to reduce the Max Tx power, you could enable it by change to "#if 1" 
	// 5.1 如果你希望减小模组的最大发射功率，可以将这里改成 #if 1，并调整下面的 tx_max_power参数的值
 	#if 0 //更改功率
		//u8 M8266WIFI_SPI_Set_Tx_Max_Power(u8 tx_max_power, u16 *status)
		//tx_max_power=68 to set the max tx power of around half of manufacture default, i.e. 50mW or 17dBm. Refer to the API specification for more info
		//下方:tx_max_power=68表示将发射最大功率设置为出厂缺省数值的一般，即50mW或者17dBm。具体数值含义可以查看这个API函数的头文件声明里的注释
		if(M8266WIFI_SPI_Set_Tx_Max_Power(68, &status) == 0)
			return 0;
	#endif

	//-------------------------------------------------------------------------------------
	// 5.2 If you expect to config or stop the on-module web server, you could enable it by change to "#if 1" and adjust as required
	// 5.2 如果你希望配置或关闭模组上自带的WEB服务器，你可以将这里改成 #if 1，并调整下面的API函数里的相关参数值
	#if 0	// Web服务器
	{
		//u8 M8266WIFI_SPI_Set_WebServer(u8 open_not_shutdown, u16 server_port, u8 saved, u16* status)
		// if(M8266WIFI_SPI_Set_WebServer(0, 3128, 0, &status)==0)  // 第一个参数0表示关闭WEB服务器)
		// {
		// 	return 0;
		// }
		// 1-> to (re-)start the web_server with port = 3128, not saved, invalid after reboot
		// 第一个参数1表示开启WEB服务器，第二个参数3128表示将WEB服务器的端口改成3128，第三个参数0表示不保存，只是当前设置有效，模组复位后失效
		if(M8266WIFI_SPI_Set_WebServer(1, 3128, 0, &status)==0)
		{                                                        
			return 0;
		}
	}
	#endif

	//-------------------------------------------------------------------------------------
	// 5.3 If you expect to change the op_mode overriding the default loaded from flash on bootup, enable it by "#if 1"
	// 5.3 如果你希望改变模组的op_mode，不使用模组启动时缺省op_mode，你可以这里改成 #if 1，并调整下面的API函数里的相关参数值
	#if 1
	{
		//u8 M8266WIFI_SPI_Set_Opmode(u8 op_mode, u8 saved, u16* status);
		if( M8266WIFI_SPI_Set_Opmode(3, 0, &status) == 0 )	// 1=STA Only, 2=AP Only, 3=STA+AP
			return 0;
	}
	#endif

	//-------------------------------------------------------------------------------------
	// 5.4 If you expect to change the ap info overriding the default loaded from flash on bootup, enable it by "#if 1". Meanwhile, according to Protocols, the length of password should not be smaller than 8 Bytes per WAP or WAP2
	// 5.4 如果你希望改变模组作为 AP热点 时的 AP热点名称 和 密码，不使用模组启动时缺省参数，你可以这里改成 #if 1，并调整下面的API函数里的相关参数值. 同时根据相关协议约定，WAP和WAP2的密码长度不能少于8个字节
	#if 1	//TODO:设置模块在 AP 模式下的WiFi名和WiFi密码
	{
		//u8 M8266WIFI_SPI_Config_AP(u8 ssid[13+1], u8 password[13+1], u8 enc, u8 channel, u8 saved, u16* status);
		if(M8266WIFI_SPI_Config_AP("Anylinkin", "1234567890", 4, 1, 0, &status)==0)  // set to 4=WPA_WPA2_PSK, not saved // 0=OPEN, 1=WEP, 2=WPA_PSK, 3=WPA2_PSK, 4=WPA_WPA2_PSK
		return 0;
	}
	#endif

	//-------------------------------------------------------------------------------------
	// 5.5 get current op_mode, if STA mode or STA+AP mode, then, wait connection to external AP and get ip
	// 5.5 查询当前的op_mode，如果处于STA模式或者STA+AP模式，那么可根据需要执行配网去链接上第三方热点/路由器，并等待获取ip地址
	//u8 M8266WIFI_SPI_Get_Opmode(u8* op_mode, u16* status);
	if(M8266WIFI_SPI_Get_Opmode(&sta_ap_mode, &status)==0)
    	return 0;

	if( (sta_ap_mode == 1) ||(sta_ap_mode == 3) )  // 1==STA模式	3==STA+AP模式
	{
		// If you expect to use smartconfig to config the module, enable here by "#if 1" and prepare to send ssid using your smart devices such as smartphone
		// 如果你希望使用智能配网来进行配网，你可以这里改成 #if 1
		#if 0 // 智能配网
			u8 smartconfig_type;
			char smartconfig_phone_ip[15+1]={0};
			// u8 M8266WIFI_SPI_DoModuleSmartConfig(u8 timeout_in_s, u8 saved, u8* smartconfig_type, char smartconfig_phone_ip[15+1], u16* status);
			if(M8266WIFI_SPI_DoModuleSmartConfig(30, 0, &smartconfig_type, smartconfig_phone_ip, &status)==0)
				return 0;
			while(1)
			{

			}
		#endif

		// If you expect to use the SSID and password here overriding the default loaded from on-module SPI flash, enable it by "#if 1", and update the ssid and password with those of your routers connected to.
		// 如果模组上保存有之前连接过的热点/路由器的SSID和密码（称为缺省SSID和密码），而模组启动后的模组包含STA，那么模组会自动加载这个缺省的SSID和密码自动去连接路由器和热点。
		// 但是如果你不希望使用这个缺省的SSID和密码来连接路由器/热点，你可以这里改成 #if 1，并将其中的SSID和密码改成你所期望连接的热点/路由器的
		#if 1  //Note:配置连接的WiFi名和密码
			//if(M8266WIFI_SPI_STA_Connect_Ap("TP-LINK_A641", "d42612345678", 0, 20, &status) == 0) //not saved,timeout=20s
			//if(M8266WIFI_SPI_STA_Connect_Ap("Anylinkin!", "1234567890", 0, 20, &status) == 0)
			// if(M8266WIFI_SPI_STA_Connect_Ap("Anylinkin", "1234567890", 1, 20, &status) == 0)
			if(M8266WIFI_SPI_STA_Connect_Ap("test", "zxcvbnm1", 1, 20, &status) == 0)
				return 0;
		#endif

		/* 如果模组工作在包含STA的模式下，需要等待模组从所连接的热点/路由器获取到ip地址。因为获取到ip地址，
		// 是后面进行套接字通信的前提，因此，这里需要等待，确保模组获取到ip真正连接成功
		// 最多等待10秒。max_wait_time_in_s可以根据实际情形调整。但这个时间不是实际等待的时间，
		// 而是最大等待时间超时上限。这个函数会在获取到ip地址或等待时间到达这里的超时上限时返回 
		*/
		if(M8266WIFI_SPI_wait_sta_connecting_to_ap_and_get_ip(sta_ip, 10)==0) 
		{
			return 0;
		}

		// If you expect to to know the reason of failure by above M8266WIFI_SPI_wait_sta_connecting_to_ap_and_get_ip(), enable below
		//如果你希望查看联网失败后的原因，你可以在这里使用 #if 1
		#if 1	
		else
		{
			// connection_status将返回上次连接热点/路由器的结果状态，协助诊断(connection_status will give the status of last connecting)
			if(M8266WIFI_SPI_Get_STA_Connection_Status(&connection_status, &status)==0)  
				return 0;
		}
		#endif

		// these below function is just an example on how to use them. You may or may not use them during module initialisation
		// 下面这个函数只是一个例子，来演示如何获取模组连接热点/路由器时的信号强度，你可以在这里使用 #if 1来使用它，也可以使用#if 0来不适用它)
		#if 1
			// u8 M8266WIFI_SPI_STA_Query_Current_SSID_And_RSSI(char* ssid, u8* rssi, u16* status)
			if(M8266WIFI_SPI_STA_Query_Current_SSID_And_RSSI(ssid, &rssi, &status)==0)
				return 0;
		#endif

	} // end to if((sta_ap_mode == 1 || ==3)
  	return 1;	
} //end of M8266WIFI_Module_Init_Via_SPI()


//	下面只是一些应用层的函数，仅供参考，非必须。

/*************************************************************************************                                           
 * @brief 重启模块，等待wifi连接，并建立udp/tcp连接，无论响应是否为预期的
 * @param tcp_udp  建立 tcp 或 udp 服务链接的类型(0=UDP, 1=TCP Client, 2=TCP Server)                 
 * @param local_port 本地端口。如果设定为 0，则由模组会产生一个随机的本地端口。
 * @param remote_addr 远程IP/DNS地址的字符串。e.g. "192.168.1.2" or "www.baidu.com"                       
 * @param remote_port 远程端口号                            
 * @param link_no  服务链接通道的链接号(每个多链接最多8个链路)
 * @return u8 (成功=1,失败=0)
 *************************************************************************************/
u8 M8266WIFI_Config_Connection_via_SPI(u8 tcp_udp, u16 local_port, char* remote_addr, u16 remote_port, u8 link_no)
{
	u16 status=0;
  	//函数原型 u8 M8266WIFI_SPI_Setup_Connection(u8 tcp_udp, u16 local_port, char remote_addr, u16 remote_port, u8 link_no, u8 timeout_in_s, u16* status);
	return M8266WIFI_SPI_Setup_Connection(tcp_udp, local_port, remote_addr, remote_port, link_no, 20, &status);
}
//








//====================================================================================
//								休眠/唤醒相关函数
//====================================================================================

/*************************************************************************************
 * @brief 让模块（深度）休眠一段时间，然后自动唤醒(进入睡眠和唤醒后所有连接都会丢失,应当被主机中心建立连接)
 *      例如                                                                    
 *          if(M8266WIFI_Sleep_M8266WIFI(5000)==0) // wakeup in 5000ms          
 *		    	return fail; // do somework here for failure                    
 *            M8266WIFI_Module_Init_Via_SPI();       // re-initial the module   
 *         or M8266WIFI_Config_Connection_via_SPI(); // re-establish the server 
 * @param time_to_wakeup_in_ms  休眠多久后退出休眠，单位是毫秒。
 *    （如果设置为 0，则模组将永久休眠，直到自动唤醒。）最大值 4294967 毫秒 (1.19h)，如果超出最大值，将使用最大值。          
 * @return u8 (成功=1,失败=0)                                                            
 *************************************************************************************/
u8 M8266WIFI_Sleep_module_for_some_time_and_then_wakeup_automatically(u16 time_to_wakeup_in_ms)
{
	u16 status;
  	//下方函数原型：u8 M8266WIFI_SPI_Sleep_Module(u8 sleep_type, u32 time_to_wakeup_in_ms, u16 *status)
	if(M8266WIFI_SPI_Sleep_Module(3, time_to_wakeup_in_ms, &status)==0) //sleep_type = 3 = deep_sleep
		return 0;

	if(time_to_wakeup_in_ms > 0) // not a forever sleep
  	{
		M8266HostIf_Set_SPI_nCS_Pin(0);   // set nCS low to ensure the module will reboot from flash after wakeup
	   	M8266WIFI_Module_delay_ms(time_to_wakeup_in_ms+1000);  // additional 1000ms for bootup time
	   	M8266HostIf_Set_SPI_nCS_Pin(1);
	}

	return 1;
}
//


//
/*************************************************************************************
 * @brief 通过 SPI 使永久模块深度休眠，并且应该由主机唤醒。调用此函数后，所有连接都会丢失
 * @param None
 * @return u8 (成功=1,失败=0)
 *************************************************************************************/
u8 M8266WIFI_Sleep_Module(void)
{
	u16 status;
	//u8 M8266WIFI_SPI_Sleep_Module(u8 sleep_type, u32 time_to_wakeup_in_ms, u16 *status)
	if(M8266WIFI_SPI_Sleep_Module(3, 0, &status)==0) //sleep_type=3=deep_sleep
		return 0;

	return 1;
}
//

//
/*************************************************************************************
 * @brief	唤醒模块- 进入睡眠和唤醒后所有连接都会丢失，并且应该在唤醒后由主机重新建立
 *      例如：                                                                        
 *      	if(M8266WIFI_Wakeup_Module()==0){	// do somework here for failure	}                                                                                                 
 *          else {	M8266WIFI_Config_Connection_via_SPI(); // re-establish the server	}                                                                              
 * @param	None
 * 
 * @return  u8  (failed=0, success=1)                                                               
 *************************************************************************************/
u8 M8266WIFI_Wakeup_Module(void)
{
	return M8266WIFI_Module_Init_Via_SPI();
}
