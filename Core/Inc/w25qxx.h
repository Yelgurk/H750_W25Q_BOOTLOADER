/*
 * w25qxx.h
 *
 *  Created on: 4 янв. 2021 г.
 *      Author: Xell
 */

#ifndef INC_W25QXX_H_
#define INC_W25QXX_H_

#include "main.h"

#define MEMORY_FLASH_SIZE               0x800000 /* 64 MBits = 8 MByte */
#define MEMORY_SECTOR_SIZE              0x10000  /* 64kBytes */
#define MEMORY_PAGE_SIZE                0x100   /* 256 bytes */

/*----------------------------------------------------------------------------*/
//W25XÏµÁÐ/QÏµÁÐÐ¾Æ¬ÁÐ±í
#define W25Q80 	0xEF13
#define W25Q16 	0xEF14
#define W25Q32 	0xEF15
#define W25Q64 	0xEF16
#define W25Q128	0xEF17
#define W25Q256 0xEF18

//±ê×¼ SPI Ö¸Áî
#define W25X_WriteEnable           0x06
#define W25X_VolatileSRWriteEnable 0x50
#define W25X_WriteDisable          0x04

#define W25X_ReleasePowerDownID	   0xAB
#define W25X_ManufacturerDeviceID  0x90
#define W25X_JedecID               0x9F
#define W25X_ReadUniqueID          0x4B //SPI
#define W25X_SetReadParameters     0xC0 //QPI

#define W25X_ReadData			   0x03 //SPI
#define W25X_FastRead              0x0B
#define W25X_BurstReadWithWrap     0x0C //QPI
#define W25X_FastReadQuadIO        0xEB //QPI

#define W25X_PageProgram		   0x02
#define W25X_SectorErase		   0x20 //4KB
#define W25X_BlockErase32          0x52 //32KB
#define W25X_BlockErase64		   0xD8 //64KB
#define W25X_ChipErase			   0xC7 //0x60

#define W25X_ReadStatusReg1		   0x05
#define W25X_ReadStatusReg2		   0x35
#define W25X_ReadStatusReg3		   0x15
#define W25X_WriteStatusReg1       0x01 // Reg1 Reg2
#define W25X_WriteStatusReg2       0x31
#define W25X_WriteStatusReg3       0x11

#define W25X_ReadSFDPReg           0x5A //SPI
#define W25X_EraseSecurityReg      0x44 //SPI
#define W25X_ProgSecurityReg       0x42 //SPI
#define W25X_ReadSecurityReg       0x48 //SPI

#define W25X_GlobalBlockLock       0x7E
#define W25X_GlobalBlockUnlock     0x98
#define W25X_ReadBlockLock         0x3D
#define W25X_IndividualBlockLock   0x36
#define W25X_IndividualBlockUnlock 0x39

#define W25X_EraseProgramSuspend   0x75
#define W25X_EraseProgramResume    0x7A

#define W25X_PowerDown			   0xB9

#define W25X_EnterQPIMode          0x38 //SPI
#define W25X_EnableReset           0x66
#define W25X_ResetDevice           0x99
#define W25X_ExitQPIMode           0xFF //QPI

/*----------------------------------------------------------------------------*/
extern uint16_t w25qxx_mid; //¶¨ÒåW25QXXÐ¾Æ¬ÐÍºÅ
/*----------------------------------------------------------------------------*/
void W25QXX_Init(void); //³õÊ¼»¯W25QXX
void W25QXX_ExitQPIMode(void); //ÍË³öQPIÄ£Ê½
void W25QXX_EnterQPIMode(void); //½øÈëQPIÄ£Ê½
uint8_t W25QXX_ReadSR(uint8_t srx); //¶ÁÈ¡×´Ì¬¼Ä´æÆ÷
void W25QXX_WriteSR(uint8_t srx, uint8_t dat); //Ð´×´Ì¬¼Ä´æÆ÷
void W25QXX_WriteEnable(uint8_t en); //Ð´Ê¹ÄÜ»ò½ûÖ¹

uint16_t W25QXX_MftrDeviceID(void); //¶ÁÈ¡³§ÉÌºÍÉè±¸ID
uint8_t W25QXX_UniqueID(void); //¶ÁÈ¡Î¨Ò»ID

void W25QXX_Write_NoCheck(uint8_t* pbuf, uint32_t addr, uint16_t size);
void W25QXX_Read(uint8_t *pbuf, uint32_t addr, uint16_t size); //¿ìËÙ¶ÁÈ¡
void W25QXX_PageProgram(uint8_t* pbuf, uint32_t addr, uint16_t size);
void W25QXX_Write(uint8_t *pbuf, uint32_t addr, uint16_t size);
void W25QXX_ChipErase(void); //ÕûÆ¬²Á³ý
void W25QXX_SectorErase(uint32_t addr); //ÉÈÇø²Á³ý
void W25QXX_WaitBusy(void); //µÈ´ý¿ÕÏÐ
void W25QXX_Reset(void); //¸´Î»

#endif /* INC_W25QXX_H_ */
