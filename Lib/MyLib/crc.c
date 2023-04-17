#include "crc.h"

volatile uint32_t crc_running;

void crc_init(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;
    crc_running = 0;
};

uint32_t crc32_get(uint8_t *bfr, uint32_t len) {
        uint32_t l;
        uint32_t *p, x, crc=0;

        l = len / 4;
        p = (uint32_t*)bfr;
        x = p[l];

        if(crc_running > 0){
            ERROR("CRC still running");
            return 0;
        }else{
            crc_running = 1;
        }

        CRC->CR |= CRC_CR_RESET;
        while(l--) {
                CRC->DR = *p++;
                crc = CRC->DR;
        }
        switch(len & 3) {
                case 1:
                    CRC->DR = (x & 0x000000FF);
                    crc = CRC->DR;
                	break;
                case 2:
                    CRC->DR = (x & 0x0000FFFF);
                    crc = CRC->DR;
                	break;
                case 3:
                    CRC->DR = (x & 0x00FFFFFF);
                    crc = CRC->DR;
                	break;
        }
        
        crc_running = 0;
        return 0xFFFFFFFF ^ crc;
}

