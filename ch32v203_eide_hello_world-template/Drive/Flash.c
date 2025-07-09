#include "Flash.h"  // 包含 Flash 驱动头文件

/* Global define */
// 定义测试状态枚举：失败或通过
typedef enum {
    FAILED = 0,
    PASSED = !FAILED
} TestStatus;

// Flash 写入起始地址（内存地址）
#define PAGE_WRITE_START_ADDR       ((uint32_t)0x0800C800)
// Flash 写入结束地址
#define PAGE_WRITE_END_ADDR         ((uint32_t)0x0800CC00)
// Flash 每页大小为 4KB
#define FLASH_PAGE_SIZE             4096

// 全局变量定义
uint32_t EraseCounter = 0x0, Address = 0x0;        // 擦除计数器、写入地址指针
uint16_t Data = 0xAAAA;                            // 默认写入数据
uint32_t WRPR_Value = 0xFFFFFFFF, ProtectedPages = 0x0; // 写保护寄存器值及受保护页码
uint32_t NbrOfPage;                                // 总页数
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE; // Flash 操作状态

volatile TestStatus MemoryProgramStatus = PASSED;   // 编程测试状态
volatile TestStatus MemoryEraseStatus = PASSED;     // 擦除测试状态

/**
 * @brief 将 calibration 数据数组写入 Flash
 * @param data - 待写入的数据指针
 * @param num  - 数据数量（单位：uint16_t）
 */
void Write_CaliData_Array(uint16_t *data, uint32_t num)
{
    uint32_t dataIndex = 0;
    
    // 提高系统时钟频率用于 Flash 操作
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV2;
    __disable_irq();            // 关闭中断，防止操作被打断
    FLASH_Unlock();             // 解锁 Flash，允许写入/擦除

    // 计算需要多少个 Flash 页面来存储数据
    uint32_t pagesNeeded = ((num * sizeof(uint16_t)) + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;

    // 清除 Flash 标志位
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    // 对所需页面进行逐页擦除
    for (EraseCounter = 0; (EraseCounter < pagesNeeded) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++) {
        FLASHStatus = FLASH_ErasePage(PAGE_WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));
    }

    // 设置写入起始地址
    Address = PAGE_WRITE_START_ADDR;

    // 开始写入数据
    while ((dataIndex < num) && (FLASHStatus == FLASH_COMPLETE)) {
        FLASHStatus = FLASH_ProgramHalfWord(Address, data[dataIndex]);
        Address += 0x02;                // 地址步进（一个半字 = 2 字节）
        dataIndex++;
    }

    FLASH_Lock();                         // 写入完成，锁定 Flash
    RCC->CFGR0 &= ~(uint32_t)RCC_HPRE_DIV2; // 恢复系统时钟配置
    __enable_irq();                       // 重新开启中断
}

/**
 * @brief 写入原始点数据到 Flash 的指定位置（偏移 200）
 * @param num - 要写入的 16 位整型数据
 */
void FLASH_Write_Original_Point(uint16_t num)
{
    // 计算写入地址：起始地址 + 偏移 200 * 半字大小
    uint32_t address = PAGE_WRITE_START_ADDR + 200 * sizeof(uint16_t);

    // 提高系统时钟频率用于 Flash 操作
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV2;
    __disable_irq();               // 关闭中断
    FLASH_Unlock();                // 解锁 Flash

    // 清除标志位
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    // 写入一个半字数据（16bit）
    FLASHStatus = FLASH_ProgramHalfWord(address, num);

    FLASH_Lock();                  // 锁定 Flash
    RCC->CFGR0 &= ~(uint32_t)RCC_HPRE_DIV2; // 恢复系统时钟设置
    __enable_irq();                // 启用中断
}

/**
 * @brief 从 Flash 中读取 sector angle 数据
 * @param addr - 索引地址（支持负数自动转换）
 * @return 读取到的 16 位数据
 */
uint16_t Flash_Read_SectorAngle(int addr)
{
    if (addr >= 0 && addr < 400) {
        if(addr < 0) addr += 200;
        if(addr >= 200) addr %= 200;

        // 计算实际 Flash 地址
        uint32_t flashAddress = PAGE_WRITE_START_ADDR + (addr) * 2;
        
        // 返回读取的半字数据
        return *(__IO uint16_t *)flashAddress;
    }
    return 0xFFFF; // 如果地址无效，返回默认错误值
}

/**
 * @brief 读取原始点数据（固定地址偏移 200）
 * @return 读取到的 16 位数据
 */
uint16_t Flash_Read_Original_Point(void)
{
    // 固定地址偏移 200
    uint32_t address = PAGE_WRITE_START_ADDR + 200 * sizeof(uint16_t);
    
    // 返回该地址处的 16 位数据
    return *(__IO uint16_t *)address;
}