#ifndef JTAG_H
#define JTAG_H



//***Flash PROM instructions****//

#define BYPASS 0xFFFF
#define IDCODE 0x00FE

#define ISC_ADDRESS_SHIFT 0x00EB
#define ISC_DATA_SHIFT 0x00ED
#define ISC_DISABLE 0x00F0
#define ISC_ENABLE 0x00E8
#define ISC_ERASE 0x00EC
#define ISC_PROGRAM 0x00EA

#define XSC_BLANK_CHECK 0x000D
#define XSC_CLR_STATUS 0x00F4
#define XSC_CONFIG 0x00EE
#define XSC_DATA_BTC 0x00F2
#define XSC_DATA_CC 0x0007
#define XSC_DATA_CCB 0x000C
#define XSC_DATA_DONE 0x0009
#define XSC_DATA_RDPT 0x0004
#define XSC_DATA_SUCR 0x000E
#define XSC_DATA_UC 0x0006
#define XSC_DATA_WRPT 0x00F7
#define XSC_OP_STATUS 0x00E3
#define XSC_READ 0x00EF
#define XSC_UNLOCK 0xAA55



#define ENDIR_PAUSE 1
#define ENDIR_IDLE  0




#define JTAG_INIT 0x01
#define FLASH_ERASED 0x02
#define FLASH_WRITING 0x03
#define FLASH_ADDRESS_SHIFTED 0x04
#define FLASH_PROGRAMMED 0x05

#define FLASH_WRITE_BYTE_MAX 32

void jtag_init();
void jtag_reset();

void flashwrite(uint8_t *buf,u08 len);
void flasherase();
u08 flashfinish(uint8_t *len);
u08  flashprepare();




#endif //JTAG_H
