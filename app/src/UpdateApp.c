#include "app/inc/UpdateApp.h"
#include "app/inc/FlashApp.h"
#include "app/inc/RegisterApp.h"
#include "app/inc/I2C2SlaveApp.h"
#include "driver/inc/NVMDriver.h"
#include "driver/inc/UartDriver.h"

uint8_t APP_POS = 0x0AU;
uint16_t u16ChecksumMCU = 0U;
static uint8_t u8TxBuffer[100] = {0};
const uint32_t crc32_tab[256] = {
	0x00000000U, 0x77073096U, 0xee0e612cU, 0x990951baU, 0x076dc419U, 0x706af48fU,
	0xe963a535U, 0x9e6495a3U, 0x0edb8832U, 0x79dcb8a4U, 0xe0d5e91eU, 0x97d2d988U,
	0x09b64c2bU, 0x7eb17cbdU, 0xe7b82d07U, 0x90bf1d91U, 0x1db71064U, 0x6ab020f2U,
	0xf3b97148U, 0x84be41deU, 0x1adad47dU, 0x6ddde4ebU, 0xf4d4b551U, 0x83d385c7U,
	0x136c9856U, 0x646ba8c0U, 0xfd62f97aU, 0x8a65c9ecU,	0x14015c4fU, 0x63066cd9U,
	0xfa0f3d63U, 0x8d080df5U, 0x3b6e20c8U, 0x4c69105eU, 0xd56041e4U, 0xa2677172U,
	0x3c03e4d1U, 0x4b04d447U, 0xd20d85fdU, 0xa50ab56bU,	0x35b5a8faU, 0x42b2986cU,
	0xdbbbc9d6U, 0xacbcf940U, 0x32d86ce3U, 0x45df5c75U, 0xdcd60dcfU, 0xabd13d59U,
	0x26d930acU, 0x51de003aU, 0xc8d75180U, 0xbfd06116U, 0x21b4f4b5U, 0x56b3c423U,
	0xcfba9599U, 0xb8bda50fU, 0x2802b89eU, 0x5f058808U, 0xc60cd9b2U, 0xb10be924U,
	0x2f6f7c87U, 0x58684c11U, 0xc1611dabU, 0xb6662d3dU,	0x76dc4190U, 0x01db7106U,
	0x98d220bcU, 0xefd5102aU, 0x71b18589U, 0x06b6b51fU, 0x9fbfe4a5U, 0xe8b8d433U,
	0x7807c9a2U, 0x0f00f934U, 0x9609a88eU, 0xe10e9818U, 0x7f6a0dbbU, 0x086d3d2dU,
	0x91646c97U, 0xe6635c01U, 0x6b6b51f4U, 0x1c6c6162U, 0x856530d8U, 0xf262004eU,
	0x6c0695edU, 0x1b01a57bU, 0x8208f4c1U, 0xf50fc457U, 0x65b0d9c6U, 0x12b7e950U,
	0x8bbeb8eaU, 0xfcb9887cU, 0x62dd1ddfU, 0x15da2d49U, 0x8cd37cf3U, 0xfbd44c65U,
	0x4db26158U, 0x3ab551ceU, 0xa3bc0074U, 0xd4bb30e2U, 0x4adfa541U, 0x3dd895d7U,
	0xa4d1c46dU, 0xd3d6f4fbU, 0x4369e96aU, 0x346ed9fcU, 0xad678846U, 0xda60b8d0U,
	0x44042d73U, 0x33031de5U, 0xaa0a4c5fU, 0xdd0d7cc9U, 0x5005713cU, 0x270241aaU,
	0xbe0b1010U, 0xc90c2086U, 0x5768b525U, 0x206f85b3U, 0xb966d409U, 0xce61e49fU,
	0x5edef90eU, 0x29d9c998U, 0xb0d09822U, 0xc7d7a8b4U, 0x59b33d17U, 0x2eb40d81U,
	0xb7bd5c3bU, 0xc0ba6cadU, 0xedb88320U, 0x9abfb3b6U, 0x03b6e20cU, 0x74b1d29aU,
	0xead54739U, 0x9dd277afU, 0x04db2615U, 0x73dc1683U, 0xe3630b12U, 0x94643b84U,
	0x0d6d6a3eU, 0x7a6a5aa8U, 0xe40ecf0bU, 0x9309ff9dU, 0x0a00ae27U, 0x7d079eb1U,
	0xf00f9344U, 0x8708a3d2U, 0x1e01f268U, 0x6906c2feU, 0xf762575dU, 0x806567cbU,
	0x196c3671U, 0x6e6b06e7U, 0xfed41b76U, 0x89d32be0U, 0x10da7a5aU, 0x67dd4accU,
	0xf9b9df6fU, 0x8ebeeff9U, 0x17b7be43U, 0x60b08ed5U, 0xd6d6a3e8U, 0xa1d1937eU,
	0x38d8c2c4U, 0x4fdff252U, 0xd1bb67f1U, 0xa6bc5767U, 0x3fb506ddU, 0x48b2364bU,
	0xd80d2bdaU, 0xaf0a1b4cU, 0x36034af6U, 0x41047a60U, 0xdf60efc3U, 0xa867df55U,
	0x316e8eefU, 0x4669be79U, 0xcb61b38cU, 0xbc66831aU, 0x256fd2a0U, 0x5268e236U,
	0xcc0c7795U, 0xbb0b4703U, 0x220216b9U, 0x5505262fU, 0xc5ba3bbeU, 0xb2bd0b28U,
	0x2bb45a92U, 0x5cb36a04U, 0xc2d7ffa7U, 0xb5d0cf31U, 0x2cd99e8bU, 0x5bdeae1dU,
	0x9b64c2b0U, 0xec63f226U, 0x756aa39cU, 0x026d930aU, 0x9c0906a9U, 0xeb0e363fU,
	0x72076785U, 0x05005713U, 0x95bf4a82U, 0xe2b87a14U, 0x7bb12baeU, 0x0cb61b38U,
	0x92d28e9bU, 0xe5d5be0dU, 0x7cdcefb7U, 0x0bdbdf21U, 0x86d3d2d4U, 0xf1d4e242U,
	0x68ddb3f8U, 0x1fda836eU, 0x81be16cdU, 0xf6b9265bU, 0x6fb077e1U, 0x18b74777U,
	0x88085ae6U, 0xff0f6a70U, 0x66063bcaU, 0x11010b5cU, 0x8f659effU, 0xf862ae69U,
	0x616bffd3U, 0x166ccf45U, 0xa00ae278U, 0xd70dd2eeU, 0x4e048354U, 0x3903b3c2U,
	0xa7672661U, 0xd06016f7U, 0x4969474dU, 0x3e6e77dbU, 0xaed16a4aU, 0xd9d65adcU,
	0x40df0b66U, 0x37d83bf0U, 0xa9bcae53U, 0xdebb9ec5U, 0x47b2cf7fU, 0x30b5ffe9U,
	0xbdbdf21cU, 0xcabac28aU, 0x53b39330U, 0x24b4a3a6U, 0xbad03605U, 0xcdd70693U,
	0x54de5729U, 0x23d967bfU, 0xb3667a2eU, 0xc4614ab8U, 0x5d681b02U, 0x2a6f2b94U,
	0xb40bbe37U, 0xc30c8ea1U, 0x5a05df1bU, 0x2d02ef8dU
};

bool UpdateApp_EraseFlashMCU(void)
{
    uint32_t address;
    bool breturn = false;
    __disable_irq();
    for(uint32_t u32Index = 0U; u32Index < LENGTH_ERASE; u32Index++)
    {
        if(APP_POS == 0x0AU)
        {
            address = (uint32_t)ADDR_APPB_START + ((uint32_t)(SIZE_ERASE_256B) * u32Index);
        }else if(APP_POS == 0x0BU)
        {
            address = (uint32_t)ADDR_APPA_START + ((uint32_t)(SIZE_ERASE_256B) * u32Index);
        }else
        {/*NEED TO CHECK STATUS*/
            address = (uint32_t)ADDR_APPA_START + ((uint32_t)(SIZE_ERASE_256B) * u32Index);
        }
        breturn = NVMDriver_RowErase(address);
    }
    __enable_irq();
    sprintf((char *)u8TxBuffer,"MCU ERASE OK\r\n");
    UartDriver_TxWriteString(u8TxBuffer);
    RegisterApp_DHU_Setup(CMD_ERASE_FB,CMD_UPDATE_DATA_POS,CMD_FB_MCU_OK);
    I2CSlaveApp_UpdateCmdChecksumSet(CMD_ERASE_FB);
    return breturn;
}

static void UpdateApp_FlashMCU(uint8_t u8DataBuffer[],uint32_t u32DataSerialNumber)
{
    uint32_t address = 0U;
    __disable_irq();
    if(u32DataSerialNumber > 0)
    {
        address = (uint32_t)ADDR_APPA_START + ((uint32_t)(SIZE_PACKUNIT_128B) * (u32DataSerialNumber-1U));
    }else{/* Do nothing*/}
    
    if(APP_POS == 0x0A && u32DataSerialNumber >= POS_SN_AB_SEPERATE)
    {
        FlashApp_WriteRowFlash(&u8DataBuffer[0],address,SIZE_PACKUNIT_128B);
    }else if(APP_POS == 0x0B && u32DataSerialNumber < POS_SN_AB_SEPERATE)
    {
        FlashApp_WriteRowFlash(&u8DataBuffer[0],address,SIZE_PACKUNIT_128B);
    }else{
        /* Do nothing*/
    }
    __enable_irq();
}

bool UpdateApp_TransferFlashMCU(void)
{
    bool breturn = false;
    uint32_t u32DataSerialNumber = 0U;
    uint8_t u8DataBuffer[128];
    /* Data Collection*/
    u32DataSerialNumber = ((uint32_t)RegisterApp_DHU_Read(CMD_TRANSFER_REQ,2U)*256U)
                        + (uint32_t)RegisterApp_DHU_Read(CMD_TRANSFER_REQ,3U);
    for(uint32_t u32index = 0U;u32index < SIZE_PACKUNIT_128B;u32index++)
    {
        u8DataBuffer[u32index] = RegisterApp_DHU_Read(CMD_TRANSFER_REQ,(uint16_t)(4U+u32index));
        u16ChecksumMCU += u8DataBuffer[u32index];
    }
    /* Execute Flashing MCU*/
    if(u32DataSerialNumber > 0U)
    {
        UpdateApp_FlashMCU(&u8DataBuffer[0],u32DataSerialNumber);
        breturn = true;
    }else{
        breturn = false;
    }
    sprintf((char *)u8TxBuffer,"TRANSFER FINISHED - SN = %ld, Checksum = %d \r\n",u32DataSerialNumber,u16ChecksumMCU);
    UartDriver_TxWriteString(u8TxBuffer);
    if(breturn == true)
    {
        RegisterApp_DHU_Setup(CMD_TRANSFER_FB,CMD_UPDATE_DATA_POS,(uint8_t)u32DataSerialNumber >> 8U);
        RegisterApp_DHU_Setup(CMD_TRANSFER_FB,CMD_UPDATE_DATA_POS+1U,(uint8_t)u32DataSerialNumber);
        I2CSlaveApp_UpdateCmdChecksumSet(CMD_TRANSFER_FB);
    }
    return breturn;
}

static uint32_t UpdateApp_CRC_32Count(uint32_t pData[], uint32_t u32Len, uint32_t u32Crc32)
{
	/*  First, the polynomial itself and its table of feedback terms.  The
	  	polynomial is X^32+X^26+X^23+X^22+X^16+X^12+X^11+X^10+X^8+X^7+X^5+X^4+X^2+X^1+X^0 
	  	Polynomial = 0x04C11DB7 */
	uint32_t u32Index, u32TableIndex, u32BytePosit, u32ByteData;

	for(u32Index=0U; u32Index<u32Len; u32Index++)
	{
        for(u32BytePosit=0U; u32BytePosit< 4U; u32BytePosit++)
        {
            u32ByteData = (uint32_t)(pData[u32Index] >> (u32BytePosit*8U)) & (uint32_t)(0x000000FFU);
            u32TableIndex = (u32Crc32 ^ u32ByteData) & (uint32_t)(0xFFU);
            u32Crc32 = ((u32Crc32>>8U) & (uint32_t)(0x00FFFFFFU)) ^ crc32_tab[u32TableIndex];
        }
	}

	(void)(pData);
	(void)(u32Len);
	return u32Crc32;
}

static uint32_t UpdateApp_ByteReverse(uint32_t u32data)
{
    uint32_t rtndata = 0U;
    rtndata = rtndata | (u32data >> 24U);
    rtndata = rtndata | (((u32data >> 16U) & 0xFFU) << 8U);
    rtndata = rtndata | (((u32data >> 8U) & 0xFFU) << 16U);
    rtndata = rtndata | ((u32data & 0xFFU) << 24U);
    return rtndata;
}

static uint8_t UpdateApp_CheckSumMCU(void)
{
    
    uint32_t u32Index = 0U;
    uint32_t u32Size = 0U;
    uint8_t u8result = TRUE;
    uint32_t u32data[16U] = {0U};
    uint32_t u32RDdata[1U] = {0U};
    
    volatile uint32_t u32ComputeCrc32 = 0xFFFFFFFFU;

    uint32_t APP_START_ADDR = 0U;
    uint32_t APP_CRCREAD_ADDR = 0U;
    
    if(APP_POS == 0x0AU)
    {
        APP_START_ADDR = ADDR_APPB_START;
        APP_CRCREAD_ADDR = (uint32_t)(ADDR_APPB_START+SIZE_APP_BASE-RESERVED_DATA_SIZE);
     }else if(APP_POS == 0x0BU)
    {
        APP_START_ADDR = ADDR_APPA_START;
        APP_CRCREAD_ADDR = (uint32_t)(ADDR_APPA_START+SIZE_APP_BASE-RESERVED_DATA_SIZE);
    }else{/* DEBUG CHECK*/}

    __disable_irq();
    u32Size = (SIZE_APP_BASE/UNIT_READ);
    for(u32Index=0U;u32Index < u32Size;u32Index++)
    {
        (void)NVMDriver_Read(&u32data[0],64U,(uint32_t)(APP_START_ADDR) + (u32Index*(uint32_t)(UNIT_READ)));
        if(u32Index == (u32Size - RESERVED_DATA_LEN))
        {
            u32ComputeCrc32 = UpdateApp_CRC_32Count(&u32data[0],16U - RESERVED_DATA_LEN,u32ComputeCrc32);
        }
        else
        {
            u32ComputeCrc32 = UpdateApp_CRC_32Count(&u32data[0],16U,u32ComputeCrc32);
        }
    }
    u32ComputeCrc32 = ~u32ComputeCrc32;
    (void)NVMDriver_Read(&u32RDdata[0],4U,APP_CRCREAD_ADDR);
    for(u32Index=0U;u32Index < 4U;u32Index++)
    {
        if((u32RDdata[0]) == UpdateApp_ByteReverse(u32ComputeCrc32)){
            u8result = u8result & TRUE;
        }else{
            u8result = u8result & FALSE;
        }
    }

    __enable_irq();

    sprintf((char *)u8TxBuffer,"MCU CRC DONE. CRC:%lX u8MCUresult:%d size:%ld APP:%X APP Flag:%d RDdata:%08lX\r\n",UpdateApp_ByteReverse(u32ComputeCrc32),u8result,u32Index,APP_POS,0x0000000A,u32RDdata[0]);
    UartDriver_TxWriteString(u8TxBuffer);
    (void)u32data;
    (void)u8TxBuffer;
    return u8result;
}


bool UpdateApp_ChecksumFlashMCU(void)
{
    bool breturn = true;
    uint16_t u16ChecksumDHU = 0U;
    /* Data Collection*/
    u16ChecksumDHU = ((uint32_t)RegisterApp_DHU_Read(CMD_CRC_REQ,2U)*256U)
                        + (uint32_t)RegisterApp_DHU_Read(CMD_CRC_REQ,3U);

    /* Do Data Communication Checksum*/
    if(u16ChecksumDHU == u16ChecksumMCU)
    {
        breturn &= true;
    }else{
        breturn &= false;
    }

    /* Do MCU data self check*/
    if(UpdateApp_CheckSumMCU() == TRUE)
    {
        breturn &= true;
    }else{
        breturn &= false;
    }

    /* Configure the checksum result to CMD_CRC_FB*/
    if(breturn == true)
    {
        RegisterApp_DHU_Setup(CMD_CRC_FB,CMD_UPDATE_DATA_POS,CMD_FB_CHECKSUM_PASS);
        I2CSlaveApp_UpdateCmdChecksumSet(CMD_CRC_FB);
        /* Update Flag would return pass after software reset (Default value)*/
    }else{
        RegisterApp_DHU_Setup(CMD_CRC_FB,CMD_UPDATE_DATA_POS,CMD_FB_CHECKSUM_FAIL);
        I2CSlaveApp_UpdateCmdChecksumSet(CMD_CRC_FB);
        RegisterApp_DHU_Setup(CMD_UPDATESTATUS_FB,CMD_UPDATE_DATA_POS,CMD_FB_UPDATE_FAIL);
        I2CSlaveApp_UpdateCmdChecksumSet(CMD_UPDATESTATUS_FB);
    }

    return breturn;
}