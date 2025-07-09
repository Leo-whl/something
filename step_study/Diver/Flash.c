#include "Flash.h"
/* Global define */
typedef enum {
    FAILED = 0,
    PASSED = !FAILED
} TestStatus;
#define PAGE_WRITE_START_ADDR       ((uint32_t)0x0800C800) //???????
#define PAGE_WRITE_END_ADDR         ((uint32_t)0x0800CC00) //???????
#define FLASH_PAGE_SIZE             4096

uint32_t EraseCounter = 0x0, Address = 0x0;
uint16_t Data       = 0xAAAA;
uint32_t WRPR_Value = 0xFFFFFFFF, ProtectedPages = 0x0;
uint32_t NbrOfPage;
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;

volatile TestStatus MemoryProgramStatus = PASSED;
volatile TestStatus MemoryEraseStatus   = PASSED;

void Write_CaliData_Array(uint16_t *data, uint32_t num)   //?????????--200?????
{
    uint32_t dataIndex = 0;
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV2; // ????
    __disable_irq();                       // ??????????
    FLASH_Unlock();                        // ???
    uint32_t pagesNeeded = ((num * sizeof(uint16_t)) + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); // ??????????

    for (EraseCounter = 0; (EraseCounter < pagesNeeded) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++) {
        FLASHStatus = FLASH_ErasePage(PAGE_WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter)); // Erase 4KB
    }
    Address = PAGE_WRITE_START_ADDR;
    while ((dataIndex < num) && (FLASHStatus == FLASH_COMPLETE)) {
        FLASHStatus = FLASH_ProgramHalfWord(Address, data[dataIndex]);
        Address += 0x02;
        dataIndex++;
    }
    FLASH_Lock();                           // ???
    RCC->CFGR0 &= ~(uint32_t)RCC_HPRE_DIV2; // ??????????
    __enable_irq();                         // ???????
}
void FLASH_Write_Original_Point(uint16_t num)  //???????????????????????
{
    uint32_t address = PAGE_WRITE_START_ADDR + 200 * sizeof(uint16_t); // ?????????????????

    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV2; // ????
    __disable_irq();                       // ??????????
    FLASH_Unlock();                        // ???Flash?????

    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); // ??????????

    // ?????????
    FLASHStatus = FLASH_ProgramHalfWord(address, num);

    // ???Flash?????
    FLASH_Lock();
    // ??????????
    RCC->CFGR0 &= ~(uint32_t)RCC_HPRE_DIV2;
    // ???????
    __enable_irq();
}
uint16_t Flash_Read_SectorAngle(int addr) //?????????200????????
{
    if (addr >= 0 && addr < 400) {
        if(addr < 0) addr += 200;
		if(addr >= 200) addr %= 200;
        uint32_t flashAddress = PAGE_WRITE_START_ADDR + (addr) * 2; // Calculate the address based on the index
        return *(__IO uint16_t *)flashAddress;
    }
}

uint16_t Flash_Read_Original_Point(void) //??????????????
{
    uint32_t address = PAGE_WRITE_START_ADDR + 200 * sizeof(uint16_t);
    return *(__IO uint16_t *)address;
}
