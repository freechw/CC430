#ifndef __W25X16_H
#define __W25X16_H

#include "cc430x513x.h"

#define CS_1  P1OUT |=  BIT7
#define CS_0  P1OUT &=~ BIT7
/* Private typedef -----------------------------------------------------------*/

//#define SPI_FLASH_PageSize      4096
#define SPI_FLASH_PageSize      256
#define SPI_FLASH_PerWritePageSize      256

/* Private define ------------------------------------------------------------*/

#define Write_Enable   0x06   //��ÿһ��д���߲�������ǰ����ִ��һ�� 
#define Write_Disable  0x04   //�ر�дʹ��
#define Read_Data      0x03   //������
#define Read_StatusReg  0x05 
#define Write_StatusReg 0x01 
#define Register_1     0x05   //��״̬�Ĵ��� 1
#define Page_Write     0x02   //дһ��ҳ������  256 byte
#define Sector_Erase   0x20   //�β���  4KB �������Ҫ400ms����ʱ��
#define Chip_Erase     0xC7   //Ƭ���� �������Ҫ40s
#define Power_Down     0xB9   //����ģʽ���͹��ģ�����ֵ 1.5 uA��,��ʱ3us����Ч
#define Power_Up       0xAB   //�ӵ���ģʽ�лָ����ɲ���ģʽ
#define ManufactDeviceID 0x90 //������ID
#define JedecDeviceID  0x9F 
#define FastReadData 0x0B 
#define FastReadDual 0x3B 
#define BlockErase  0xD8 
#define DeviceID    0xAB 

#define WIP_Flag                  0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte                0xFF

void SPI_FLASH_Init(void);
void SPI_Send_Data( unsigned char data);      //SPI����һ���ֽ�����
unsigned char SPI_Read_Data( void);           //SPI��ȡһ���ֽ�����
void ExtenalFlash_Write_Enable(void );  // дʹ��
unsigned char ExtenalFlash_Read_Register(void );  // ��ʹ��
void ExtenalFlash_Power_Down(void ) ; // ������ģʽ
void ExtenalFlash_Power_Up(void ) ; // �˳�����ģʽ
void ExtenalFlash_Sector_Erase( long addr);  //flash ��������
void ExtenalFlash_Write(unsigned char *p, int count ,long addr);  //flash д����
void ExtenalFlash_Read(unsigned char *p, long Adress , int count) ; //flash ������
long ExtenalFlash_ReadID(void);  //��������ID
unsigned char SPI_FLASH_ReadDeviceID(void);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_WaitForWriteEnd(void);


#endif /* __W25X16_H */

