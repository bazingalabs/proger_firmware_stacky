#include <stdint.h>
#include "I2C.h"
#include <avr/wdt.h>
#include <stdint.h>

uint8_t mcusr_mirror __attribute__ ((section (".noinit")));

void get_mcusr(void) \
 __attribute__((naked)) \
 __attribute__((section(".init3")));
void get_mcusr(void)
{
 mcusr_mirror = MCUSR;
 MCUSR = 0;
 wdt_disable();
}

#define I2C_ADDR 0x29
#define CMD_READ_PAGE		0x10
#define CMD_WRITE_PAGE		0x20
#define CMD_ERASE_FLASH		0x30
#define CMD_SET_ADDRESS		0x40
#define CMD_WRITE_DBG		0x50

#define SUPPORTED_DEVICE_CODE    0x73 

// SRAM Page buffer for flash page 
uint8_t pageBuffer[SPM_PAGESIZE + 3]; 

uint32_t address = 0; 

#define UART_RX_BUFFER_SIZE SPM_PAGESIZE 
//unsigned char gBuffer[UART_RX_BUFFER_SIZE]; 



void setup() {
	//wdt_disable();
 Serial.begin(57600); 
 while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  I2c.begin();
  I2c.pullup(1);
  I2c.setSpeed(0);
  pinMode(5,OUTPUT);
  digitalWrite(5,HIGH);
  delay(20);
  
 Serial.println("test start");


 wdt_enable(WDTO_4S);
}

void loop() {
	wdt_reset();
  if (Serial.available()) {
	  char cmd = Serial.read();
	  uint16_t tempSize;
      switch (cmd) 
      { 
         case 'P': // Step 8
		 	enterProgramMode(); 
            break; 
         case 'a': // Step 4
		 	autoIncrementAddress(); 
            break; 
         case 'A': // Step 10
		 	setAddress(); 
			//resetDevice();
			//enterBootloader();
            break; 
         case 'e': // Step 12
		 	chipErase();    
            break; 
         case 'L': // Step 13
		 	enterProgramMode(); 
            break; 
         case 'T': // Step 7
		 	selectDeviceType(); 
            break;                
         case 's': // Step 9
		 	readSignatureBytes();    
            break;          
         case 't': // Step 6
		 	returnSupportedDeviceCodes();    
            break; 
         case 'S': // Step 1
		 	returnSoftwareIdentifier();    
            break;          
         case 'V': // Step 2
		 	returnSoftwareVersion();    
            break;          
         case 'x': 
            //setLED(); 
            break; 
         case 'y': 
            //clearLED(); 
            break; 
         case 'p': // Step 3
		 	returnProgrammerType();    
            break;          
         case 'E': // Step 14
		 	exitBootloader();    
            break; 
         case 'b': // Step 5
		 	checkBlockSupport();    
            break; 
         case 'B': 
		 	tempSize = (receiveByte() << 8) | receiveByte(); 
            BlockLoad( tempSize, receiveByte() ); 
            break; 
         case 'g': // Step 11
		 	tempSize = (receiveByte() << 8) | receiveByte(); 
            BlockRead( tempSize, receiveByte() );   // Perform the block read 
            break;                            
         default: 
            if(cmd != 0x1B) sendByte('?'); 
      } 
  }
}

void sendByte(byte b) {
	Serial.write(b);
}

byte receiveByte() {
	while(!Serial.available());
	Serial.read();
}

void returnSoftwareIdentifier(void) // 'S' 
{ 
    // Software identifier is 'AVRBOOT' 
   sendByte('A'); 
    sendByte('V'); 
    sendByte('R'); 
    sendByte('B'); 
    sendByte('O'); 
    sendByte('O'); 
    sendByte('T'); 
} 

void returnSoftwareVersion(void) // 'V' 
{ 
   // Return software version. 
    sendByte('0'); 
    sendByte('1'); 
} 

void returnProgrammerType(void) // 'p' 
{      
   // Get programmer type - serial. 
    sendByte('S'); 
} 

void autoIncrementAddress(void) // 'a' 
{ 
   // Yes, this code autoincrements 
   sendByte('Y'); 
} 

void checkBlockSupport(void) // 'b' 
{ 
    sendByte('Y'); // yes, block load is supported. 
    sendByte((SPM_PAGESIZE>>8) & 0xFF); // send MSB first. 
    sendByte(SPM_PAGESIZE & 0xFF); // send LSB second. 
} 

void returnSupportedDeviceCodes(void) // 't' 
{ 
   sendByte(SUPPORTED_DEVICE_CODE); // Support only this device 
   sendByte(0); // list terminator 
}

void selectDeviceType() // 'T' 
{ 
   //dummy read since we only have one device type 
   uint8_t dummy; 
   dummy = receiveByte(); 
   sendByte('\r'); 
}

void enterProgramMode() // 'P' 
{ 
   resetDevice();
   I2c.write(I2C_ADDR,0);
   // what else is it going to do? 
   sendByte('\r'); 
} 

void readSignatureBytes(void) // 'S' 
{ 
	uint8_t cmd[3] = {0,0,0};
	uint8_t info_buffer[8]={0,0,0,0,0,0,0,0};
	memset(info_buffer,0,8);
	I2c.write(I2C_ADDR,2,cmd,3);
	I2c.read(I2C_ADDR,8,info_buffer);
    sendByte( info_buffer[2] ); 
    sendByte( info_buffer[1] ); 
    sendByte( info_buffer[0] ); 
} 

void setAddress(void) // 'A' 
{ 
   // Note that flash addresses are in words, not bytes                
    address = receiveByte();    
   address = (address<<8) | receiveByte(); 
   address = address << 1; // convert word address to byte address 

    sendByte('\r');  // respond okay 
} 

void resetDevice() {
    digitalWrite(5,LOW);
    delay(20);
    digitalWrite(5,HIGH);
	delay(5);
}

void enterBootloader() {
	I2c.write(I2C_ADDR,0);
}

void BlockRead(uint16_t size, uint8_t type) 
{ 
   if(type == 'F') 
   { 
      startBlockFlashRead(size);// Read flash 
   } 
 //  else if(type == 'E') 
 //  { 
     // startBlockEEPROMRead(size);// Read EEPROM 
 //  } 
   else sendByte('?'); 
} 

void startBlockFlashRead(uint16_t size) 
{ 
		uint8_t cmd[4] = {2,1,0,0};
		uint8_t * addr_p = (uint8_t*)&address; 
		//uint16_t waddress = address >> 1;
		cmd[2] = *(addr_p + 1);//(uint8_t)(((uint16_t)address) << 8); // Convert to word address
		cmd[3] = *addr_p;

		memset(pageBuffer,0,sizeof(pageBuffer)); 
	  	  I2c.read(I2C_ADDR,4,cmd,size,pageBuffer);
	     address = address + size;
	                       // send byte 
	    for(int i=0;i<size;i++) {
	                         // reduce number of bytes to read by one 
	    	wdt_reset();
	    	sendByte(pageBuffer[i]);
	  }       
} 


void chipErase(void) // 'e' 
{ 
   sendByte('\r');  // respond okay 
} 


void BlockLoad(uint16_t size, uint8_t type) 
{ 
   if(type == 'F') 
   { 
      startBlockFlashLoad(size);// load flash 
   } 
  /* else if(type == 'E') 
   { 
      startBlockEEPROMLoad(size);// load EEPROM 
   }*/ 
   else sendByte('?'); 
} 

void startBlockFlashLoad(uint16_t size) 
{ 

	uint16_t i,cnt,tempWord; 

	// store values to be programmed in temporary buffer 
	for (cnt=3; cnt<UART_RX_BUFFER_SIZE+3; cnt++) { 
		if (cnt<size+3) pageBuffer[cnt]=receiveByte(); 
		else pageBuffer[cnt]=0xFF; 
		wdt_reset();
	} 
	cnt=0; 

	pageBuffer[0] = 1;
	uint8_t * addr_p = (uint8_t*)&address; 

	pageBuffer[1] = *(addr_p + 1);
	pageBuffer[2] = *addr_p;

	I2c.write(I2C_ADDR,2,pageBuffer,SPM_PAGESIZE+3);

	address = address + size;     // word increment 
	delay(250);
	wdt_reset();
	sendByte('\r'); 
} 

void exitBootloader() 
{
    I2c.write(I2C_ADDR,1,0x80);
    // what else is it going to do? 
    sendByte('\r'); 
}