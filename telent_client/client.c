#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h>
#include <arpa/inet.h>

#include "../src/packet.h"

#define BUF_SIZE    1024
#define PORT        19021

#define CRC32_POLY   0x04C11DB7
#define CRC32_POLY_R 0xEDB88320

uint32_t crc32_table[256];
uint32_t crc32r_table[256];

void crc32_init(void);
uint32_t crc32_stm32(uint32_t init_crc, uint8_t *buf, int len);

struct data_packet_t data_packet;


int main(int argc, char const *argv[]) 
{ 
    //struct sockaddr_in address; 
    struct sockaddr_in serv_addr; 
    int sock = 0; 
    char *hello = "$$SEGGER_TELNET_ConfigStr=RTTCh;1;$$";   //https://wiki.segger.com/RTT#RTTCh
    char buffer[BUF_SIZE] = {0}; 

    crc32_init();

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){ 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
    
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0){ 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){ 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 

    send(sock , hello , strlen(hello) , 0 ); 
    
    while(1){
        int read_count = recv( sock , buffer, BUF_SIZE, 0); 
        
        if(read_count > 0){
            uint32_t count = read_count / sizeof(struct data_packet_t);
            struct data_packet_t *data_ptr = (struct data_packet_t *)&buffer;

            while(count--){
                uint32_t crc = data_ptr->crc;
                data_ptr->crc = 0;
                if(crc32_stm32(0,(uint8_t *)data_ptr,sizeof(struct data_packet_t)) == crc ){
                    printf("[%d] Sensor1: %d Sensor2: %d \n",count,data_ptr->sensor1,data_ptr->sensor2);
                }else{
                    printf("Crc error\n");
                }
                data_ptr++;
            }
        }
    };
    return 0; 
} 

void crc32_init(void){
        int i, j;
        uint32_t c, cr;
        for (i = 0; i < 256; ++i) {
                cr = i;
                c = i << 24;
                for (j = 8; j > 0; --j) {
                        c = c & 0x80000000 ? (c << 1) ^ CRC32_POLY : (c << 1);
                        cr = cr & 0x00000001 ? (cr >> 1) ^ CRC32_POLY_R : (cr >> 1);
                        }
                crc32_table[i] = c;
                crc32r_table[i] = cr;
        }
}

uint32_t crc32_stm32(uint32_t init_crc, uint8_t *buf, int len){
        uint32_t v=0;
        uint32_t crc=0;
        uint32_t *ptr=(uint32_t *)buf;
        crc = ~init_crc;
        
        while(len >= 4) {
                v = htonl(*ptr++);
                crc = ( crc << 8 ) ^ crc32_table[0xFF & ( (crc >> 24) ^ (v ) )];
                crc = ( crc << 8 ) ^ crc32_table[0xFF & ( (crc >> 24) ^ (v >> 8) )];
                crc = ( crc << 8 ) ^ crc32_table[0xFF & ( (crc >> 24) ^ (v >> 16) )];
                crc = ( crc << 8 ) ^ crc32_table[0xFF & ( (crc >> 24) ^ (v >> 24) )];
                len -= 4;
        }
        if(len) {
                switch(len) {
                        case 1: v = 0xFF000000 & htonl(*ptr++); break;
                        case 2: v = 0xFFFF0000 & htonl(*ptr++); break;
                        case 3: v = 0xFFFFFF00 & htonl(*ptr++); break;
                }
                crc = ( crc << 8 ) ^ crc32_table[0xFF & ( (crc >> 24) ^ (v ) )];
                crc = ( crc << 8 ) ^ crc32_table[0xFF & ( (crc >> 24) ^ (v >> 8) )];
                crc = ( crc << 8 ) ^ crc32_table[0xFF & ( (crc >> 24) ^ (v >> 16) )];
                crc = ( crc << 8 ) ^ crc32_table[0xFF & ( (crc >> 24) ^ (v >> 24) )];
        }
        return ~crc;
}
