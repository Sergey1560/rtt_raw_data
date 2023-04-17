#include "main.h"

struct data_packet_t data_packet;
uint8_t data_buffer[BUF_SIZE];

int main(void){
	int send;

	SEGGER_RTT_Init();
	SEGGER_RTT_ConfigUpBuffer(1,"Data",data_buffer,BUF_SIZE,SEGGER_RTT_MODE_NO_BLOCK_SKIP);
	
	RCC_init();
	crc_init();

	while(1){

		data_packet.head = 0xAABBAABB;
		data_packet.sensor1 += 1;
		data_packet.sensor2 += 3;
		data_packet.crc = 0;
		data_packet.crc = crc32_get((uint8_t*)&data_packet,sizeof(struct data_packet_t));
		
		send=SEGGER_RTT_Write(1, (const char *)&data_packet,sizeof(struct data_packet_t));
		
		DEBUG("Sending data. Sensor 1: %d Sensor 2: %d Send: %d",data_packet.sensor1,data_packet.sensor2,send);

		for(uint32_t i=0; i<0x1000000; i++){__NOP();};
	};


}
