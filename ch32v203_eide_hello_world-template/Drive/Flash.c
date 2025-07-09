#include "Flash.h"  // ���� Flash ����ͷ�ļ�

/* Global define */
// �������״̬ö�٣�ʧ�ܻ�ͨ��
typedef enum {
    FAILED = 0,
    PASSED = !FAILED
} TestStatus;

// Flash д����ʼ��ַ���ڴ��ַ��
#define PAGE_WRITE_START_ADDR       ((uint32_t)0x0800C800)
// Flash д�������ַ
#define PAGE_WRITE_END_ADDR         ((uint32_t)0x0800CC00)
// Flash ÿҳ��СΪ 4KB
#define FLASH_PAGE_SIZE             4096

// ȫ�ֱ�������
uint32_t EraseCounter = 0x0, Address = 0x0;        // ������������д���ַָ��
uint16_t Data = 0xAAAA;                            // Ĭ��д������
uint32_t WRPR_Value = 0xFFFFFFFF, ProtectedPages = 0x0; // д�����Ĵ���ֵ���ܱ���ҳ��
uint32_t NbrOfPage;                                // ��ҳ��
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE; // Flash ����״̬

volatile TestStatus MemoryProgramStatus = PASSED;   // ��̲���״̬
volatile TestStatus MemoryEraseStatus = PASSED;     // ��������״̬

/**
 * @brief �� calibration ��������д�� Flash
 * @param data - ��д�������ָ��
 * @param num  - ������������λ��uint16_t��
 */
void Write_CaliData_Array(uint16_t *data, uint32_t num)
{
    uint32_t dataIndex = 0;
    
    // ���ϵͳʱ��Ƶ������ Flash ����
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV2;
    __disable_irq();            // �ر��жϣ���ֹ���������
    FLASH_Unlock();             // ���� Flash������д��/����

    // ������Ҫ���ٸ� Flash ҳ�����洢����
    uint32_t pagesNeeded = ((num * sizeof(uint16_t)) + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;

    // ��� Flash ��־λ
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    // ������ҳ�������ҳ����
    for (EraseCounter = 0; (EraseCounter < pagesNeeded) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++) {
        FLASHStatus = FLASH_ErasePage(PAGE_WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));
    }

    // ����д����ʼ��ַ
    Address = PAGE_WRITE_START_ADDR;

    // ��ʼд������
    while ((dataIndex < num) && (FLASHStatus == FLASH_COMPLETE)) {
        FLASHStatus = FLASH_ProgramHalfWord(Address, data[dataIndex]);
        Address += 0x02;                // ��ַ������һ������ = 2 �ֽڣ�
        dataIndex++;
    }

    FLASH_Lock();                         // д����ɣ����� Flash
    RCC->CFGR0 &= ~(uint32_t)RCC_HPRE_DIV2; // �ָ�ϵͳʱ������
    __enable_irq();                       // ���¿����ж�
}

/**
 * @brief д��ԭʼ�����ݵ� Flash ��ָ��λ�ã�ƫ�� 200��
 * @param num - Ҫд��� 16 λ��������
 */
void FLASH_Write_Original_Point(uint16_t num)
{
    // ����д���ַ����ʼ��ַ + ƫ�� 200 * ���ִ�С
    uint32_t address = PAGE_WRITE_START_ADDR + 200 * sizeof(uint16_t);

    // ���ϵͳʱ��Ƶ������ Flash ����
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV2;
    __disable_irq();               // �ر��ж�
    FLASH_Unlock();                // ���� Flash

    // �����־λ
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    // д��һ���������ݣ�16bit��
    FLASHStatus = FLASH_ProgramHalfWord(address, num);

    FLASH_Lock();                  // ���� Flash
    RCC->CFGR0 &= ~(uint32_t)RCC_HPRE_DIV2; // �ָ�ϵͳʱ������
    __enable_irq();                // �����ж�
}

/**
 * @brief �� Flash �ж�ȡ sector angle ����
 * @param addr - ������ַ��֧�ָ����Զ�ת����
 * @return ��ȡ���� 16 λ����
 */
uint16_t Flash_Read_SectorAngle(int addr)
{
    if (addr >= 0 && addr < 400) {
        if(addr < 0) addr += 200;
        if(addr >= 200) addr %= 200;

        // ����ʵ�� Flash ��ַ
        uint32_t flashAddress = PAGE_WRITE_START_ADDR + (addr) * 2;
        
        // ���ض�ȡ�İ�������
        return *(__IO uint16_t *)flashAddress;
    }
    return 0xFFFF; // �����ַ��Ч������Ĭ�ϴ���ֵ
}

/**
 * @brief ��ȡԭʼ�����ݣ��̶���ַƫ�� 200��
 * @return ��ȡ���� 16 λ����
 */
uint16_t Flash_Read_Original_Point(void)
{
    // �̶���ַƫ�� 200
    uint32_t address = PAGE_WRITE_START_ADDR + 200 * sizeof(uint16_t);
    
    // ���ظõ�ַ���� 16 λ����
    return *(__IO uint16_t *)address;
}