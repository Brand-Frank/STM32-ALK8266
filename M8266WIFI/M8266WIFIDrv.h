/********************************************************************
 *	�������������ļ���
 *	(1) ʵ���˺�ģ��Ļ���ͨ��API����
 *	(2) SPI������ȫ��ԭ�ӹ��ܣ����������������׽��ֵȵ�
 *	- �ֲ᣺��ALK8266WIFIģ��SPI�ӿڸ���ͨ��ʹ���뼯�� ��������API����˵������4��
 *
 * M8266WIFIDrv.h                                                   *
 * @brief                                                     *
 *     Header file of M8266WIFI Dirver Layer                        *
 ********************************************************************/


#ifndef _M8266WIFI_DRV_H_
#define _M8266WIFI_DRV_H_

//// Holtek HT32F16XX
//#include "ht32f1655_56.h"

// TI's MSP430x1xx, MSP430x552x, TMS320F28335, which use unsigned long for 32-bit
// C8051 Serials, which use unsigned long for 32-bit
//typedef unsigned char       uint8_t;
//typedef unsigned int  	    uint16_t;
//typedef unsigned long  	    uint32_t;
//typedef   signed char       sint8_t;

//Others which use unsigned int for 32-bit
typedef unsigned char         uint8_t;
typedef unsigned short int    uint16_t;
typedef unsigned int          uint32_t;
typedef   signed char         sint8_t;

typedef uint32_t		u32;
typedef uint16_t		u16;
typedef uint8_t			u8;
typedef sint8_t			s8;




//====================================================================================
//				(1) �ӿڳ�ʼ���� SPI ��ͨ���߼������ܲ��� API
//====================================================================================
//----------------------------------------------------------------------------------
//   1. APIs related to SPI Host I/F Function Configuration and Performance Test //
//       - M8266HostIf_SPI_Select                                        4.1.1    // ѡ��SPI
//       - M8266WIFI_SPI_Interface_Communication_OK                      4.1.2    // ͨ���ȶ��Լ��
//       - M8266WIFI_SPI_Interface_Communication_Stress_Test             4.1.3    // ͨ�����ܲ���
//----------------------------------------------------------------------------------

/***********************************************************************************
 * M8266HostIf_SPI_Select                                                          *
 * @brief  <4.1.1> - ������������ʹ�õ� SPI ����ַ��������������ʹ�õ� SPI Ƶ��
 * (1) ͨ��������������ʹ�õ� SPI ����ַ��ָ�룩��ʹ���������Լ������������SPIͨ����
 * (2) ͨ������������Ŀǰ������ʹ�õ� SPI Ƶ�ʣ��������Ը�����Ӧ�� SPI Ƶ�ʶ�ʱ����������߼������΢���Ż��������ͨ�����ܡ�
 * (3).Called when Initial M8266WIFI Module before perform SPI read/write           *
 * @param spi_base_addr ��ģ�����ӵ����� SPI �Ļ���ַ(you may use M8266WIFI_INTERFACE_SPI defined in brd_cfg.h as example)
 * @param spi_clock ���� SPI ��Ƶ�ʣ���λ Hz��(e.g. 10500000 for 10.5MHz)
 * @param status  ʧ��ʱ���ش����루LSB����״̬��MSB����ָ��
 *               �������Ҫ����״̬�룬����ʹ�� NULL��                          *
 * @return u8(1=success, 0=failure)
 ***********************************************************************************/
u8 M8266HostIf_SPI_Select(uint32_t spi_base_addr, uint32_t spi_clock, u16* status);

/***********************************************************************************
 * M8266WIFI_SPI_Interface_Communication_OK                                        *
 * @brief   <4.1.2> - д��һ���ֽ�Ȼ���M8266WIFIģ���SPI�Ĵ����ж����������ʱ�Ӻͻ������߼�����SPIͨ�ţ���/д���Ƿ��ȶ�����
 *  1. To write a byte and then read out from M8266WIFI module SPI registers       *
 *     to check whether the logical fundamentai SPI communication (read/write)     *
 *     is stabilly OK under the clock and interconnection                           *
 *  2. M8266WIFIģ���ʼ��ʱ���ã�ģ���������к�Ҫ����
 * @param byte:  a pointer to the byte read out during test                           *
 *     - normally the data should be 0x41 during test.                             *
 *       if it is 0x41, this function will return 1 for success as well            *
 *     - If readout other value, it may indicating the fundamental SPI             *
 *       communication is not OK. e.g. 0xFF may indicates that your spi            *
 *       port has problem such incoreect interconnection or initialization         *
 *     - user NULL if you don't expect it                                          * 
 * @return  1. success  0. failure
 ***********************************************************************************/
u8 M8266WIFI_SPI_Interface_Communication_OK(u8* byte);

/***********************************************************************************
 * M8266WIFI_SPI_Interface_Communication_Stress_Test                               *
 * @brief �������Խ׶ε� SPI ͨ�ŵ����ܲ���
 *  1. M8266WIFIģ����������ֽ�д��Ȼ�������SPI�Ĵ��������ٴ���������SPIͨ�ţ���/д���Ƿ���ʱ�����ȶ�����
 *  2. M8266WIFIģ���ʼ��ʱ���ã�ģ���������к�Ҫ����
 *  3. �������ڵ��Դ����е��������ڶ����Ĳ�Ʒ����ѹ�����ܲ����ڼ�����ٶ��ȶ��Բ���
 * @param max_times:  the max write-read times used for the stress test               *
 *     - And the test data to be written will be number byte of data               *
 *       from 0 to  max_times                                                      *
 * @return  success times of write-read-verify                                          *
 ***********************************************************************************/
u32 M8266WIFI_SPI_Interface_Communication_Stress_Test(u32 max_times);


       






//====================================================================================
//				(2)	WIFIģ�������ѯ��������ص� API
//	���� WIFI ���ܣ�WIFI ģ����������������ѯIP����ѯ�ź�ǿ�ȡ����� AP
//	�������ܣ��ź�ɨ���Լ����/��ֹ����ȹ���
//====================================================================================
/*----------------------------------------------------------------------------------
//   2. APIs related to Module WIFI fundamental Query and Configurations via SPI //
//       - M8266WIFI_SPI_Get_Opmode                                      4.2.1    // ��ѯ/��ȡ����ģʽ
//       - M8266WIFI_SPI_Set_Opmode                                      4.2.2    // ���ù���ģʽ
//                                                                                //
//       - M8266WIFI_SPI_STA_Connect_Ap                                  4.2.3    //
//       - M8266WIFI_SPI_STA_DisConnect_Ap                               4.2.4    //
//       - M8266WIFI_SPI_Config_STA_StaticIpAddr                         4.2.5    //
//       - M8266WIFI_SPI_Query_STA_Dhcpc                                 4.2.6    //
//       - M8266WIFI_SPI_Enable_STA_Dhcpc                                4.2.7    //
//       - M8266WIFI_SPI_Get_STA_Connection_Status                       4.2.8    //
//       - M8266WIFI_SPI_Get_STA_IP_Addr                                 4.2.9    //
//       - M8266WIFI_SPI_STA_Query_Current_SSID_And_RSSI                 4.2.10   //
//                                                                                //
//       - M8266WIFI_SPI_STA_ScanSignals                                 4.2.11   //
//       - M8266WIFI_SPI_STA_Fetch_Last_Scanned_Signals                  4.2.12   //
//       - M8266WIFI_SPI_STA_ScanSignalsBySsid                           4.2.13   //
//       - M8266WIFI_SPI_STA_ScanSignalsByBssid                          4.2.14   //
//                                                                                //
//       - M8266WIFI_SPI_Set_STA_Hostname                                4.2.15   //
//       - M8266WIFI_SPI_Get_STA_Hostname                                4.2.16   //
//       - M8266WIFI_SPI_Query_STA_Param                                 4.2.17   //
//                                                                                //
//       - M8266WIFI_SPI_Config_AP                                  	 4.2.18   //
//       - M8266WIFI_SPI_Query_AP_Param                             	 4.2.19   //
//       - M8266WIFI_SPI_Config_AP_Param                             	 4.2.20   //
//       - M8266WIFI_SPI_AP_List_STAs_Info                               4.2.21   //
//       - M8266WIFI_SPI_AP_Permit_Block_A_STA                           4.2.22   //
//       - M8266WIFI_SPI_AP_Config_PermitBlock_List                      4.2.23   //
//       - M8266WIFI_SPI_AP_Query_PermitBlock_List                       4.2.24   //
//----------------------------------------------------------------------------------*/

/***********************************************************************************
 * M8266WIFI_SPI_Get_Opmode   (SAT/AP/AP+SATģʽ)                                  *
 * @brief  <4.2.1> - ͨ�� SPI �ӿڶ�ȡģ�鵱ǰ�� WIFI ����ģʽ(STA, AP, or STA+AP)
 * @param op_mode ִ����ȷʱ����ȡ�õ���ģ�鵱ǰ�Ĳ���ģʽ����1=STA ��2=AP��3=STA+AP��
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣 �������Ҫ����״̬�룬����ʹ�� NULL��
 * @return (u8)	success=1, has errors=0
 ***********************************************************************************/
u8 M8266WIFI_SPI_Get_Opmode(u8* op_mode, u16* status);

/***********************************************************************************
 * M8266WIFI_SPI_Set_Opmode                                                        *
 * @brief <4.2.2> - ͨ�� SPI �ӿ�����ģ�鵱ǰ�� WIFI ����ģʽ��(STA, AP, or STA+AP)
 * @param op_mode Ҫ���õĲ���ģʽ��STA=1��AP=2��STA+AP=3��other=STA+AP��
 * @param saved   �Ƿ�Ѳ���ģʽд�뵽flash(=0�� �����浽ģ���ϵ� FLASH ��´�����ʱ��Ч��=1�� ���浽ģ���ϵ� FLASH ��´�����ʱ��Ȼ��Ч��)
 *               �벻Ҫ��ÿ������ʱ��ʹ�� Saved != 0 ������������ģ���ϵ�������ܻ�����д����������ڶ�ʧ��
 * @param status  ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣�������Ҫ����״̬�룬����ʹ�� NULL��
 * @return (u8)	success=1, has errors=0
 ***********************************************************************************/
u8 M8266WIFI_SPI_Set_Opmode(u8 op_mode, u8 saved, u16* status);


/***********************************************************************************
 * M8266WIFI_SPI_STA_Connect_Ap                                                    *
 * @brief <4.2.3> - ͨ�� SPI �ӿ���ģ������ȵ㣨����·��������
 * ��SSID������ͨ��SPI�ӿڣ��������ڴ��ڣ����ݸ�ģ�飬ģ�����յ����SSID������󣬿�ʼ���Ӷ�Ӧ���ȵ�AP��·������
 *			ssid - AP����������(wifi��) | passwd(wifi����) | �Ƿ񱣴�WiFi��/���� | �ȴ����ӵ��ʱ�䣨����Ϊ��λ��
 *			To connect the M8266WIFI STA to an AP or router via SPI                     *
 * @param ssid    : the ssid of AP connected to. Max len = 32 Bytes                *
 *                  use NULL if use last connected ssid(without reboot)            *
 * @param password: the password of AP connecting to. Max len = 64 Bytes           *
 *                  use NULL if use last connected ssid(without reboot)            *
 * @param saved   : to save the ssid and password into flash the opmode or not     *
 *        =0,       not saved, i.e. after reboot setting will restore to previous  *
 *        =others,  saved, i.e. after reboot, the saved setting will be loaded     *
 *                  PLEASE DO NOT CALL IT EACH TIME OF BOOTUP WITH SAVED != 0      *
 *                  OR, THE FLASH ON MODULE MIGHT GO TO FAILURE DUE TO LIFT CYCLE  *
 *                  OF WRITE                                                       *
 * @param timeout_in_s: max time in seconds waiting for being connected            *
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�룬�������Ҫ����״̬�룬����ʹ�� NULL��                       *
 *                  errcode(LSB)                                                   *
 *                    = 0x32, if connect timeout                                   *
 *                    = 0x4A, if SSID not found or not present                     *
 *                    = 0x4B, if SSID found but authentication failed,wrong password*
 *                    = 0x4C, if module in AP Only mode                            *
 *                    = 0x4D, if failed due to other reasons                       *
 * @return (u8)	success=1, has errors=0
 ***********************************************************************************/
u8 M8266WIFI_SPI_STA_Connect_Ap(u8 ssid[32], u8 password[64], u8 saved, u8 timeout_in_s, u16* status);

/***********************************************************************************
 * M8266WIFI_SPI_STA_DisConnect_Ap                                                 *
 * @brief  <4.2.4> - ͨ�� SPI �Ͽ�ģ��STAվ����AP��·����������
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣�������Ҫ����״̬�룬����ʹ�� NULL��
 * @return (u8)	success=1, has errors=0
 ***********************************************************************************/
u8 M8266WIFI_SPI_STA_DisConnect_Ap(u16* status);

/***********************************************************************************
 * M8266WIFI_SPI_Config_STA_StaticIpAddr                                           *
 * @brief <4.2.5> - ͨ�� SPI �ӿ�����ģ��(STA��)�ľ�̬IP��ַ��ִ�б� API �����󣬻�ر�ģ��� DHCPC��DHCP �ͻ��˷���
 * @param ip_addr : the ip address to set                                 		   * // Ҫ���õ�ip��ַ
 *                  - SHOULD be string like "192.168.1.103"                        *
 *                  - USE NULL if don't expect to set it                           *
 * @param gw_addr : the gateway address to set                             		   * // Ҫ���õ����ص�ַ
 *                  - SHOULD be string like "192.168.1.1"                          *
 *                  - USE NULL if don't expect to set it                           *
 * @param netmask : the netmask address to set                             		   * // Ҫ���õ������ַ
 *                  - SHOULD be string like "255.255.255.0"                        *
 *                  - USE NULL if don't expect to set it                           *
 * @param saved                                                                    *
 *             = 0,      config not saved, only this time valid                    *
 *             = others, config be saved, and valid upon next boot                 *
 *               if saved,  module bootup with dhcp diabled using stored           *
 *                          ip address                                             *
 *    PLEASE DO NOT CALL IT EACH TIME OF BOOTUP WITH SAVED != 0. OR, THE FLASH ON MODULE MIGHT GO TO FAILURE DUE TO LIFT CYCLE OF WRITE                                                       *
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣�������Ҫ����״̬�룬����ʹ�� NULL��
 * @return 1=success��0=has errors
 ***********************************************************************************/
u8 M8266WIFI_SPI_Config_STA_StaticIpAddr(char* ip_addr, char* gw_addr, char* netmask, u8 saved, u16* status);
//����ĺ����ڱ�������ǰ�� API ����
#define M8266WIFI_SPI_Config_STA_Static_IP_Addr(ip_addr, gw_addr, netmask, status)  M8266WIFI_SPI_Config_STA_StaticIpAddr(ip_addr,gw_addr,netmask,0,status)

/***********************************************************************************
 * M8266WIFI_SPI_Query_STA_Dhcpc         	                                         *
 * @brief <4.2.6> - ͨ��SPI��ѯģ��STA��dhcpc�Ƿ�ʹ��
 *     To query whether the dhcpc enabled or not of the module STA via SPI         *
 * @param enable: pointer to return whether diabled or not           				       *
 *             = 0, dhcpc disabled                                                 *
 *             = 1, dhcpc enabled                                                  *
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣   *
 *                  �������Ҫ����״̬�룬����ʹ�� NULL��                       *
 * @return (u8)	success=1, has errors=0
 ***********************************************************************************/
u8 M8266WIFI_SPI_Query_STA_Dhcpc(u8* enabled, u16* status);

/***********************************************************************************
 * M8266WIFI_SPI_Enable_STA_Dhcpc       	                                         *
 * @brief  <4.2.7> - ͨ�� SPI ���û����ģ�� STA �� dhcpc
 * @param enable
 *             = 0,      disable the dhcpc, and module uses static ip address      *
 *             = others, enable  the dhcpc, and module uses ip address via dhcp     *
 *     2  saved                                                                    *
 *             = 0,      config not saved, only this time valid                    *
 *             = others, config be saved, and valid upon next boot                 *
 *               kindly reminded that:                                             *
 *               - if saved=1 with enable=1, module bootup with dhcp enabled       *
 *               - if saved=1 with enable=0, module bootup with dhcp disabled      *
 *                 and using previous stored static ip address by                  *
 *                 M8266WIFI_SPI_Config_STA_StaticIpAddr with saved != 0           *
 *                 So, make sure an valid static ip address stored before          *
 *               -  PLEASE DO NOT CALL IT EACH TIME OF BOOTUP WITH SAVED != 0      *
 *                  OR, THE FLASH ON MODULE MIGHT GO TO FAILURE DUE TO LIFT CYCLE  *
 *                  OF WRITE                                                       *
 * @param status ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣�������Ҫ����״̬�룬����ʹ�� NULL��                       *
 * @return (u8)	success=1, has errors=0
 ***********************************************************************************/
u8 M8266WIFI_SPI_Enable_STA_Dhcpc(u8 enable, u8 saved, u16* status);

/***********************************************************************************
 * M8266WIFI_SPI_Get_STA_Connection_Status                                         *
 * @brief <4.2.8> - ͨ��SPI��ȡM8266WIFI STA��AP��·����������״̬
 *     To get the connecting status of M8266WIFI STA to AP or routers via SPI      *

 * @param connection_status :  the connection status returned                      *
 *          =0, if station is in idle                                              * // ����
 *          =1, if station is connecting to an AP                                  * // ��������
 *          =2, if station is to connect to an AP but with an wrong password       * // ����ʱ�������
 *          =3, if station is to connect to an AP which is not found               * // δ�ҵ������
 *          =4, if station failed to connect to the AP                             * // ����ʧ��
 *          =5, if station is connected to the AP and has got the IP successfully  * // ������AP���ɹ���ȡIP��ַ
 *          =255, if the module is in AP-only mode                                 * // ģ�鴦��AP-onlyģʽ
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣�������Ҫ����״̬�룬����ʹ�� NULL��                       *
 * @return (u8)	success=1, has errors=0
 ***********************************************************************************/
u8 M8266WIFI_SPI_Get_STA_Connection_Status(u8* connection_status, u16* status);
typedef enum{
	STA_AP_STATUS_IDLE = 0,
	STA_AP_STATUS_CONNECTING = 1,
	STA_AP_STATUS_PASSWORD = 2,
	STA_AP_STATUS_FOUND = 3,
	STA_AP_STATUS_FAILED = 4,
	STA_AP_STATUS_CONNECTED = 5,
}STA_AP_STATUS;

/***********************************************************************************
 * M8266WIFI_SPI_Get_STA_IP_Addr                                                   *
 * @brief  <4.2.9> - ͨ�� SPI �ӿڻ�ȡģ��� STA ģʽ IP ��ַ
 *     To get ip address of M8266WIFI STA via SPI                                  *

 * @param sta_ip :  the sta ip address returned if successful                      * // �ɹ����ص� sta ip ��ַ
 *                  "0.0.0.0" returned if in AP-only mode or ip not achieved       * // ��0.0.0.0������� AP-only ģʽ�·��ػ�ipδ�ﵽ
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣   *
 *                  �������Ҫ����״̬�룬����ʹ�� NULL��                       *
 * @return (u8)	success=1, has errors=0
 ***********************************************************************************/
u8 M8266WIFI_SPI_Get_STA_IP_Addr(char* sta_ip , u16* status);

/***********************************************************************************
 * M8266WIFI_SPI_STA_Query_Current_SSID_And_RSSI                                   *
 * @brief                                                                   *
 *     To get current AP info (SSID and RSSI) M8266WIFI STA connected to           *

 * @param ssid    : the current SSID returned which m8266wifi sta connected to     *
 * @param rssi    : the rssi of current connected ssid. 31 if error                *
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣   *
 *                  �������Ҫ����״̬�룬����ʹ�� NULL��                       *
 * @return value:                                                                  *
 *     =1, success                                                                 *
 *     =0, has error(s)                                                            *
 ***********************************************************************************/
// <4.2.10> - ��ȡM8266WIFI STA ���ӵ��ĵ�ǰ AP ��Ϣ��SSID �� RSSI��
u8 M8266WIFI_SPI_STA_Query_Current_SSID_And_RSSI(char* ssid, s8* rssi, u16* status);


//====================================================================================
//		STAģʽ  |  ɨ���ź�<4.2.11>-<4.2.14>  |  ����STA����<4.2.15>-<4.2.17>
//====================================================================================

/***********************************************************************************
 * M8266WIFI_SPI_STA_ScanSignals                                                   *
 * @brief                                                                   *
 *     To perform a scanning procedure, the scanned signals sorted by rssi         *

 * @param scanned_signals : the return signals after scanning                      * // ɨ���ķ����ź�
 * @param max_signals     : max counts of signals to scan                          * // Ҫɨ�������ź���
 * @param channel         : channel to scan.                                       * // ɨ���Ƶ��
 *                          - 0xFF, to scan all channles with default parameters   *
 *                            i.e. with below parameters overrided                 *
 *                             . show_hidden = 1, show hidden signals              *
 *                             . passive_not_active_scan = 0, active scan          *
 *                             . channel_scan_time_ms_max= 120                     *
 *                             . channel_scan_time_ms_min=  60                     *
 *                          - 0x00, to scan all channles with paramters specified  *
 *                               by this function                                  *
 *                          - others, to scan a channel with paramters specified   *
 *                               by this function                                  *
 *     4. show_hidden     : scan hidden signals or not                             * // �Ƿ�ɨ�������ź�
 *                          = 1,      scan hidden signals                          *
 *                          = others, not scan hidden signals                      *
 *     5. passive_not_active_scan                                                  *
 *                        : passive or active scan                                 *
 *                          = 1,      passive scan                                 * // ����ɨ��
 *                          = others, active  scan                                 * // ����ɨ��
 *     6. channel_scan_time_ms_max: max time in miliseconds for each channel scan  *
 *                          - if =0, 120ms will be used                            *
 *     7. channel_scan_time_ms_min: min time in miliseconds for each channel scan  *
 *                                                                                 *
 *     8. timeout_in_s    : time out in seconds                                    *
 *     9. status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣   *
 *                  �������Ҫ����״̬�룬����ʹ�� NULL��                       *
 *                  Upon failure with status!=NULL:                                *
 *                  errcode(LSB)=0x3E, if start scan failed                        *
 *                              =0x27, if last scan result has failure             *
 *                              =0x28, if scan timeout                             *
 *                              =0x29, other failure                               *
 * @return value:                                                                  *
 *    !=0, signal numbers scanned succesfully                                      *
 *     =0, has error(s)                                                            *
 ***********************************************************************************/
// <4.2.11> - ͨ�� SPI �ӿ�ɨ���ܱߵ��ȵ��ź�	
//		(1) Ϊ��ִ��ɨ�����ɨ����źŰ� rssi ����
//		(2) �����������ӿ�ģʽ2��ֻʹ�� SPI �ӿڲ�ʹ�ô��ڣ���
//		(3) ��ģ�鴦�� AP-Only ģʽʱ���ú����᷵�ش���
struct ScannedSigs{
	char ssid[32];	// �������������
	u8   channel;	// �ŵ� = Ƶ�� | (�ŵ�����bps �ŵ�����kHz) | (ͬһ�ŵ��ϵ��豸Խ�࣬WiFi�źŵ�ǿ��Խ��)
	u8   authmode;	// ��Ȩģʽ
	s8   rssi;
};
u8 M8266WIFI_SPI_STA_ScanSignals(struct ScannedSigs scanned_signals[], u8 max_signals, u8 channel, u8 show_hidden, u8 passive_not_active_scan, u32 channel_scan_time_ms_max, u32 channel_scan_time_ms_min, u8 timeout_in_s, u16* status);
// ����ĺ�������1.1.8-Q֮ǰ�ľ�API�Ĵ�����ݡ� ����ʹ��M8266WIFI_SPI_STA_ScanSignals����M8266WIFI_SPI_STA_Scan_Signals��
#define M8266WIFI_SPI_STA_Scan_Signals(scanned_signals, max_signals, channel, timeout_in_s, status) 	\
        M8266WIFI_SPI_STA_ScanSignals(scanned_signals,  max_signals , channel, 0, 0, 0, 0, timeout_in_s, status)

/***********************************************************************************
 * M8266WIFI_SPI_STA_Fetch_Last_Scanned_Signals                                    *
 * @brief <4.2.12> - ��ȡ�ϴ���ɨ�赫��û�б���Ƭ����������ȡ�ߵ��ź��б�
 * 					�����������ӿ�ģʽ 2��ֻʹ�� SPI �ӿڲ�ʹ�ô��ڣ��¡�                                                                  *
 *					(To Fetch the signals last scanned if not fetched before)
 * @param scanned_signals : the return signals after scanning                      *
 * @param max_signals     : max counts of signals to scan                          *
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣   *
 *                  �������Ҫ����״̬�룬����ʹ�� NULL��                       *
 *                  Upon failure with status!=NULL:                                *
 *                  errcode(LSB)=0x25, if not start scan before                    *
 *                              =0x26, if currently module is scanning             *
 *                              =0x27, if last scan result has failure             *
 *                              =0x29, other failure                               *
 * @return value:                                                                  *
 *    !=0, signal numbers fetched successfully                                      *
 *     =0, has error(s)                                                            *
 ***********************************************************************************/
u8 M8266WIFI_SPI_STA_Fetch_Last_Scanned_Signals(struct ScannedSigs scanned_signals[], u8 max_signals, u16* status);

/***********************************************************************************
 * u8 M8266WIFI_SPI_STA_ScanSignalsBySsid                                          *
 * @brief                                                                   *
 *     To fast scan the signal strength of specified SSID,                         *
 *     with rssi returned to facilitates applications of distance measurement       *
 * @param target_ssid  :  ssid to scan, e.g. "Anylinkin"                           *
 *                        max length of ssid string supported is 27                *
 * @param channel      :  channel to scan                                          *
 *                        = 0     scan all channels for the target ssid            *
 *                        = 1~13  scan only the specified channel                  *
 * @param rssi         : signal strength of target ssid                            *
 *                       If two or more signals of the specified ssid present,     *
 *                       then return the rssi of ssid of strongest signal          *
 *     4. status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣   *
 *                  �������Ҫ����״̬�룬����ʹ�� NULL��                       *
 * @return value:                                                                  *
 *    !=0, signal numbers scanned successfully                                      *
 *     =0, has error(s)                                                            *
 ***********************************************************************************/
// <4.2.13> - ����ɨ��ָ�� SSID ���ź�ǿ�ȣ�����rssi��������Ӧ��
u8 M8266WIFI_SPI_STA_ScanSignalsBySsid(char target_ssid[27+1], u8 channel, s8* rssi, u16* status);

/***********************************************************************************
 * u8 M8266WIFI_SPI_STA_ScanSignalsByBssid                                         *
 * @brief <4.2.14> - ����ɨ��ָ�� BSSID �ַ������ź�ǿ�ȣ�����rssi��������Ӧ��
 *     To fast scan the signal strength of s specified BSSID String,                *
 *     with rssi returned to facilitates applications of distance measurement       *
 * @param target_bssid :  bssid string to scan, e.g. "EE:C7:00:8A:1A:0C"           *
 *                        valid length of bssid string should be 17                *
 * @param channel      :  channel to scan                                          *
 *                        = 0     scan all channels for the target bssid           *
 *                        = 1~13  scan only the specified channel                  *
 * @param rssi         : signal strength of target bssid                           *
 *                       If two or more signals of the specified bssid present,    *
 *                       then return the rssi of ssid of strongest signal          *
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣(�������Ҫ����״̬�룬����ʹ�� NULL��)
 * @return u8 ��!=0, signal numbers scanned successfully | =0, has error(s)
 ************************************************************************************/
u8 M8266WIFI_SPI_STA_ScanSignalsByBssid(char target_bssid[17+1], u8 channel, s8* rssi, u16* status);

/**********************************************************************************
 * M8266WIFI_SPI_Get_STA_Hostname								                                  *
 * @brief <4.2.15> - ��ȡ STA �������������ģ�鴦�� AP-Only ģʽ��������ʧ��
 *     To get the hostname of the STA via SPI                                     *
 *     Note: Will return failure if module in AP-Only Mode                        *
 * @param																																	*
 * @param hostname 		: the hostame to get  		       														*
 * @param status  		: pointer to the status upon failure												*
 *                      �������Ҫ����״̬�룬����ʹ�� NULL��                  * 
 * @return value:																																	*
 *     =1, success																																*
 *     =0, has error(s)																														*
 **********************************************************************************/
u8 M8266WIFI_SPI_Get_STA_Hostname(char hostname[28+1], u16* status);

/**********************************************************************************
 * M8266WIFI_SPI_Set_STA_Hostname								                                  *
 * @brief																																	*
 *     To set up the hostname of the STA via SPI                                  *
 *     Note: Will return failure if module in AP-Only Mode                        *
 * @param																																	*
 * @param hostname 		: the hostame to setup		       														*
 * @param status  		: pointer to the status upon failure												*
 *                      �������Ҫ����״̬�룬����ʹ�� NULL��                  * 
 * @return value:																																	*
 *     =1, success																																*
 *     =0, has error(s)																														*
 **********************************************************************************/
// <4.2.16> - ���� STA �������������ģ�鴦�� AP-Only ģʽ��������ʧ��
u8 M8266WIFI_SPI_Set_STA_Hostname(char hostname[28+1], u16* status);

/***********************************************************************************
 * M8266WIFI_SPI_Query_STA_Param                                                   *
 * @brief                                                                   *
 *     To query a parameter of the module STA via SPI                              *

 * @param param_type : the param type to set, enum of STA_PARAM_TYPE      		   *
 *                      STA_PARAM_TYPE_SSID					= 0,                   *
 *                      STA_PARAM_TYPE_PASSWORD			= 1,                       *
 *                      STA_PARAM_TYPE_CHANNEL			= 2,                       *
 *                      STA_PARAM_TYPE_HOSTNAME			= 3,                       *
 *						STA_PARAM_TYPE_USE_BSSID		= 4,					   *
 *                      STA_PARAM_TYPE_BSSID			= 5,					   *
 *						STA_PARAM_TYPE_RSSI				= 6,					   *
 *						STA_PARAM_TYPE_IP_ADDR			= 7,					   *
 *                      STA_PARAM_TYPE_GATEWAY_ADDR		= 8,                       *
 *                      STA_PARAM_TYPE_NETMASK_ADDR		= 9,                       * 
 *                     	STA_PARAM_TYPE_MAC				= 11,                      *
 * @param param      : pointer to the param value returned                         *
 * @param param_len  : pointer to length the param, unit in bytes                  * // ָ��������ȵ�ָ�룬���ֽ�Ϊ��λ
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣 (�������Ҫ����״̬�룬����ʹ�� NULL��)
 * @return u8 (1=success, 0=has errors)
 ***********************************************************************************/
// <4.2.17> - ��ѯģ�� STA �Ĳ���
typedef enum{
	STA_PARAM_TYPE_SSID				= 0,
	STA_PARAM_TYPE_PASSWORD			= 1,
	STA_PARAM_TYPE_CHANNEL			= 2,
	STA_PARAM_TYPE_HOSTNAME			= 3,
	STA_PARAM_TYPE_USE_BSSID		= 4,
	STA_PARAM_TYPE_BSSID			= 5,
	STA_PARAM_TYPE_RSSI				= 6,
	STA_PARAM_TYPE_IP_ADDR			= 7,
	STA_PARAM_TYPE_GATEWAY_ADDR		= 8,
	STA_PARAM_TYPE_NETMASK_ADDR		= 9,
	STA_PARAM_TYPE_DHCPC			= 10,
	STA_PARAM_TYPE_MAC				= 11,
}STA_PARAM_TYPE;
u8 M8266WIFI_SPI_Query_STA_Param(STA_PARAM_TYPE param_type, u8* param, u8* param_len, u16* status);



//====================================================================================
//		APģʽ  |  ���á���ѯ��<4.2.18>-<4.2.21>
//====================================================================================

/***********************************************************************************
 * M8266WIFI_SPI_Config_AP                                                         *
 * @brief                                                                   *
 *     To config the module AP via SPI                                             *

 * @param ssid    : the ssid string of AP, Max Size=27 Bytes                       * // AP��ssid�ַ�����Max Size=27 Bytes
 * @param password: the password string of AP, Max Size=27 Bytes                   * // AP�������ַ�����Max Size=27 Bytes
 * @param enc     : authorisation mode, valid value is 0~4                         * // ��Ȩ��ʽ����ЧֵΪ0~4
 *                  = 0, OPEN                                                      *
 *                  = 1, WEP                                                       *
 *                  = 2, WPA_PSK                                                   *
 *                  = 3, WPA2_PSK                                                  *
 *                  = 4, WPA_WPA2_PSK                                              *
 *                  = others, OPEN                                                 *
 * @param channel: the channel of AP, valid value is 1~13.                         *
 *        - Value other than 1~13 will set to channel 6                            *
 * @param saved   : to save the ssid and password into flash the opmode or not     *
 *        =0,       not saved, i.e. after reboot setting will restore to previous  *
 *        =others,  saved, i.e. after reboot, the saved setting will be loaded     *
 *                  PLEASE DO NOT CALL IT EACH TIME OF BOOTUP WITH SAVED != 0      *
 *                  OR, THE FLASH ON MODULE MIGHT GO TO FAILURE DUE TO LIFT CYCLE  *
 *                  OF WRITE                                                       *
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣   *
 *                  �������Ҫ����״̬�룬����ʹ�� NULL��                       *
 * @return u8 (1=success, 0=has errors)
 ***********************************************************************************/
// <4.2.18> - ����ģ���APģʽ
u8 M8266WIFI_SPI_Config_AP(u8 ssid[27+1], u8 password[27+1], u8 enc, u8 channel, u8 saved, u16* status);

/***********************************************************************************
 * M8266WIFI_SPI_Query_AP_Param                                                    *
 * @brief                                                                   *
 *     To query a parameter of the module AP via SPI                               *

 * @param param_type : the param type to set, enum of AP_PARAM_TYPE       		   *
 *                      AP_PARAM_TYPE_SSID 				= 0,					   *
 *                      AP_PARAM_TYPE_PASSWORD 			= 1,					   *
 *                      AP_PARAM_TYPE_CHANNEL 			= 2,					   *
 *                      AP_PARAM_TYPE_AUTHMODE 			= 3,					   *
 *                      AP_PARAM_TYPE_SSID_HIDDEN 		= 4,					   *
 *                      AP_PARAM_TYPE_MAX_CONNECT 		= 5,					   *
 *                      AP_PARAM_TYPE_BEACON_INTERVAL 	= 6,					   *
 *                      AP_PARAM_TYPE_IP_ADDR			= 7,					   *
 *                      AP_PARAM_TYPE_GATEWAY_ADDR	  	= 8,                       *
 *                      AP_PARAM_TYPE_NETMASK_ADDR	  	= 9,                       *
 *                      AP_PARAM_TYPE_PHY_MODE			= 10,					   *
 *                      AP_PARAM_TYPE_MAC				= 11					   *
 * @param param      : pointer to the param value returned                         *
 * @param param_len  : pointer to length the param, unit in bytes                  *
 *     4. status     : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣*
 *                     �������Ҫ����״̬�룬����ʹ�� NULL��                    *
 * @return u8 (1=success, 0=has errors)
 ***********************************************************************************/
// <4.2.19> - ��ѯģ��AP�Ĳ���
typedef enum{
	AP_PARAM_TYPE_SSID 				= 0,
	AP_PARAM_TYPE_PASSWORD 			= 1,
	AP_PARAM_TYPE_CHANNEL 			= 2,
	AP_PARAM_TYPE_AUTHMODE 			= 3,
	AP_PARAM_TYPE_SSID_HIDDEN 		= 4,
	AP_PARAM_TYPE_MAX_CONNECT 		= 5,
	AP_PARAM_TYPE_BEACON_INTERVAL 	= 6,	
	AP_PARAM_TYPE_IP_ADDR			= 7,
	AP_PARAM_TYPE_GATEWAY_ADDR	  	= 8,
	AP_PARAM_TYPE_NETMASK_ADDR	  	= 9,
	AP_PARAM_TYPE_PHY_MODE			= 10,
	AP_PARAM_TYPE_MAC				= 11
} AP_PARAM_TYPE;
u8 M8266WIFI_SPI_Query_AP_Param(AP_PARAM_TYPE param_type, u8* param, u8* param_len, u16* status);


/***********************************************************************************
 * M8266WIFI_SPI_Config_AP_Param                                                   *
 * @brief                                                                   *
 *     To config a parameter of the module AP via SPI                              *

 * @param param_type : the param type to set, enum of AP_PARAM_TYPE       		   *
 *                      AP_PARAM_TYPE_SSID 				= 0,				  	   *
 *                      AP_PARAM_TYPE_PASSWORD 			= 1,				  	   *
 *                      AP_PARAM_TYPE_CHANNEL 			= 2,				  	   *
 *                      AP_PARAM_TYPE_AUTHMODE 			= 3,				  	   *
 *                      AP_PARAM_TYPE_SSID_HIDDEN 		= 4,					   *
 *                      AP_PARAM_TYPE_MAX_CONNECT 		= 5,					   *
 *                      AP_PARAM_TYPE_BEACON_INTERVAL 	= 6,					   *
 *                      AP_PARAM_TYPE_IP_ADDR			= 7						   *
 *                      AP_PARAM_TYPE_GATEWAY_ADDR	  	= 8,                       *
 *                      AP_PARAM_TYPE_NETMASK_ADDR	  	= 9,                       * 
 * @param param      : pointer to the param value                                  *
 * @param param_len  : length the param, unit in bytes                             *
 *     4. saved   : to save the param into flash or not								*
 *        =0,       not saved, i.e. after reboot setting will restore to previous  *
 *        =others,  saved, i.e. after reboot, the saved setting will be loaded     *
 *                  PLEASE DO NOT CALL IT EACH TIME OF BOOTUP WITH SAVED != 0      *
 *                  OR, THE FLASH ON MODULE MIGHT GO TO FAILURE DUE TO LIFT CYCLE  *
 *                  OF WRITE                                                       *
 *     5. status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣   *
 *                  �������Ҫ����״̬�룬����ʹ�� NULL��                       *
 * @return u8 (1=success, 0=has errors)
 ***********************************************************************************/
// <4.2.20> - ͨ��SPI ���� ģ��AP�Ĳ���
u8 M8266WIFI_SPI_Config_AP_Param(AP_PARAM_TYPE param_type, u8* param, u8 param_len, u8 saved, u16* status);

/***********************************************************************************
 * M8266WIFI_SPI_AP_List_STAs_Info                                                 *
 * @brief                                                                   *
 *     To list the STAs connected to the moduel AP                                 *

 * @param sta_list   : the array of struct CONNECTED_STATION_INFO                  *
 *                     which will store the returned station infos                 *
 * @param stas       : sum of return stations                                      *
 * @param max_states : max number of stations to returned                          *
 *     4. status     : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣*
 *                     �������Ҫ����״̬�룬����ʹ�� NULL��                    *
 *                      - LSB = 0x2A: module does not include ap mode              *
*                             = 0x2C: other failures                               *
 * @return value:                                                                  *
 *     =1, success                                                                 *
 *     =0, has error(s)                                                            *
 ***********************************************************************************/
struct CONNECTED_STATION_INFO{
	u8 bssid[6];
	u8 ipaddr[4];
};
// <4.2.21> - �г����ӵ�ģ��AP��STAվ��
u8 M8266WIFI_SPI_AP_List_STAs_Info(struct CONNECTED_STATION_INFO sta_list[], u8* stas, u8 max_stas, u16* status);



//====================================================================================
//		��ȫ���  |  <4.2.22>-<4.2.24>
//====================================================================================

/***********************************************************************************
 * M8266WIFI_SPI_AP_Permit_Block_A_STA                                             *
 * @brief                                                                   *
 *     To permit or block the connection of a STA by STA's BSSID/MAC               *

 * @param block_not_permit: permit or block the conecttion of the sta              *
 *                  = 0, if to permit                                              *
 *                  = others, if to block                                          *
 * @param bssid   : the bssid of the sta to be removed                             *
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣   *
 *                  �������Ҫ����״̬�룬����ʹ�� NULL��                       *
 * @return value:                                                                  *
 *     =1, success                                                                 *
 *     =0, has error(s)                                                            *
 ***********************************************************************************/
// <4.2.22> - ͨ�� STA �� BSSID/MAC �������ֹ STA ������
u8 M8266WIFI_SPI_AP_Permit_Block_A_STA(u8 block_not_enable, u8 bssid[6], u16* status);

typedef enum{
	AP_MAC_LIST_PERIMIT_BLOCK_TYPE_NONE 	= 0,
	AP_MAC_LIST_PERIMIT_BLOCK_TYPE_PERMIT 	= 1,	
	AP_MAC_LIST_PERIMIT_BLOCK_TYPE_BLOCK 	= 2,	
} AP_MAC_LIST_PERIMIT_BLOCK_TYPE;


/***********************************************************************************
 * M8266WIFI_SPI_AP_Config_PermitBlock_List                                        *
 * @brief                                                                   *
 *     To config the Permitted or Blocked MAC list to the AP                       *

 * @param block_permit_type   : the list type                                      *
 *               = 1,  the list is permitted list                                  *
 *               = 2,  the list is blocked list                                    *
 *               = others,  neither block nor permit                               *
 * @param mac_list            : The permitted/blocked mac list to be configured    *
 * @param mac_list_count      : The size of the permitted/blocked mac list         *
 *     4. status    : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣 *
 *                    �������Ҫ����״̬�룬����ʹ�� NULL��                     *
 * @return value:                                                                  *
 *     =1, success                                                                 *
 *     =0, has error(s)                                                            *
 ***********************************************************************************/
// <4.2.23> - Ϊ AP ���� ����Ļ���ֹ�� MAC �б�
u8 M8266WIFI_SPI_AP_Config_PermitBlock_List(AP_MAC_LIST_PERIMIT_BLOCK_TYPE block_permit_type, u8 mac_list[][6], u8 mac_list_count, u16* status);

/***********************************************************************************
 * M8266WIFI_SPI_AP_Query_PermitBlock_List                                         *
 * @brief                                                                   *
 *     To query the Permitted or Blocked MAC list to the AP                       *

 * @param block_permit_type   : pointer to the list type                           *
 *               = 1,  the list is permitted list                                  *
 *               = 2,  the list is blocked list                                    *
 *               = others,  neither block nor permit                               *
 * @param mac_list            : The permitted/blocked mac list                     *
 * @param mac_list_count      : The pointer to return the size of mac list         *
 *     4. status    : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣 *
 *                    �������Ҫ����״̬�룬����ʹ�� NULL��                     *
 * @return value:                                                                  *
 *     =1, success                                                                 *
 *     =0, has error(s)                                                            *
 ***********************************************************************************/
// <4.2.24> - ��ѯ AP����Ļ���ֹ�� MAC �б�
u8 M8266WIFI_SPI_AP_Query_PermitBlock_List(AP_MAC_LIST_PERIMIT_BLOCK_TYPE *block_permit_type, u8 mac_list[][6], u8* mac_list_count, u16* status);












//====================================================================================
//				UDP �� TCP ����/���ӵĽ������ѯ����
//====================================================================================		
//----------------------------------------------------------------------------------
//   3. APIs related to Module WIFI TCP/UDP Connection Operations via SPI        //
//       - M8266WIFI_SPI_Setup_Connection                                4.3.1    // ��������
//       - M8266WIFI_SPI_DisConnect_Connection                           4.3.2    // �Ͽ�����
//       - M8266WIFI_SPI_Delete_Connection                               4.3.3    // ɾ������
//       - M8266WIFI_SPI_Query_Connection                                4.3.4    // ��ѯ����
//                                                                                //
//       - M8266WIFI_SPI_Op_Multicuast_Group							 4.3.5    //
//                                                                                //
//       - M8266WIFI_SPI_Set_TcpServer_Auto_Discon_Timeout               4.3.6    //
//       - M8266WIFI_SPI_Query_Max_Clients_Allowed_To_A_Tcp_Server       4.3.7    //
//       - M8266WIFI_SPI_Config_Max_Clients_Allowed_To_A_Tcp_Server      4.3.8    //
//       - M8266WIFI_SPI_List_Clients_On_A_TCP_Server                    4.3.9    //
//       - M8266WIFI_SPI_Disconnect_TcpClient                            4.3.10   //

//       - M8266WIFI_SPI_Query_Last_Tcp_Disconnect_Cause                 4.3.11   //
//       - M8266WIFI_SPI_Query_Tcp_Send_Retry_Max                        4.3.12   //
//       - M8266WIFI_SPI_Config_Tcp_Retran_Max                           4.3.13   //
//       - M8266WIFI_SPI_Query_Tcp_Mss                                   4.3.14   //
//       - M8266WIFI_SPI_Query_Tcp_Window_num                            4.3.15   //
//       - M8266WIFI_SPI_Config_Tcp_Window_num                           4.3.16   //
//                                                                                //
//       - SPI_STA_Get_HostIP_by_HostName                                4.3.17   //
//
//----------------------------------------------------------------------------------

/***********************************************************************************
 * M8266WIFI_SPI_Setup_Connection                                                  *
 * @brief                                                                   *
 *     To setup a UDP connection or an TCP client connection via SPI               *

 * @param tcp_udp      : connection type                                           *
 *                       =0, udp                                                   *
 *                       =1, tcp client                                            *
 *                       =2, tcp server                                            *
 * @param local_port   : local_port specified                                      *
 *                       =0, M8266WIFI module will generate a random local port    *
 *                      !=0, the local_port specified here will be used            *
 * @param remote_addr  : string of ip or dns address of the remote connection      *
 *     4. remote_port  : port of remote connection                                 *
 *     5. link_no      : the number of link used for multiple links. Max 4         *
 *     6. timeout_in_s : the max timeout connecting to a remote, unit in seconds   *
 *     7. status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣   *
 *                  �������Ҫ����״̬�룬����ʹ�� NULL��                       *
 * @return value:                                                                  *
 *     =1, success                                                                 *
 *     =0, has error(s)                                                            *
 ***********************************************************************************/
#define SOCKET_TYPE_UDP 									0
#define SOCKET_TYPE_TCP_CLIENT 								1
#define SOCKET_TYPE_TCP_SERVER 								2
#define SOCKET_TCP_CLIENT_BITMAP_SSL 						0x40
#define SOCKET_TCP_CLIENT_BITMAP_CLIENT_CERT_REQUEST_EN 	0x20 
#define SOCKET_TCP_CLIENT_BITMAP_CA_CERT_EN 				0x10
// <4.3.1> - ͨ�� SPI ���� UDP ���ӻ� TCP �ͻ�������
u8 M8266WIFI_SPI_Setup_Connection(u8 tcp_udp, u16 local_port, char* remote_addr, u16 remote_port, u8 link_no, u8 timeout_in_s, u16* status);
																	
/***********************************************************************************
 * M8266WIFI_SPI_Disconnect_Connection                                             *
 * @brief                                                                   *
 *     To disconnect the connections on M8266WIFI TCP server via SPI               *

 * @param link_no : the number of link to be deleted/disconnected                  *
 * @param status  : pointer to the status upon failure                             *
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣   *
 *                  �������Ҫ����״̬�룬����ʹ�� NULL��                       *
 *                  errcode(LSB)                                                   *
 *                    = 0x54, disconnect a socket not present                      *
 *                    = 0x55, disconnect a TCP socket when busy in receiving       *
 *                    = 0x56, disconnect a TCP socket when busy in reading         *
 *                    = 0x57, no need to disconnect the sockets since it is        *
 *                            - UDP, using M8266WIFI_SPI_Delete_Connection, or     *
 *                            - TCP Server without clients connecting to, or       *
 *                            - TCP Client already disconnected                    *
 * @return value:                                                                  *
 *     =1, success                                                                 *
 *     =0, has error(s)                                                            *
 ***********************************************************************************/
// <4.3.2> - ͨ�� SPI �Ͽ� M8266WIFI (TCP ������)�ϵ�����
u8 M8266WIFI_SPI_Disconnect_Connection(u8 link_no, u16* status);

/***********************************************************************************
 * M8266WIFI_SPI_Delete_Connection                                                 *
 * @brief                                                                   *
 *     To disconnect and delete a client connection on M8266WIFI via SPI           *

 * @param link_no : the number of link to be deleted/disconnected                  *
 * @param status  : pointer to the status upon failure                             *
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣   *
 *                  �������Ҫ����״̬�룬����ʹ�� NULL��                       *
 *                  errcode(LSB)                                                   *
 *                    = 0x54, delete a socket not present                          *
 *                    = 0x55, delete a socket when busy in receiving               *
 *                    = 0x56, delete a socket when busy in reading                 *
 * @return value:                                                                  *
 *     =1, success                                                                 *
 *     =0, has error(s)                                                            *
 ***********************************************************************************/
// <4.3.3> - ͨ�� SPI �Ͽ���ɾ�� M8266WIFI �ϵ�(�ͻ���)����
 u8 M8266WIFI_SPI_Delete_Connection(u8 link_no, u16* status);

/***********************************************************************************
 * M8266WIFI_SPI_Query_Connection                                                  *
 * @brief    ͨ�� SPI ��ѯ M8266WIFI ��������Ϣ							   *
 *     To Query the information about a connection on M8266WIFI via SPI            *
 *     including connection type, state, local port, remote_ip and port, etc       *

 * @param link_no 				: the number of link to query                            *
 * @param connection_type	:	pointer to the connection type returned                *
 *                          If you don't expected this value returned, use NULL    *
 *                          = 0, udp                                               *
 *                          = 1, tcp Client                                        *
 *                          = 2, tcp Server                                        *
 *                          = others, invalid                                      *
 *                          �������Ҫ����״̬�룬����ʹ�� NULL��               *
 * @param connection_state:	pointer to the connection state returned               *
 *                          = 0, invalid                                           *
 *                          = 1, wait                                              *
 *                          = 2, listen                                            *
 *                          = 3, connect                                           *
 *                          = 4, write                                             *
 *                          = 5, read                                              *
 *                          = 6, close                                             *
 *                          = others, invalid                                      *
 *                          �������Ҫ����״̬�룬����ʹ�� NULL��               *
 *     4. local_port      : pointer to the local port returned                     *
 *                          �������Ҫ����״̬�룬����ʹ�� NULL��               *
 *     5. remote_ip       : pointer to the remote ip returned,                     *
 *                          - String like "192.168.4.2"                            *
 *                          - �������Ҫ����״̬�룬����ʹ�� NULL��             *
 *     6. remote_port     : pointer to the remote port returned                    *
 *                          �������Ҫ����״̬�룬����ʹ�� NULL��               *
 *     7. status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣   *
 *                  �������Ҫ����״̬�룬����ʹ�� NULL��                       *
 * @return value:                                                                  *
 *     =1, success                                                                 *
 *     =0, has error(s)                                                            *
 ***********************************************************************************/
// <4.3.4> - ͨ�� SPI ��ѯ M8266WIFI ��������Ϣ
u8 M8266WIFI_SPI_Query_Connection(u8 link_no, u8* connection_type, u8* connection_state,
												u16* local_port, u8* remote_ip, u16* remote_port, u16* status);
typedef enum{
	ANYLINKIN_WIFI_LINK_CONNECTION_STATE_INVALID  =	0,
	ANYLINKIN_WIFI_LINK_CONNECTION_STATE_WAIT	  =	1,
	ANYLINKIN_WIFI_LINK_CONNECTION_STATE_LISTEN	  =	2,
	ANYLINKIN_WIFI_LINK_CONNECTION_STATE_CONNECT  =	3,
	ANYLINKIN_WIFI_LINK_CONNECTION_STATE_WRITE    =	4,
	ANYLINKIN_WIFI_LINK_CONNECTION_STATE_READ     =	5,
	ANYLINKIN_WIFI_LINK_CONNECTION_STATE_CLOSE    =	6
} ANYLINKIN_WIFI_LINK_CONNECTION_STATES;

/***********************************************************************************
 * M8266WIFI_SPI_Op_Multicuast_Group                                               *
 * @brief                                                                   *
 *     To join or leave a multicuast groupo AP via SPI                             *
 *     The module should be in STA or STA+AP mode and have connected               *
 *     to a routers before call this API                                           *

 * @param join_not_leave : join or leave the multicuast groud       				       *
 *        =1, join                                          											 *
 *        =0, leave                                          											 *
 * @param multicust_group_ip : the ip address of a multicuast groud       				 *
 *        e.g. "224.6.6.6".                                                        *
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣   *
 *                  �������Ҫ����״̬�룬����ʹ�� NULL��                       *
 * @return value:                                                                  *
 *     =1, success                                                                 *
 *     =0, has error(s)                                                            *
 ***********************************************************************************/
// <4.3.5> - 
u8 M8266WIFI_SPI_Op_Multicuast_Group(u8 join_not_leave, char multicust_group_ip[15+1], u16* status);

/************************************************************************************
 * M8266WIFI_SPI_Set_TcpServer_Auto_Discon_Timeout                              	*
 * @brief																	*
 *     To set up the tcp server's auto-disconnection timeout time 					*
 *     when no communication from clients via SPI									*
 * @param																	*
 * @param link_no 		: the number of link to setup								*
 * @param timeout_in_s: the timeout in seconds the tcp server will auto 			*
 *                      disconnect the connection when no communication				*
 *										  from clients								*
 * @param status  		: pointer to the status upon failure						*
 * @return value:																	*
 *     =1, success																	*
 *     =0, has error(s)																*
 **********************************************************************************/
// <4.3.6> - 
u8 M8266WIFI_SPI_Set_TcpServer_Auto_Discon_Timeout(u8 link_no, u16 timeout_in_s, u16* status);

/**********************************************************************************
 * M8266WIFI_SPI_Query_Max_Clients_Allowed_To_A_Tcp_Server                        *
 * @brief																																	*
 *     To query the max tcp clients could be accepted simutenoeously to           *
 *     this tcp server                                           					  			*
 * @param																																	*
 * @param server_link_no: the number of tcp server link to query		  						*
 * @param max_allowed   : pointer to max allowed                                  *
 * @param status    		: pointer to the status upon failure											*
 * @return value:																																	*
 *     =1, success																																*
 *     =0, has error(s)																														*
 **********************************************************************************/
// <4.3.7> - 
u8 M8266WIFI_SPI_Query_Max_Clients_Allowed_To_A_Tcp_Server(u8 server_link_no, u8* max_allowed, u16* status);
 
/**********************************************************************************
 * M8266WIFI_SPI_Config_Max_Clients_Allowed_To_A_Tcp_Server                       *
 * @brief <4.3.8> - To Config the max tcp clients could be accepted simultaneously to this tcp server                                           					  			*
 * @param server_link_no: the number of tcp server link to config		  						*
 * @param max_allowed   : max allowed, range 1~15                                 *
 * @param status    		: pointer to the status upon failure											*
 * @return value:																																	*
 *     =1, success																																*
 *     =0, has error(s)																														*
 **********************************************************************************/
u8 M8266WIFI_SPI_Config_Max_Clients_Allowed_To_A_Tcp_Server(u8 server_link_no, u8 max_allowed, u16* status);

typedef struct {
	  u8  connection_state;
	  u8  remote_ip[4];
	  u16 remote_port;
 }ClientInfo;
/***********************************************************************************
 * M8266WIFI_SPI_List_Clients_On_A_TCP_Server                                      *
 * @brief  <4.3.9> - To Query the information of clients on a link of tcp server of M8266WIFI    *
 * @param server_link_no  : the link number of a tcp server to query               *
 * @param clients        	:	pointer to sum of clients listed                       *
 *                          value range: 0~15                                      *
 * @param pRemoteClients  :	array of struct clients_info the clients returned      *
 * @param status          : pointer to return errcode(LSB) and status(MSB)         *
 *                          upon exception(s) of error(s)                          *
 *                          �������Ҫ����״̬�룬����ʹ�� NULL��               *
 * @return value:                                                                  *
 *     =1, success                                                                 *
 *     =0, has error(s)                                                            *
 ***********************************************************************************/
u8 M8266WIFI_SPI_List_Clients_On_A_TCP_Server(u8 server_link_no, u8* clients, ClientInfo RemoteClients[], u16* status);

/***********************************************************************************
 * M8266WIFI_SPI_Disconnect_TcpClient                                              *
 * @brief <4.3.10> - To disconnect a client connections on M8266WIFI TCP server via SPI          *
 * @param link_no     : the number of link to be deleted/disconnected              *
 * @param client_info : pointer to the remote tcp client info to disconnect        *
 *                      use NULL to disconnect all the remote clients if any       *
 * @param status      : pointer to return errcode(LSB) and status(MSB) upon error  *
 *                      �������Ҫ����״̬�룬����ʹ�� NULL��                   *
 *                      errcode(LSB)                                               *
 *                      = 0x54, disconnect a tcp client socket not present         *
 *                      = 0x55, disconnect a TCP socket when busy in receiving     *
 *                      = 0x56, disconnect a TCP socket when busy in reading       *
 *                      = 0x57, no need to disconnect the sockets since it is      *
 *                            - UDP, use M8266WIFI_SPI_Delete_Connection. OR       *
 *                            - TCP Server without clients connecting to. OR       *
 *                            - TCP Client already disconnected                    *
 * @return (u8)	success=1, has errors=0                                                          *
 ***********************************************************************************/
u8 M8266WIFI_SPI_Disconnect_TcpClient(u8 link_no, ClientInfo *client_info, u16* status);
 

/**********************************************************************************
 * M8266WIFI_SPI_Query_Last_Tcp_Disconnect_Cause
 * @brief <4.3.11> - To query the cause of last tcp disconnect
 * @param link_no 			: the number of link to query															*
 * @param discon_cause pointer to the last disconnect cause
 *                        -3, if sending timeout
 *                        -9, if reset request by remote peer reset
 *                        -20,if disconnected by remote peer
 *                        -21,if disconnected by the local
 *                        -22,if deleted by the local
 * @param status  		  : pointer to the status upon failure
 * @return (u8)	success=1, has errors=0
 **********************************************************************************/
u8 M8266WIFI_SPI_Query_Last_Tcp_Disconnect_Cause(u8 link_no, s8* disconnect_cause, u16* status);

// <4.3.12> - 
u8 M8266WIFI_SPI_Disconnect_TcpClient(u8 link_no, ClientInfo *client_info, u16* status);

/**********************************************************************************
 * M8266WIFI_SPI_Query_Tcp_Retran_Max                                             *
 * @brief <4.3.12> - To query the tcp max retransmission                  					       			*
 * @param link_no 			: the number of link to query															*
 * @param max_retran    : pointer to max retransmission                           *
 * @param status  		  : pointer to the status upon failure											*
 *                        �������Ҫ����״̬�룬����ʹ�� NULL��                *
 * @return (u8)	success=1, has errors=0
 **********************************************************************************/
u8 M8266WIFI_SPI_Query_Tcp_Retran_Max(u8 link_no, u8* max_retran, u16* status);
 
/**********************************************************************************
 * M8266WIFI_SPI_Config_Tcp_Retran_Max
 * @brief <4.3.13> - To config the tcp max retransmission
 * @param link_no	the number of link to query
 * @param max_retran	value of max retransmission, range 1-12(if <1, then use 1;if >12, then use 12)
 * @param status	pointer to the status upon failure
 *                        �������Ҫ����״̬�룬����ʹ�� NULL��
 * @return  (u8)	success=1, has errors=0
 **********************************************************************************/
u8 M8266WIFI_SPI_Config_Tcp_Retran_Max(u8 link_no, u8 max_retran, u16* status);
 
/**********************************************************************************
 * M8266WIFI_SPI_Query_Tcp_Mss                                                    *
 * @brief	<4.3.14> - To query the tcp mss value
 * @param link_no 			: the number of link to query															*
 * @param remote_ip     : remote_ip like "192.168.1.101" to differentiate multiple*
 *                        remote e.g. multiple clients.                           *
 *                        USE NULL if only one remote(e.g. wifi module is a tcp   *
 *                        Client) or check current access remote                  *
 * @param remote_port   : remote_ip like 4321 if remote_ip!=NULL*                 *
 * @param tcp_mss       : pointer to returned mss                                 *
 * @param status  		  : pointer to the status upon failure											*
 *                        �������Ҫ����״̬�룬����ʹ�� NULL��                *
 * @return (u8)	success=1, has errors=0																														*
 **********************************************************************************/
u8 M8266WIFI_SPI_Query_Tcp_Mss(u8 link_no, char* remote_ip, u16 remote_port, u16* tcp_mss, u16* status);

/**********************************************************************************
 * M8266WIFI_SPI_Query_Tcp_Window_num                                             *
 * @brief	<4.3.15> -��ѯTCP����ֵ
 * @param link_no ��Ҫ��ѯ�ķ�������ͨ�������Ӻţ���ֵΪ 0-3��
 * @param tcp_wnd_num	TCP������ֵ
 * @param status  pointer to the status upon failure	(�������Ҫ����״̬�룬����ʹ�� NULL��)
 *                                       *
 * @return (u8)	success=1, has errors=0																														*
 **********************************************************************************/
u8 M8266WIFI_SPI_Query_Tcp_Window_num(u8 link_no, u8* tcp_wnd_num, u16* status);
 
/**********************************************************************************
 * M8266WIFI_SPI_Config_Tcp_Window_num                                            *
 * @brief <4.3.16> - ����TCP����ֵ
 * @param link_no ��Ҫ���õķ�������ͨ�������Ӻţ���ֵΪ 0-3
 * @param tcp_wnd_num   TCP����ֵ��0-15�����ֵ<1��ʹ��1�����>15��ʹ��15��
 * @param status  ָ��ʧ��״̬��ָ�루�������ϣ�������أ���ʹ�� NULL��
 * @return (u8)	success=1, has errors=0
 **********************************************************************************/
u8 M8266WIFI_SPI_Config_Tcp_Window_num(u8 link_no, u8 tcp_wnd_num, u16* status);

/***********************************************************************************
 * M8266WIFI_SPI_STA_Get_HostIP_by_HostName
 * @brief <4.3.17> - ͨ����������ȡ����IP��ַ
 * @param hostIp		the host ip_addr returned
 * @param hostName 		the host name to get ip, max 255 Bytes
 * @param timeout_in_s	timeout in seconds
 * @param status		pointer to return errcode(LSB) and status(MSB) upon error(�������Ҫ����״̬�룬����ʹ�� NULL��)
 * @return (u8)	success=1, has errors=0
 ***********************************************************************************/ 
u8 M8266WIFI_SPI_STA_Get_HostIP_by_HostName(char* hostIp, char* hostName, u8 timeout_in_s, u16* status);
//����ǰ�汾���ݵĺ�
#define SPI_STA_Get_HostIP_by_HostName(hostIp,hostName,timeout_in_s,status) M8266WIFI_SPI_STA_Get_HostIP_by_HostName(hostIp,hostName,timeout_in_s,status) 






//====================================================================================
//					TCP/UDP���ݰ����շ�
//====================================================================================
//----------------------------------------------------------------------------------
//   4. APIs releated to TCP/UDP Packets TXD and RXD via SPI                      //
//       - M8266WIFI_SPI_Send_Data                                       4.4.1    //
//       - M8266WIFI_SPI_Send_BlockData                                  4.4.2    //
//       - M8266WIFI_SPI_Send_Udp_Data                                   4.4.3    //
//       - M8266WIFI_SPI_Send_Data_to_TcpClient                          4.4.4    //

//       - M8266WIFI_SPI_Has_DataReceived                                4.4.5    //
//       - M8266WIFI_SPI_RecvData                                        4.4.6    //
//       - M8266WIFI_SPI_RecvData_ex                                     4.4.7    //
//----------------------------------------------------------------------------------

/***********************************************************************************
 * M8266WIFI_SPI_Send_Data                                                         *
 * @brief                                                                   *
 *     To send Data to WIFI via M8266 module SPI                                   *
 * .Parameters                                                                     *
 * @param Data   : the pointer to the Data buffer to be sent                       *
 * @param len    : the length the Data buffer to be sent                           *
 * @param link_no: the wifi service link number sent to                            *
 *     4. pointer to return errcode(LSB) and status(MSB) when error encountered    *
 *        use NULL if you don't expect errcode and status                          *
 *        errcode:                                                                 *
 *            = 0x10: timeout when wait Module spi rxd Buffer ready                *
 *            = 0x11: timeout when wait wifi to send data                          *
 *            = 0x12: Module Sending Buffer full                                   *
 *              Module Buffer full defined as                                      *
 *              - If total size of packets waiting to send via WIFI > 5*1024, or   *
 *              - If counts     of packets waiting to send via WIFI > 8            *
 *            = 0x13: Wrong link_no used                                           *
 *            = 0x14: connection by link_no not present                            *
 *            = 0x15: connection by link_no closed                                 *
 *            = 0x18: No clients connecting to this TCP server                     *
 *            = 0x1D: connection is being established, please wait                 *
 *            = 0x1F: Other errors                                                 *
 * @return value:                                                                  *
 *     Actually length that has been sent successfully                             *
 ***********************************************************************************/
// <4.4.1> - 
u16 M8266WIFI_SPI_Send_Data(u8 Data[], u16 Data_len, u8 link_no, u16* status);

/***********************************************************************************
 * M8266WIFI_SPI_Send_BlockData                                                    *
 * @brief                                                                   *
 *     To send Block Data to WIFI via M8266 module SPI                             *
 * .Parameters                                                                     *
 * @param Data			  : the pointer to the Data buffer to be sent              *
 * @param Data_len		  : the length the Data buffer to be sent                  *
 * @param link_no		  : the wifi service link number sent to                   *
 *     4. max_loops       : max loops to repeat send if blocked or failed during  *
 *                          transimission to avoid dead-loops, use 1000 as default *
 *     4. max_loops       : max loops to repeat send if blocked or failed during  *
 *     5. remote_ip       : remote_ip like "192.168.1.101" to differentiate        *
 *                          multiple remote e.g. multiple clients.                 *
 *                          USE NULL if only one remote(e.g. wifi module is a tcp  *
 *                          Client) or send to latest access/connected remote      *
 *     6. remote_port     : remote_ip like 4321 if remote_ip!=NULL                 *
 *                          use 0 if remote_ip==NULL                               *
 *     7. pointer to return errcode(LSB) and status(MSB) when error encountered    *
 *        use NULL if you don't expect errcode and status                          *
 *        errcode:                                                                 *
 *            = 0x13: Wrong link_no used                                           *
 *            = 0x14: connection by link_no not present                            *
 *            = 0x15: connection by link_no closed                                 *
 *            = 0x18: No clients connecting to this TCP server                     *
 *            = 0x1E: too many errors encountered during sending can not fixed      *
 *            = 0x1F: Other errors                                                 *
 * @return value:                                                                  *
 *     Actually length that has been sent successfully                             *
 ***********************************************************************************/
// <4.4.2> - 
u32 M8266WIFI_SPI_Send_BlockData(u8 Data[], u32 Data_len, u16 max_loops, u8 link_no, char* remote_ip, u16 remote_port, u16* status);															
//Below macro is used to be compatible with code with lagacy APIs. Recommended to Use M8266WIFI_SPI_Send_BlockData other than M8266WIFI_SPI_Send_Data_Block.
#define M8266WIFI_SPI_Send_Data_Block(Data, Data_len, tcp_packet_size, link_no, status)  M8266WIFI_SPI_Send_BlockData(Data[], Data_len, 10000, link_no, NULL, 0, status)

/***********************************************************************************
 * M8266WIFI_SPI_Send_Udp_Data                                                     *
 * @brief                                                                   *
 *     .To send Udp Data to WIFI via M8266 module SPI especially suitable for      *
 *      those UDP transmission requiring frequently changing destination           *
 *     .If the UDP transission does not need update dest, please use               *
 *       M8266WIFI_SPI_Send_Data above for better efficiency                       *
 * .Parameters                                                                     *
 * @param Data   : the pointer to the Data buffer to be sent                       *
 * @param len    : the length the Data buffer to be sent                           *
 * @param link_no: the wifi service link number sent to                            *
 *     4. udp_dest_addr: string of ip or dns address of the remote connection      *
 *     5. udp_dest_port: port of remote connection                                 *
 *     6. pointer to return errcode(LSB) and status(MSB) when error encountered    *
 *        use NULL if you don't expect errcode and status                          *
 *        errcode:                                                                 *
 *            = 0x10: timeout when wait Module spi rxd Buffer ready                *
 *            = 0x11: timeout when wait wifi to send data                          *
 *            = 0x12: Module Sending Buffer full                                   *
 *              Module Buffer full defined as                                      *
 *              - If total size of packets waiting to send via WIFI > 5*1024, or   *
 *              - If counts     of packets waiting to send via WIFI > 8            *
 *            = 0x13: Wrong link_no used                                           *
 *            = 0x14: connection by link_no not present                            *
 *            = 0x15: connection by link_no closed                                 *
 *            = 0x19: this link is not a UDP service                               *
 *            = 0x1A: try to point to a new udp dest but failed                    *
 *            = 0x1F: Other errors                                                 *
 * @return value:                                                                  *
 *     Actually length that has been sent successfully                             *
 ***********************************************************************************/
// <4.4.3> - 
u16 M8266WIFI_SPI_Send_Udp_Data(u8 Data[], u16 Data_len, u8 link_no, char* udp_dest_addr, u16 udp_dest_port, u16* status);

/***********************************************************************************
 * M8266WIFI_SPI_Send_Data_to_TcpClient                                             *
 * @brief                                                                   *
 *     .To send tcp Data to a tcp client from this  M8266 tcp server               *
 *       especially suitable for scenarios of multiple clients for                 *
 *     .If the tcp server has only one client, please use                          *
 *       M8266WIFI_SPI_Send_Data above for better efficiency                       *
 * .Parameters                                                                     *
 * @param Data   : the pointer to the Data buffer to be sent                       *
 * @param len    : the length the Data buffer to be sent                           *
 * @param link_no: the tcp server link number sent to                              *
 *     4. tcp_client_dest_addr: string of ip or dns address of the remote client   *
 *     5. tcp_client_dest_port: port of remote client                              *
 *     6. pointer to return errcode(LSB) and status(MSB) when error encountered    *
 *        use NULL if you don't expect errcode and status                          *
 *        errcode:                                                                 *
 *            = 0x10: timeout when wait Module spi rxd Buffer ready                *
 *            = 0x11: timeout when wait wifi to send data                          *
 *            = 0x12: Module Sending Buffer full                                   *
 *              Module Buffer full defined as                                      *
 *              - If total size of packets waiting to send via WIFI > 5*1024, or   *
 *              - If counts     of packets waiting to send via WIFI > 8            *
 *            = 0x13: Wrong link_no used                                           *
 *            = 0x14: connection by link_no not present                            *
 *            = 0x15: connection by link_no closed                                 *
 *            = 0x19: this link is not a TCP Server                                *
 *            = 0x1A: try to point to a client but failed. no this client          *
 *            = 0x1D: connection is being established, please wait                 *
 *            = 0x1F: Other errors                                                 *
 * @return value:                                                                  *
 *     Actually length that has been sent successfully                             *
 ***********************************************************************************/
// <4.4.4> - 
u16 M8266WIFI_SPI_Send_Data_to_TcpClient(u8 Data[], u16 Data_len, u8 server_link_no, char* tcp_client_dest_addr, u16 tcp_client_dest_port, u16* status);

/***********************************************************************************
 * M8266WIFI_SPI_Has_DataReceived                                                  *
 * @brief                                                                   *
 *     To check whether the M8266WIFI module has received data awaiting master     *
 *     to fetch away                                                               *
 * .Parameters                                                                     *
 *     None                                                                        *
 * @return value:                                                                  *
 *     1, if the M8266WIFI module has received data from wifi                      *
 *     0, if the M8266WIFI module has not received data from wifi                  *
 ***********************************************************************************/
// <4.4.5> - 
u8 M8266WIFI_SPI_Has_DataReceived(void);

/***********************************************************************************
 * M8266WIFI_SPI_RcvData                                                           *
 * @brief                                                                   *
 *     To receive the wifi data from M8266WIFI                                     *
 * .Parameters                                                                     *
 * @param Data           - the buffer to contained the received Data               *
 * @param max_len        - the max length of Data to fetch                        *
 * @param max_wait_in_ms - the max timeout to wait for the Data                    *
 *     4. link_no        - pointer to return the link_no that current wifi Data    *
 *                         come from. �������Ҫ����״̬�룬����ʹ�� NULL��     *
 *     5. status         - pointer to return errcode(LSB) and status(MSB)          *
 *                         when error encountered. Use NULL if you don't expect    *
 *                         it returned                                             *
 *        errcode:                                                                 *
 *            = 0x20: timeout when wait module has received data via WIFI          *
 *            = 0x22: no date in Module wifi receive buffer                        *
 *            = 0x23: Read data from the left of last packets in Module wifi       *
 *                    receive buffer                                               *
 *            = 0x24: The packet in the Module wifi receive buffer is larger       *
 *                    in size than the max_len specified here. Only part of        *
 *                    the packet received                                          *
 *            = 0x2F: Other errors                                                 *
 * @return value:                                                                  *
 *     - the size of larger packet first in the Module wifi receive buffer         *
 *       if errcode = 0x24                                                         *
 *     - the actual length of wifi packet received successfully                    *
 *       if others                                                                 *
 ***********************************************************************************/
// <4.4.6> - 
u16 M8266WIFI_SPI_RecvData(u8 Data[], u16 max_len, uint16_t max_wait_in_ms, u8* link_no, u16* status);

/***********************************************************************************
 * M8266WIFI_SPI_RcvData_ex                                                        *
 * @brief                                                                   *
 *     To receive the wifi data from M8266WIFI, extended                           *
 *     Compared with M8266WIFI_SPI_RcvData(), this function also return the        *
 *     source remote_ip and remote_port meanwhile                                  *
 * .Parameters                                                                     *
 * @param Data           - the buffer to contained the received Data               *
 * @param max_len        - the max length of Data to fetch                        *
 * @param max_wait_in_ms - the max timeout to wait for the Data                    *
 *     4. link_no        - pointer to return the link_no that current wifi Data    *
 *                         come from. �������Ҫ����״̬�룬����ʹ�� NULL��     *
 *     5. remote_ip      - array[4] to return the remote_ip that current wifi Data *
 *                         come from. �������Ҫ����״̬�룬����ʹ�� NULL��     *
 *                         e.g. if remote ip is "192.168.4.2", then remote_ip will *
 *                         return with remote_ip[0]=192, remote_ip[1]=168,         *
 *                         remote_ip[2]=4, and remote_ip[3]=2                      *
 *     6. remote_ip      - pointer to return the remote_port that current wifi Data*
 *                         come from. �������Ҫ����״̬�룬����ʹ�� NULL��     *
 *     7. status         - pointer to return errcode(LSB) and status(MSB)          *
 *                         when error encountered. Use NULL if you don't expect    *
 *                         it returned                                             *
 *        errcode:                                                                 *
 *            = 0x20: timeout when wait module has received data via WIFI          *
 *            = 0x22: no date in Module wifi receive buffer                        *
 *            = 0x23: Read data from the left of last packets in Module wifi       *
 *                    receive buffer                                               *
 *            = 0x24: The packet in the Module wifi receive buffer is larger       *
 *                    in size than the max_len specified here. Only part of        *
 *                    the packet received                                          *
 *            = 0x2F: Other errors                                                 *
 * @return value:                                                                  *
 *     - the size of larger packet first in the Module wifi receive buffer         *
 *       if errcode = 0x24                                                         *
 *     - the actual length of wifi packet received successfully                    *
 *       if others                                                                 *
 ***********************************************************************************/
// <4.4.7> - 
u16 M8266WIFI_SPI_RecvData_ex(u8 Data[], u16 max_len, uint16_t max_wait_in_ms, u8* link_no, u8 remote_ip[4], u16* remote_port, u16* status);









//====================================================================================
//					��������
//====================================================================================
//----------------------------------------------------------------------------------
//   5. APIs related to Smart Config via SPI                                     //
//       - M8266WIFI_SPI_DoModuleSmartConfig                             4.5.1    //
//       - M8266WIFI_SPI_StartModuleSmartConfig                          4.5.2    //
//                                                                                //
//----------------------------------------------------------------------------------

/***********************************************************************************
 * M8266WIFI_SPI_DoModuleSmartConfig                                               *
 * @brief                                                                   *
 *     To perform an SmartConfig procedure to wifi module via SPI                  *

 * @param timeout_in_s  : timeout for an smartconfig procedure                     *
 * @param saved   : to save ssid/password or not after configuration with en=1     *
 *                  =0, not save                                                   *
 *                  = others, save                                                 *
 * @param smartconfig_type   : pointer to smartconfig type returned                *
 *                    use NULL if you don't expect this param returned             *
 *                    = 0, if type is ESPTOUCH                                     *
 *                    = 1, if type is AIRKISS                                      *
 *                    =-1, if invalid or unknown                                   *
 *     4. smartconfig_phone_ip :                                                   *
 *                   - the ip of the phone or smart device                         *
 *                     that runs smartconfig app to broadcast ssid/password        *
 *                     e.g. "192.168.143.121"                                      *
 *                   - It should be a buffer at least of 16Bytes                   *
 *                   - use NULL if you don't expect this param returned            *
 *                   - Airkiss does not response ip addrsss of smart devices,      *
 *                     so smartconfig_phone_ip[0] will be 0 if airkiss             *
 *     5. status  : pointer to return errcode upon failures                        *
 *                  �������Ҫ����״̬�룬����ʹ�� NULL��                       *
 *                  == 0x0000  success                                             *
 *                  != 0x0000  failed                                              *
 *                     (1) failed to start smartconfig if LSB(8) = 0x68            *
 *                     (2) failed during smartconfig   if LSB(8) = 0x6A            *
 *                          - failed to find channel           if MSB(8) = 0x00    *
 *                          - failed to get ssid/password      if MSB(8) = 0x01    *
 *                          - failed to connect the ap/routers if MSB(8) = 0x02/03 *
 * @return value:                                                                  *
 *     =1, success                                                                 *
 *     =0, has error(s)                                                            *
 ***********************************************************************************/
// <4.5.1> - ͨ�� SPI �ӿ�ʹģ����롰��������ģʽ������ʱģ�齫��ʼ�ȴ�����
u8 M8266WIFI_SPI_DoModuleSmartConfig(u8 timeout_in_s, u8 saved, u8* smartconfig_type, char smartconfig_phone_ip[15+1], u16* status);

//Macro for Compatibility with those MCU codes using previous version of API which will save SSID and passwords
#define M8266WIFI_SPI_Module_SmartConfig(timeout_in_s,smartconfig_type,smartconfig_phone_ip,status) M8266WIFI_SPI_DoModuleSmartConfig(timeout_in_s,1,smartconfig_type,smartconfig_phone_ip,status)

/***********************************************************************************
 * M8266WIFI_SPI_StartModuleSmartConfig                                            *
 * @brief  <4.5.2> - To start/stop smart config of M8266WIFI via SPI.
 *     Once enter the smartconfig mode, either smartlinkin or airkiss be adapted   *
 *     After perform a start operation, the M8266WIFI will be set in STA mode      *
 * @param en      : to start or stop                                               *
 *                  =0, to stop                                                    *
 *                  = others, to start                                             *
 * @param saved   : to save ssid/password or not after configuration with en=1     *
 *                  =0, not save                                                   *
 *                  = others, save                                                 *
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣   *
 *                  �������Ҫ����״̬�룬����ʹ�� NULL��                       *
 * @return value:                                                                  *
 *     =1, success                                                                 *
 *     =0, has error(s)                                                            *
 ***********************************************************************************/
u8 M8266WIFI_SPI_StartModuleSmartConfig(u8 en, u8 saved, u16* status);

//Macro for Compatibility with those MCU codes using previous version of API which will save SSID and passwords��ʹ����ǰ�汾�� API ����Щ MCU ������ݵĺ꣬�⽫���� SSID ������
#define M8266WIFI_SPI_StartSmartConfig(en,status) M8266WIFI_SPI_StartModuleSmartConfig(en,1,status)


//				WPSһ������
//
/***********************************************************************************
 * M8266WIFI_SPI_StartWpsConfig                                                    *
 * @brief                                                                   *
 *     To start/stop wps config of M8266WIFI via SPI                               *
 *     After perform a start operation, the M8266WIFI will be set in STA mode      *
 * @param en      : to start or stop                                               *
 *                  =0, to stop                                                    *
 *                  others, to start                                               *
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣   *
 *                  �������Ҫ����״̬�룬����ʹ�� NULL��                       *
 * @return u8 (1=success, 0=has errors)
 ***********************************************************************************/
u8 M8266WIFI_SPI_StartWpsConfig(u8 en, u16* status);












//====================================================================================
//					Web����(STA+APģʽ��)
//====================================================================================
//   6. APIs related to on-module web operations via SPI                         //
//       - M8266WIFI_SPI_Set_WebServer                                   4.6.1    //
//----------------------------------------------------------------------------------

/***********************************************************************************
 * M8266WIFI_SPI_Set_WebServer                                                     *
 * @brief ����web��������֧�ֵ�Ƭ����������Ƕ WEB ���п������رջ����ö˿ںš�(To set the web server via SPI, with option save or not)
 * @param open_not_shutdown  : to start or shutdown the local web server           *
 *            =0,      to shutdown the webserver if it is running                  *
 *            =others, to (re-)start the webserver                                 *
 * @param server_port : the port used for web_server                               *
 *            - Only valid when open_not_shutdown!=0.                              *
 *            - When open_not_shutdown !=0, and server_port=0, then default        *
 *              port = 80 will be used                                             *
 * @param saved: to save the setting or not.                                       *
 *            - saved == 1, the setting will be saved and valid on module bootup   *
 *              e.g. if you start a web server with saved=1, upon next bootup      *
 *              the web server on module will startup automatically                *
 *              e.g. if you stop a web server with saved1=1, upon next bootup       *
 *              the web server on the module will not start-up                     *
 *            - saved == 0, the setting will not be saved and valid only currently *
 *              And after reboot, the settings will restore to previous default    *
 *            - when called with saved=1, the API will write the FL:ASH on module. *
 *                  PLEASE DO NOT CALL IT EACH TIME OF BOOTUP WITH SAVED != 0      *
 *                  OR, THE FLASH ON MODULE MIGHT GO TO FAILURE DUE TO LIFT CYCLE  *
 *                  OF WRITE                                                       *
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣   *
 *                  �������Ҫ����״̬�룬����ʹ�� NULL��                       *
 * @return u8 (1=success, 0=has errors)
 ***********************************************************************************/
u8 M8266WIFI_SPI_Set_WebServer(u8 open_not_shutdown, u16 server_port, u8 saved, u16* status);

//u8 M8266WIFI_SPI_Set_Web_Server(u8 open_not_shutdown, u16 server_port, u16* status);
//below macro used to keep compatible with previous API
#define SPI_Set_Web_Server(open_not_shutdown, server_port, status)  M8266WIFI_SPI_Set_WebServer(open_not_shutdown, server_port, 0, status)

/***********************************************************************************
 * M8266WIFI_SPI_QueryDns                                             *
 * @brief                                                                   *
 *     To query the dns server info via SPI                                        *
 * @param start_on_boot   : pointer to whether start on boot                       *
 *        use NULL if you don't expect it                                          *
 * @param current_running : pointer to whether dns is now running                  *
 *        use NULL if you don't expect it                                          *
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣   *
 *                  �������Ҫ����״̬�룬����ʹ�� NULL��                       *
 * @return value:                                                                  *
 *     =1, success                                                                 *
 *     =0, has error(s)                                                            *
 ***********************************************************************************/
// ��ѯDNS��������Ϣ
u8 M8266WIFI_SPI_QueryDns(u8* start_on_bootup, u8* current_running, u16* status);
 
/***********************************************************************************
 * M8266WIFI_SPI_StartDns                                                          *
 * @brief                                                                   *
 *     To start or shutdown the local DNS on module                                *
 * @param start_not_shutdown  : to start or shutdown the local dns server          *
 *            =0,      to shutdown the webserver if it is running                  *
 *            =others, to (re-)start the webserver                                 *
 * @param saved: to save the setting or not.                                       *
 *            - saved == 1, the setting will be saved and valid on module bootup   *
 *              e.g. if you start a dns server with saved=1, upon next bootup      *
 *              the dns server on module will startup automatically                *
 *              e.g. if you stop a dns server with saved1=1, upon next bootup       *
 *              the dns server on the module will not start-up                     *
 *            - saved == 0, the setting will not be saved and valid only currently *
 *              And after reboot, the settings will restore to previous default    *
 *            - when called with saved=1, the API will write the FL:ASH on module. *
 *                  PLEASE DO NOT CALL IT EACH TIME OF BOOTUP WITH SAVED != 0      *
 *                  OR, THE FLASH ON MODULE MIGHT GO TO FAILURE DUE TO LIFT CYCLE  *
 *                  OF WRITE                                                       *
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣   *
 *                  �������Ҫ����״̬�룬����ʹ�� NULL��                       *
 * @return u8(1=success, 0=has errors)
 ***********************************************************************************/

// ����/�ر�ģ��ı���DNS
u8 M8266WIFI_SPI_StartDns(u8 start_not_shutdown, u8 save, u16* status);












//====================================================================================
//					�͹��Ĳ���
//====================================================================================
//----------------------------------------------------------------------------------
//   7. APIs related to module low power operations via SPI                      //
//       - M8266WIFI_SPI_Set_Tx_Max_Power                                4.7.1    //
//       - M8266WIFI_SPI_Sleep_Module                                    4.7.2    //
//----------------------------------------------------------------------------------

/***********************************************************************************
 * M8266WIFI_SPI_Set_Tx_Max_Power                                                  *
 * @brief                                                                   *
 *     To set the RF Tx Max Power via SPI command                                  *
 * @param tx_max_power : the max tx power in 0.25 dBm.                             *
 *                       range = 0~82, i.e. 0~20.5dBm, or 1mW~112mW                *
 *                               tx_max_power  dBm        P/mW                     *
 *                                 0            0      @param0                     *
 *                                 4            1      @param3                     *
 *                                12            3      @param0                     *
 *                                28            7          5.0                     *
 *                                40           10         10.0                     *
 *                                52           13         20.0                     *
 *                                68           17         50.0                     *
 *                                80           20        100.0                     *
 *                                82           20.5      112.20                    *
 *        upon bootup default, tx_max_power = 82, i.e. 20.5dBm or 112.20mW         *
 *                                                                                 *
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣. �������Ҫ����״̬�룬����ʹ�� NULL��                       *
 * @return u8(1=success,0=has error(s))
 ***********************************************************************************/
 u8 M8266WIFI_SPI_Set_Tx_Max_Power(u8 tx_max_power, u16 *status);

/**********************************************************************************
 * M8266WIFI_SPI_Sleep_Module                                                      *
 * @brief ͨ��SPI�ӿ�����ģ������������ģʽ
 * 
 * @param sleep_type  ��������(�������=3, 0/1/2/���� ����)
 * @param time_to_wakeup_in_ms ���߶�ú��˳����ߣ���λ�Ǻ��롣
 * ���������Ϊ 0����ģ�齫�������ߣ�ֱ���ⲿ���ѡ������ֵ 4294967 ���� (1.19h)������������ֵ����ʹ�����ֵ��
 * @param status ִ�г���ʱ�����ص�״̬���ָ�룬���������ϡ� (�������Ҫ����״̬�룬����ʹ��NULL��)
 * 
 * @note  1.The nCS should be pulled low before exit sleep and release after bootup
 *        	from sleep if reset_type=3 in order for a normal bootup from extern flash
 *     	  2. after a call of this function, normally the module should be re init via
 *       	functions such as M8266WIFI_Module_Init_Via_SPI(), and the connection
 *        should be re-established as well.                                        *
 * @return u8 (�ɹ�=1���д���=0)
 ***********************************************************************************/
 u8 M8266WIFI_SPI_Sleep_Module(u8 sleep_type, u32 time_to_wakeup_in_ms, u16 *status);
  
/***********************************************************************************
 * M8266WIFI_SPI_Reset_Module                                                      *
 * @brief                                                                   *
 *     To reset the M8266WIFI module at once via SPI command                       *
 * @param reset_type : the type of reset                                           *
 *        = 0 :  to perform a software reset to the module,                        *
 *               boot-mode of last power on will be kept                           *
 *        = 1 :  to perform a hardware reset via chip_EN                           *
 *               boot-mode will be sampled again.                                  *
 * Note:                                                                           *
 * @param after a call of this function, normally the module should be re init     *
 *        via functions such as M8266WIFI_Module_Init_Via_SPI(), and the connection*
 *        should be re-established as well.                                        *
 * @return value:                                                                  *
 *     =1, success                                                                 *
 *     =0, has error(s)                                                            *
 ***********************************************************************************/
 u8 M8266WIFI_SPI_Reset_Module(u8 reset_type, u16* status);

/**********************************************************************************
 * M8266WIFI_SPI_Restore_Module_to_Default                                         *
 * @brief                                                                   *
 *     To restore the ALK8266WIFI module to factory default and then reboot module *
 *     via SPI command.                                                            *
 *     After calling this API, all the saved paramters will be restored to         *
 *     manufacture default and module will be in AP+STA mode                       *
 * @param status  : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣   *
 *                  �������Ҫ����״̬�룬����ʹ�� NULL��                       *
 * @return value:                                                                  *
 *     =1, success                                                                 *
 *     =0, has error(s)                                                            *
 ***********************************************************************************/
 u8 M8266WIFI_SPI_Restore_Module_to_Default(u16* status);













//====================================================================================
//					ģ���ϵ�GPIO��Դ
//====================================================================================
//----------------------------------------------------------------------------------
//   8. APIs related to module resources operations via SPI                      //
//       - M8266WIFI_SPI_Query_Module_Gpio_Mode                          4.8.1    //
//       - M8266WIFI_SPI_Config_Module_Gpio_Mode                         4.8.2    //
//       - M8266WIFI_SPI_Read_Module_Gpio                                4.8.3    //
//       - M8266WIFI_SPI_Write_Module_Gpio                               4.8.4    //
//       - M8266WIFI_SPI_Read_Module_Adc                                 4.8.5    //
//       - M8266WIFI_SPI_Module_Query_LedFlashOnBoot                     4.8.6    //
//       - M8266WIFI_SPI_Module_Config_LedFlashOnBoot                    4.8.7    //
//       - M8266WIFI_SPI_Module_Query_Wifi_Inidcator                     4.8.8    //
//       - M8266WIFI_SPI_Module_Config_Wifi_Inidcator                    4.8.9    //
//       - M8266WIFI_SPI_Module_Query_Wifi_Rxd_Interrupt_Trigger         4.8.10   //
//       - M8266WIFI_SPI_Module_Config_Wifi_Rxd_Interrupt_Trigger        4.8.11   //
//----------------------------------------------------------------------------------

typedef enum{
	 M8266WIFI_MODULE_GPIO_MODES_INPUT_NOPULL 	= 0,
	 M8266WIFI_MODULE_GPIO_MODES_INPUT_PULLUP	= 1,
	 M8266WIFI_MODULE_GPIO_MODES_OUTPUT_NORMAL	= 2,
	 M8266WIFI_MODULE_GPIO_MODES_OUTPUT_OD		= 3,
	 M8266WIFI_MODULE_GPIO_MODES_ALTERNATIIVE	= 4,
} M8266WIFI_MODULE_GPIO_MODES;	//wifiģ��GPIOģʽ
 
 /**********************************************************************************
 * M8266WIFI_SPI_Query_Module_Gpio_Mode                                            *
 * @brief                                                                   *
 *     To query the on-module gpios whether as input or output                     *
 * @param io_no        : the number of on-module GPIO                              *
 * @param io_mode      : mode to get                                               *
 *                       =0, input  without pull                                   *
 *                       =1, input  with    pullup                                 *
 *                       =2, output normal                                         *
 *                       =3, output open drain                                     *
 * @param status       : pointer to return errcode(LSB) and status(MSB) upon error *
 *                       �������Ҫ����״̬�룬����ʹ�� NULL��                  *
 * @return u8(1=success, 0=has errors)
 ***********************************************************************************/
 u8 M8266WIFI_SPI_Query_Module_Gpio_Mode(u8 io_no, M8266WIFI_MODULE_GPIO_MODES *io_mode, u16* status);

 /**********************************************************************************
 * M8266WIFI_SPI_Config_Module_Gpio_Mode                                           *
 * @brief                                                                   *
 *     To config the on-module gpios as input or output                            *
 * @param io_no        : the number of on-module GPIO                              *
 * @param io_mode      : mode to set                                               *
 *                       =0, input  without pull                                   *
 *                       =1, input  with    pullup                                 *
 *                       =2, output normal                                         *
 *                       =3, output open drain                                     *
 * @param status       : pointer to return errcode(LSB) and status(MSB) upon error *
 *                       �������Ҫ����״̬�룬����ʹ�� NULL��                  *
 * @return u8(1=success, 0=has errors)
 ***********************************************************************************/
 u8 M8266WIFI_SPI_Config_Module_Gpio_Mode(u8 io_no, M8266WIFI_MODULE_GPIO_MODES io_mode, u16* status);
 
 /**********************************************************************************
 * M8266WIFI_SPI_Read_Module_Gpio                                                  *
 * @brief                                                                   *
 *     To read the gpios level                                                     *
 * @param io_no        : the number of on-module GPIO                              *
 * @param level        : pointer of the level read back                            *
 * @param status       : pointer to return errcode(LSB) and status(MSB) upon error *
 *                       �������Ҫ����״̬�룬����ʹ�� NULL��                  *
 * @return value:                                                                  *
 *     =1, success                                                                 *
 *     =0, has error(s)                                                            *
 ***********************************************************************************/
 u8 M8266WIFI_SPI_Read_Module_Gpio(u8 io_no, u8* level, u16* status);

/**********************************************************************************
 * M8266WIFI_SPI_Write_Module_Gpio                                                 *
 * @brief                                                                   *
 *     To write the on-module gpios as output                                      *
 * @param io_no        : the number of on-module GPIO                              *
 * @param level        : pointer of the level read back                            *
 * @param status       : pointer to return errcode(LSB) and status(MSB) upon error *
 *                       �������Ҫ����״̬�룬����ʹ�� NULL��                  *
 * @return u8(1=success, 0=has errors)
 ***********************************************************************************/
 u8 M8266WIFI_SPI_Write_Module_Gpio(u8 io_no, u8 level, u16* status);

 /**********************************************************************************
 * M8266WIFI_SPI_Read_Module_Adc                                                   *
 * @brief                                                                   *
 *     To write on-module gpios as output                                          *
 * @param adc          : pointer to the adc value                                  *
 * @param status       : pointer to return errcode(LSB) and status(MSB) upon error *
 *                       �������Ҫ����״̬�룬����ʹ�� NULL��                  *
 * @return u8(1=success, 0=has errors)
 ***********************************************************************************/
 u8 M8266WIFI_SPI_Read_Module_Adc(u16* adc, u16* status);
 
/**********************************************************************************
 * M8266WIFI_SPI_Module_Query_LedsFlashOnBoot                                      *
 * @brief                                                                   *
 *     To query whether the two module Leds would flash on Boot via the SPI API    *
 * @param leds_flash_on_boot:  store the values return                             *
 *        = 0 :  LEDs not flash but off during module boot up                      *
 *        = 1 :  LEDs flash LEDs        during module boot up                      *
 * @param status  : pointer to return errcode(LSB) and status(MSB) upon error      *
 *                  Use NULL if you don't expect them returned                     *
 * @return u8(1=success, 0=has errors)
 ***********************************************************************************/
 u8 M8266WIFI_SPI_Module_Query_LedsFlashOnBoot(u8* leds_flash_on_boot, u16 *status);

/**********************************************************************************
 * M8266WIFI_SPI_Module_Config_LedsFlashOnBoot                                     *
 * @brief                                                                   *
 *     To config whether the two module Leds would flash on Boot via the SPI API   *
 * @param leds_flash_on_boot :  store the values to config                         *
 *        = 0 :  LEDs not flash but off during module boot up                      *
 *        = 1 :  LEDs flash LEDs        during module boot up                      *
 * @param status  : pointer to return errcode(LSB) and status(MSB) upon error      *
 *                  Use NULL if you don't expect them returned                     *
 * @return u8(1=success, 0=has errors)
 ***********************************************************************************/
 u8 M8266WIFI_SPI_Module_Config_LedsFlashOnBoot(u8 leds_flash_on_boot, u16 *status);
 
/**********************************************************************************
 * M8266WIFI_SPI_Module_Query_Wifi_Inidcator                                       *
 * @brief                                                                   *
 *     To query Led/pin usage for wifi indicatorvia the SPI API                    *
 * @param default_not_current:  query the default or current config                *
 *        = 0 :  query current config for wifi indicator                           *
 *        = 1 :  query default config for wifi indicator                           *
 * @param Wifi_Inidcator_en: enabled or disabled                                   *
 *        = 0 :  disabled                                                          *
 *        = 1 :  enabled, default value                                            * 
 * @param Wifi_Inidcator_io_pin: on module io pin# for Wifi Inidcator              *
 *        valid values are 0, 2, 4, and 5                                          *
 *        factory default value is 2, connecting to on-module yellow led           *
 *         = 0    :  io0 used for Wifi Inidcator                                   *
 *         = 2    :  io2 used for Wifi Inidcator                                   *
 *         = 4    :  io4 used for Wifi Inidcator                                   *
 *         = 5    :  io5 used for Wifi Inidcator                                   *
 *     4. status  : pointer to return errcode(LSB) and status(MSB) upon error      *
 *                  Use NULL if you don't expect them returned                     *
 * @return u8(1=success, 0=has errors)
 ***********************************************************************************/
 u8 M8266WIFI_SPI_Module_Query_Wifi_Inidcator(u8 default_not_current, u8* Wifi_Inidcator_en, u8* Wifi_Inidcator_io_pin, u16 *status);

/**********************************************************************************
 * M8266WIFI_SPI_Module_Config_Wifi_Inidcator                                      *
 * @brief                                                                   *
 *     To config Led/pin usage for wifi indictor led via the SPI API               *
 * @param Wifi_Inidcator_en :  enable or disable wifi indicator                    *
 *        = 0 :  wifi indicator will be disabled and light off                     *
 *        = 1 :  wifi indicator functional                                         *
 * @param Wifi_Inidcator_io_pin : io pins used for wifi indicator                  *
 *        valid values are 0, 2, 4, 5, and 0xFF                                    *
 *        factory default value is 2, connecting to on-module yellow led           *
 *         = 0    :  wifi indicator will use io0                                   *
 *         = 2    :  wifi indicator will use io2                                   *
 *         = 4    :  wifi indicator will use io4                                   *
 *         = 5    :  wifi indicator will use io5                                   *
 *         = 0xFF :  wifi indicator will use previous configured value             *
 *                   i.e. unchanged                                                *
 * @param saved   : to save the configure or not                                   *
 *         = 0,     not saved                                                      *
 *         =others, saved, i.e. after reboot, the saved setting will be loaded     *
 *                  PLEASE DO NOT CALL IT EACH TIME OF BOOTUP WITH SAVED != 0      *
 *                  OR, THE FLASH ON MODULE MIGHT GO TO FAILURE DUE TO LIFT CYCLE  *
 *                  OF WRITE                                                       *
 * @param status  : pointer to return errcode(LSB) and status(MSB) upon error      *
 *                  Use NULL if you don't expect them returned                     *
 * @return u8(1=success, 0=has errors)
 ***********************************************************************************/
 u8 M8266WIFI_SPI_Module_Config_Wifi_Inidcator(u8 Wifi_Inidcator_en, u8 Wifi_Inidcator_io_pin, u8 saved, u16 *status);

/**********************************************************************************
 * M8266WIFI_SPI_Module_Query_Wifi_Rx_Interrupt_Trigger                            *
 * @brief                                                                   *
 *     To query Led/pin usage for wifi rxd interrupt trigger via the SPI API       *
 * @param default_not_current:  query the default or current config                *
 *        = 0 :  query current config for wifi indicator                           *
 *        = 1 :  query default config for wifi indicator                           *
 * @param Wifi_Rx_Interrupt_Trigger_en: enabled or disabled                        *
 *        = 0 :  disabled                                                          *
 *        = 1 :  enabled, default value                                            *
 * @param Wifi_Rx_Interrupt_Trigger_io_pin: on module io pin# for Interrupt_Trigger*
 *        valid values are 0, 2, 4, and 5                                          *
 *        factory default value is 0, connecting to on-module green led            *
 *         = 0    :  io0 used for Wifi Rx Interrupt Trigger                        *
 *         = 2    :  io2 used for Wifi Rx Interrupt Trigger                        *
 *         = 4    :  io4 used for Wifi Rx Interrupt Trigger                        *
 *         = 5    :  io5 used for Wifi Rx Interrupt Trigger                        *
 * @param Wifi_Rx_Continous_trigger_delay_us: interval of continuos trigger         *
 *         default 50us                                                            *
 * @param status  : pointer to return errcode(LSB) and status(MSB) upon error      *
 *                  Use NULL if you don't expect them returned                     *
 * @return u8(1=success, 0=has errors)
 ***********************************************************************************/
 u8 M8266WIFI_SPI_Module_Query_Wifi_Rxd_Interrupt_Trigger(u8 default_not_current, u8* Wifi_Rx_Interrupt_Trigger_en, u8* Wifi_Rx_Interrupt_Trigger_io_pin, u8* Wifi_Rx_Continous_trigger_delay_us, u16 *status);

/**********************************************************************************
 * M8266WIFI_SPI_Module_Config_Wifi_Rx_Interrupt_Trigger                           
 * @brief                                                                   
 *     To config Led/pin usage for wifi rxd interrupt trigger via the SPI API      
 * @param Wifi_Inidcator_en 	enable or disable wifi indicator                   
 *        = 0 :  wifi indicator will be disabled and light off                     
 *        = 1 :  wifi indicator functional                                         
 * @param Wifi_Inidcator_io_pin : io pins used for wifi indicator                  
 *        valid values are 0, 2, 4, 5, and 0xFF                                    
 *        factory default value is 2, connecting to on-module yellow led           
 *         = 0    :  wifi indicator will use io0                                   
 *         = 2    :  wifi indicator will use io2                                   
 *         = 4    :  wifi indicator will use io4                                   
 *         = 5    :  wifi indicator will use io5                                   
 *         = 0xFF :  wifi indicator will use previous configured value             
 *                   i.e. unchanged                                                
 * @param Wifi_Rx_Continous_trigger_delay_us �����������
 *         = 50us  if Continous_trigger_delay_us<5                                 
 *         = 50us  if Continous_trigger_delay_us>250 and !=0xFF                    
 *         = Continous_trigger_delay_us if >=5 and <=250                           
 *         = unchanged if =0xFF                                                    
 * @param saved   �Ƿ񱣴�����
 *         = 0,     not saved                                                      
 *         =others, saved, i.e. after reboot, the saved setting will be loaded     
 *                  PLEASE DO NOT CALL IT EACH TIME OF BOOTUP WITH SAVED != 0      
 *                  OR, THE FLASH ON MODULE MIGHT GO TO FAILURE DUE TO LIFT CYCLE  
 *                  OF WRITE                                                       
 * @param status  ����ʱ���ش����루LSB����״̬��MSB����ָ��    
 *                Use NULL if you don't expect them returned                   
 * @return u8(1=success, 0=has errors)                                                         
 ***********************************************************************************/
 u8 M8266WIFI_SPI_Module_Config_Wifi_Rx_Interrupt_Trigger(u8 Wifi_Rx_Interrupt_Trigger_en, u8 Wifi_Rx_Interrupt_Trigger_io_pin, u8 Wifi_Rx_Continous_trigger_delay_us, u8 saved, u16 *status);

#define M8266WIFI_SPI_Module_Config_Leds_Off(onoff, saved, status)  									\
		{																								\
			M8266WIFI_SPI_Module_Config_LedsFlashOnBoot(onoff, status);									\
			M8266WIFI_SPI_Module_Config_Wifi_Inidcator(onoff,  0xFF, saved, status);					\
			M8266WIFI_SPI_Module_Config_Wifi_Rx_Interrupt_Trigger(onoff, 0xFF, 0xFF, saved, status);	\
		}

 #define M8266WIFI_SPI_Module_Swap_Leds_pins(saved, status)  											\
		{																								\
			M8266WIFI_SPI_Module_Config_Wifi_Inidcator(1,  0, saved, status);							\
			M8266WIFI_SPI_Module_Config_Wifi_Rx_Interrupt_Trigger(1, 2, 50, saved, status);				\
		}
//








//====================================================================================
//					ģ����Ϣ��ѯ
//====================================================================================
//----------------------------------------------------------------------------------
//   9. APIs related to module information					                      //
//       - M8266WIFI_SPI_Get_Module_Info                                 4.9.1    //
//       - M8266WIFI_SPI_Get_Driver_Info                                 4.9.2    //
//----------------------------------------------------------------------------------

/***********************************************************************************
 * M8266WIFI_SPI_Get_Module_Info                                                   *
 * @brief                                                                   *
 *     ��ȡM8266WIFIģ����Ϣ, �� module_id, Flash Size, Firmware Version           *

 * @param module_id: the pointer to the returned module_id                         *
 *                    �������Ҫ����״̬�룬����ʹ�� NULL��                     *
 * @param flash_size: the pointer to the returned flash_size                       *
 *                    returned value                                               *
 *                      = 0, if 512KB                                              *
 *                      = 1, if 1MB                                                *
 *                      = 2, if 2MB                                                *
 *                      = 3, if 4MB                                                *
 *                      = others, invalid                                          *
 *                    �������Ҫ����״̬�룬����ʹ�� NULL��                     *
 * @param fw_ver    : the pointer to the returned firmware version                 *
 *                    buffer size should be no less than 16 bytes                  *
 *                    returned value e.g. "1.1.4-4"                                *
 *                    �������Ҫ����״̬�룬����ʹ�� NULL��                     *
 * @param status    : ʧ��ʱ���ش����루LSB����״̬��MSB����ָ�롣 *
 *                    �������Ҫ����״̬�룬����ʹ�� NULL��                     *
 * @return u8(1=success, 0=has errors)
 ***********************************************************************************/
 u8 M8266WIFI_SPI_Get_Module_Info(u32* module_id, u8* flash_size, char* fw_ver, u16* status);

/***********************************************************************************
 * M8266WIFI_SPI_Get_Driver_Info                                                   *
 * @brief  ��ȡM8266WIFI������Ϣ				                                       *
 * @param drv_info: the pointer to the returned driver information buffer.         *
 *                  buffer size should be no less than 64 Bytes                    *
 *                  e.g. "ANYLINKIN M8266WIFI SPI DRIVER V1.4, 20170316"           *
 * @return ������Ϣ(driver_info)
 ***********************************************************************************/
char* M8266WIFI_SPI_Get_Driver_Info(char* drv_info);








//====================================================================================
//					������ͨ�� SPI �ӿڵ����á���ѯ���Ϳ��ƹ���
//====================================================================================
//----------------------------------------------------------------------------------
//   Below specifications are required by the M8266WIFI Driver library            //
//   And should be implemented according to the hardware in M8266HostIf.c         //
//   Put here kust for purpose ofheader specification                             //
//----------------------------------------------------------------------------------

void M8266HostIf_Set_nRESET_Pin(u8 level);	// M8266HostIf_STM32xx.c
void M8266HostIf_Set_SPI_nCS_Pin(u8 level);	// M8266HostIf_STM32xx.c
void M8266HostIf_delay_us(u8 nus);			// M8266HostIf_STM32xx.c
u8   M8266HostIf_SPI_ReadWriteByte(u8 byte); // Not required for most platforms.	// M8266HostIf_STM32xx.c

#endif
