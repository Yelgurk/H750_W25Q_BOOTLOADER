/*
 * w25qxx.c
 *
 *  Created on: 4 янв. 2021 г.
 *      Author: Xell
 */


#include "w25qxx.h"
#include "delay.h"
/*----------------------------------------------------------------------------*/
//4KBytesÎªÒ»¸öSector,16¸öSectorÎª1¸öBlock
//¹²ÓÐ128¸öBlock,2048¸öSector,ÈÝÁ¿Îª8M×Ö½Ú
/*----------------------------------------------------------------------------*/
#define  CONSOLE_LOG 0
/*----------------------------------------------------------------------------*/
#define  W25QXX_MODE_SPI 0
#define  W25QXX_MODE_QPI 1

#define  QE_MASK   0x02
#define  BUSY_MASK 0x01
/*----------------------------------------------------------------------------*/
uint8_t  w25qxx_mode = W25QXX_MODE_SPI;
uint8_t  w25qxx_uid[8];
uint8_t  w25qxx_buf[4096];
uint16_t w25qxx_mid = W25Q64;

extern QSPI_HandleTypeDef hqspi;

/*----------------------------------------------------------------------------*/
//³õÊ¼»¯
void W25QXX_Init(void)
{
	W25QXX_ExitQPIMode();
	W25QXX_Reset();
	W25QXX_EnterQPIMode();
}
//ÍË³öQPIÄ£Ê½
void W25QXX_ExitQPIMode(void)
{
	QSPI_CommandTypeDef cmd;

	cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
	cmd.Instruction = W25X_ExitQPIMode;

	cmd.AddressMode = QSPI_ADDRESS_NONE;
	cmd.AddressSize = QSPI_ADDRESS_24_BITS;
	cmd.Address = 0x00;

	cmd.DataMode = QSPI_DATA_NONE;
	cmd.NbData = 0;

	cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	cmd.AlternateBytesSize = 0;
	cmd.AlternateBytes = 0x00;

	cmd.DummyCycles = 0;

	cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	HAL_QSPI_Command(&hqspi, &cmd, 100);

	w25qxx_mode = W25QXX_MODE_SPI;
}

//½øÈëQPIÄ£Ê½
void W25QXX_EnterQPIMode(void)
{
	uint8_t dat;

	QSPI_CommandTypeDef cmd;

	dat = W25QXX_ReadSR(2); //ÏÈ¶Á³ö×´Ì¬¼Ä´æÆ÷2µÄÔ­Ê¼Öµ
	if ((dat & QE_MASK) == 0x00) //QEÎ»Î´Ê¹ÄÜ
	{
		W25QXX_WriteEnable(1); //Ð´Ê¹ÄÜ
		dat |= QE_MASK; //Ê¹ÄÜQEÎ»
		W25QXX_WriteSR(2, dat); //Ð´×´Ì¬¼Ä´æÆ÷2
	}

	cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	cmd.Instruction = W25X_EnterQPIMode;

	cmd.AddressMode = QSPI_ADDRESS_NONE;
	cmd.AddressSize = QSPI_ADDRESS_24_BITS;
	cmd.Address = 0x00;

	cmd.DataMode = QSPI_DATA_NONE;
	cmd.NbData = 0;

	cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	cmd.AlternateBytesSize = 0;
	cmd.AlternateBytes = 0x00;

	cmd.DummyCycles = 0;

	cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	HAL_QSPI_Command(&hqspi, &cmd, 100);

	w25qxx_mode = W25QXX_MODE_QPI;

	cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
	cmd.Instruction = W25X_SetReadParameters;
	cmd.DataMode = QSPI_DATA_4_LINES;
	cmd.NbData = 1;
	dat = 0x03 << 4; //ÉèÖÃP4&P5=11,8¸ödummy clocks,104MHz

	W25QXX_WriteEnable(1);
	if (HAL_QSPI_Command(&hqspi, &cmd, 100) == HAL_OK)
	{
		HAL_QSPI_Transmit(&hqspi, &dat, 100);
	}
}

//¶Á×´Ì¬¼Ä´æÆ÷
//srx:¼Ä´æÆ÷ºÅ
//·µ»ØÖµ:¼Ä´æÆ÷Öµ
uint8_t W25QXX_ReadSR(uint8_t srx)
{
	uint8_t dat = 0;

	QSPI_CommandTypeDef cmd;

	if (w25qxx_mode)
	{
		cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
		cmd.DataMode = QSPI_DATA_4_LINES;
	}
	else
	{
		cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
		cmd.DataMode = QSPI_DATA_1_LINE;
	}

	cmd.AddressMode = QSPI_ADDRESS_NONE;
	cmd.AddressSize = QSPI_ADDRESS_24_BITS;
	cmd.Address = 0x00;

	cmd.NbData = 1;

	cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	cmd.AlternateBytesSize = 0;
	cmd.AlternateBytes = 0x00;

	cmd.DummyCycles = 0;

	cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    switch(srx)
    {
        case 1:
            cmd.Instruction = W25X_ReadStatusReg1;
            break;
        case 2:
            cmd.Instruction = W25X_ReadStatusReg2;
			break;
		case 3:
            cmd.Instruction = W25X_ReadStatusReg3;
            break;
        default:
            cmd.Instruction = W25X_ReadStatusReg1;
            break;
    }
	if (HAL_QSPI_Command(&hqspi, &cmd, 100) == HAL_OK)
	{
		HAL_QSPI_Receive(&hqspi, &dat, 100);
	}

	return dat;
}

//Ð´×´Ì¬¼Ä´æÆ÷
void W25QXX_WriteSR(uint8_t srx, uint8_t dat)
{
	QSPI_CommandTypeDef cmd;

	if (w25qxx_mode)
	{
		cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
		cmd.DataMode = QSPI_DATA_4_LINES;
	}
	else
	{
		cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
		cmd.DataMode = QSPI_DATA_1_LINE;
	}

	cmd.AddressMode = QSPI_ADDRESS_NONE;
	cmd.AddressSize = QSPI_ADDRESS_24_BITS;
	cmd.Address = 0x00;

	cmd.NbData = 1;

	cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	cmd.AlternateBytesSize = 0;
	cmd.AlternateBytes = 0x00;

	cmd.DummyCycles = 0;

	cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    switch(srx)
    {
        case 1:
            cmd.Instruction = W25X_WriteStatusReg1;
            break;
        case 2:
            cmd.Instruction = W25X_WriteStatusReg2;
			break;
		case 3:
            cmd.Instruction = W25X_WriteStatusReg3;
            break;
        default:
            cmd.Instruction = W25X_WriteStatusReg1;
            break;
    }
	if (HAL_QSPI_Command(&hqspi, &cmd, 100) == HAL_OK)
	{
		HAL_QSPI_Transmit(&hqspi, &dat, 100);
	}
}

//Ð´Ê¹ÄÜ»ò½ûÖ¹£¬½«S1¼Ä´æÆ÷µÄWELÖÃÎ»/ÇåÁã
void W25QXX_WriteEnable(uint8_t en)
{
	QSPI_CommandTypeDef cmd;

	if (w25qxx_mode)
	{
		cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
	}
	else
	{
		cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	}

	if (en)
	{
		cmd.Instruction = W25X_WriteEnable;
	}
	else
	{
		cmd.Instruction = W25X_WriteDisable;
	}

	cmd.AddressMode = QSPI_ADDRESS_NONE;
	cmd.AddressSize = QSPI_ADDRESS_24_BITS;
	cmd.Address = 0x00;

	cmd.DataMode = QSPI_DATA_NONE;
	cmd.NbData = 0;

	cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	cmd.AlternateBytesSize = 0;
	cmd.AlternateBytes = 0x00;

	cmd.DummyCycles = 0;

	cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	HAL_QSPI_Command(&hqspi, &cmd, 100);
}

//»ñÈ¡³§ÉÌºÍÉè±¸ID
uint16_t W25QXX_MftrDeviceID(void)
{
	uint8_t pData[2];
	uint16_t MftrID = 0xEEEE;

	QSPI_CommandTypeDef cmd;

	if (w25qxx_mode == W25QXX_MODE_QPI)
	{
		cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
		cmd.AddressMode = QSPI_ADDRESS_4_LINES;
		cmd.DataMode = QSPI_DATA_4_LINES;
	}
	else
	{
		cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode = QSPI_ADDRESS_1_LINE;
		cmd.DataMode = QSPI_DATA_1_LINE;
	}

	cmd.Instruction = W25X_ManufacturerDeviceID;

	cmd.AddressSize = QSPI_ADDRESS_24_BITS;
	cmd.Address = 0x00;

	cmd.NbData = 2;

	cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	cmd.AlternateBytesSize = 0;
	cmd.AlternateBytes = 0x00;

	cmd.DummyCycles = 0;

	cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(&hqspi, &cmd, 100) != HAL_OK)
	{
		return MftrID;
	}
	if (HAL_QSPI_Receive(&hqspi, pData, 100) != HAL_OK)
	{
		return MftrID;
	}
	MftrID = (pData[0] << 8) | pData[1];
#if CONSOLE_LOG
	printf("MID: %04X\r\n", MftrID);
#endif
	return MftrID;
}

//Î¨Ò»ID£¬½öSPIÄ£Ê½
uint8_t W25QXX_UniqueID(void)
{
	QSPI_CommandTypeDef cmd;

	if (w25qxx_mode)
	{
		return 1;
	}

	cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	cmd.Instruction = W25X_ReadUniqueID;

	cmd.AddressMode = QSPI_ADDRESS_NONE;
	cmd.AddressSize = QSPI_ADDRESS_24_BITS;
	cmd.Address = 0x00;

	cmd.DataMode = QSPI_DATA_1_LINE;
	cmd.NbData = 8;

	cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	cmd.AlternateBytesSize = 0;
	cmd.AlternateBytes = 0x00;

	cmd.DummyCycles = 32 - 1;

	cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(&hqspi, &cmd, 100) != HAL_OK)
	{
		return 1;
	}
	if (HAL_QSPI_Receive(&hqspi, w25qxx_uid, 100) != HAL_OK)
	{
		return 1;
	}
#if CONSOLE_LOG
	uint32_t id;
	id = (w25qxx_uid[0] << 24) | (w25qxx_uid[1] << 16) | (w25qxx_uid[2] << 8) | w25qxx_uid[3];
	printf("UID: %08X", id);
	id = (w25qxx_uid[4] << 24) | (w25qxx_uid[5] << 16) | (w25qxx_uid[6] << 8) | w25qxx_uid[7];
	printf("%08X\r\n", id);
#endif
	return 0;
}

//¿ìËÙ¶ÁÈ¡
void W25QXX_Read(uint8_t *pbuf, uint32_t addr, uint16_t size)
{
	QSPI_CommandTypeDef cmd;

	if (w25qxx_mode)
	{
		cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
		cmd.AddressMode = QSPI_ADDRESS_4_LINES;
		cmd.DataMode = QSPI_DATA_4_LINES;
	}
	else
	{
		cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode = QSPI_ADDRESS_1_LINE;
		cmd.DataMode = QSPI_DATA_1_LINE;
	}
	cmd.Instruction = W25X_FastRead;

	cmd.AddressSize = QSPI_ADDRESS_24_BITS;
	cmd.Address = addr;

	cmd.NbData = size;

	cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	cmd.AlternateBytesSize = 0;
	cmd.AlternateBytes = 0x00;

	cmd.DummyCycles = 8;

	cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(&hqspi, &cmd, 100) != HAL_OK)
	{
		return;
	}
	HAL_QSPI_Receive(&hqspi, pbuf, 1000);
}
//0.4~3ms
//±à³ÌÄ³Ò³
void W25QXX_PageProgram(uint8_t* pbuf, uint32_t addr, uint16_t size)
{
	QSPI_CommandTypeDef cmd;

	if (size > 256) return;

	if (w25qxx_mode)
	{
		cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
		cmd.AddressMode = QSPI_ADDRESS_4_LINES;
		cmd.DataMode = QSPI_DATA_4_LINES;
	}
	else
	{
		cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode = QSPI_ADDRESS_1_LINE;
		cmd.DataMode = QSPI_DATA_1_LINE;
	}
	cmd.Instruction = W25X_PageProgram;

	cmd.AddressSize = QSPI_ADDRESS_24_BITS;
	cmd.Address = addr;

	cmd.NbData = size;

	cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	cmd.AlternateBytesSize = 0;
	cmd.AlternateBytes = 0x00;

	cmd.DummyCycles = 0;

	cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	W25QXX_WriteEnable(1); //Ð´Ê¹ÄÜ
	if (HAL_QSPI_Command(&hqspi, &cmd, 100) != HAL_OK)
	{
		return;
	}
	if (HAL_QSPI_Transmit(&hqspi, pbuf, 100) != HAL_OK)
	{
		return;
	}
	W25QXX_WaitBusy();
}

//ÎÞ¼ìÑéÐ´SPI FLASH
void W25QXX_Write_NoCheck(uint8_t* pbuf, uint32_t addr, uint16_t size)
{
	uint32_t page_rem;
	page_rem = 256 - addr % 256; //µ¥Ò³Ê£ÓàµÄ×Ö½ÚÊý
	if (size <= page_rem)
	{
		page_rem = size; //²»´óÓÚ256¸ö×Ö½Ú
	}
	while(1)
	{
		W25QXX_PageProgram(pbuf, addr, page_rem);
		if (size == page_rem)
		{
			break; //Ð´Èë½áÊøÁË
		}
	 	else //size > page_rem
		{
			pbuf += page_rem;
			addr += page_rem;

			size -= page_rem; //¼õÈ¥ÒÑ¾­Ð´ÈëÁËµÄ×Ö½ÚÊý
			if (size > 256)
			{
				page_rem = 256; //Ò»´Î¿ÉÒÔÐ´Èë256¸ö×Ö½Ú
			}
			else
			{
				page_rem = size; //²»¹»256¸ö×Ö½ÚÁË
			}
		}
	}
}

//Ð´SPI FLASH
void W25QXX_Write(uint8_t *pbuf, uint32_t addr, uint16_t size)
{
	uint32_t sec_pos;
	uint32_t sec_off;
	uint32_t sec_rem;
 	uint32_t i;
	uint8_t *W25QXX_BUF;

	W25QXX_BUF = w25qxx_buf;
 	sec_pos = addr / 4096; //ÉÈÇøµØÖ·
	sec_off = addr % 4096; //ÔÚÉÈÇøÄÚµÄÆ«ÒÆ
	sec_rem = 4096 - sec_off; //ÉÈÇøÊ£Óà¿Õ¼ä´óÐ¡
	#if CONSOLE_LOG
 	printf("addr:%08X size:%hu\r\n", addr, size); //²âÊÔÓÃ
	#endif
 	if(size <= sec_rem)
	{
		sec_rem = size; //²»´óÓÚ4096¸ö×Ö½Ú
	}
	while(1)
	{
		W25QXX_Read(W25QXX_BUF, sec_pos * 4096, 4096); //¶Á³öÕû¸öÉÈÇøµÄÄÚÈÝ
		for (i = 0; i < sec_rem; i++) //Ð£ÑéÊý¾Ý
		{
			if (W25QXX_BUF[sec_off + i] != 0xFF)
				break; //ÐèÒª²Á³ý
		}
		if (i < sec_rem) //ÐèÒª²Á³ý
		{
			W25QXX_SectorErase(sec_pos); //²Á³ýÕâ¸öÉÈÇø
			for (i = 0; i < sec_rem; i++) //¸´ÖÆ
			{
				W25QXX_BUF[sec_off + i] = pbuf[i];
			}
			W25QXX_Write_NoCheck(W25QXX_BUF, sec_pos * 4096, 4096); //Ð´ÈëÕû¸öÉÈÇø
		}
		else
		{
			W25QXX_Write_NoCheck(pbuf, addr, sec_rem); //Ð´ÒÑ¾­²Á³ýÁËµÄ,Ö±½ÓÐ´ÈëÉÈÇøÊ£ÓàÇø¼ä.
		}
		if (size == sec_rem)
		{
			break; //Ð´Èë½áÊøÁË
		}
		else //Ð´ÈëÎ´½áÊø
		{
			sec_pos++; //ÉÈÇøµØÖ·Ôö1
			sec_off = 0; //Æ«ÒÆÎ»ÖÃÎª0

			pbuf += sec_rem; //Ö¸ÕëÆ«ÒÆ
			addr += sec_rem; //Ð´µØÖ·Æ«ÒÆ
			size-=sec_rem;				//×Ö½ÚÊýµÝ¼õ
			if (size > 4096)
			{
				sec_rem = 4096; //ÏÂÒ»¸öÉÈÇø»¹ÊÇÐ´²»Íê
			}
			else
			{
				sec_rem = size; //ÏÂÒ»¸öÉÈÇø¿ÉÒÔÐ´ÍêÁË
			}
		}
	}
}

//²Á³ýÕû¸öÐ¾Æ¬ 20~100s
void W25QXX_ChipErase(void)
{
	QSPI_CommandTypeDef cmd;

	if (w25qxx_mode)
	{
		cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
	}
	else
	{
		cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	}
	cmd.Instruction = W25X_ChipErase;

	cmd.AddressMode = QSPI_ADDRESS_NONE;
	cmd.AddressSize = QSPI_ADDRESS_24_BITS;
	cmd.Address = 0x00;

	cmd.DataMode = QSPI_DATA_NONE;
	cmd.NbData = 0;

	cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	cmd.AlternateBytesSize = 0;
	cmd.AlternateBytes = 0x00;

	cmd.DummyCycles = 0;

	cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	W25QXX_WriteEnable(1);
	W25QXX_WaitBusy();
	HAL_QSPI_Command(&hqspi, &cmd, 100);
	W25QXX_WaitBusy();
}

//²Á³ýÒ»¸öÉÈÇø 45~400ms
void W25QXX_SectorErase(uint32_t addr)
{
	QSPI_CommandTypeDef cmd;

	if (w25qxx_mode)
	{
		cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
		cmd.AddressMode = QSPI_ADDRESS_4_LINES;
	}
	else
	{
		cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode = QSPI_ADDRESS_1_LINE;
	}
	cmd.Instruction = W25X_SectorErase;

	cmd.AddressSize = QSPI_ADDRESS_24_BITS;
	cmd.Address = addr;

	cmd.DataMode = QSPI_DATA_NONE;
	cmd.NbData = 0;

	cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	cmd.AlternateBytesSize = 0;
	cmd.AlternateBytes = 0x00;

	cmd.DummyCycles = 0;

	cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	W25QXX_WriteEnable(1); //SET WEL
	W25QXX_WaitBusy();
	HAL_QSPI_Command(&hqspi, &cmd, 100);
	W25QXX_WaitBusy(); //µÈ´ý²Á³ýÍê³É
}

//µÈ´ý¿ÕÏÐ
void W25QXX_WaitBusy(void)
{
	while((W25QXX_ReadSR(1) & BUSY_MASK) == BUSY_MASK); //µÈ´ýBUSYÎ»Çå¿Õ
}

//¸´Î»
void W25QXX_Reset(void)
{
	QSPI_CommandTypeDef cmd;

	if (w25qxx_mode)
	{
		cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
	}
	else
	{
		cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	}
	cmd.Instruction = W25X_EnableReset;

	cmd.AddressMode = QSPI_ADDRESS_NONE;
	cmd.AddressSize = QSPI_ADDRESS_24_BITS;
	cmd.Address = 0;

	cmd.DataMode = QSPI_DATA_NONE;
	cmd.NbData = 0;

	cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	cmd.AlternateBytesSize = 0;
	cmd.AlternateBytes = 0x00;

	cmd.DummyCycles = 0;

	cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	W25QXX_WaitBusy();
	if (HAL_QSPI_Command(&hqspi, &cmd, 100) == HAL_OK)
	{
		cmd.Instruction = W25X_ResetDevice;
		HAL_QSPI_Command(&hqspi, &cmd, 100);
	}
}
