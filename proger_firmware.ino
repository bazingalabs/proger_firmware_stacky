#include <stdint.h>
#include <I2C.h>

#define I2C_ADDR 0x10
#define CMD_READ_PAGE		0x10
#define CMD_WRITE_PAGE		0x20
#define CMD_ERASE_FLASH		0x30
#define CMD_SET_ADDRESS		0x40
#define CMD_WRITE_DBG		0x50



void setup() {
 Serial.begin(57600); 
 while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  I2c.begin();
  I2c.pullup(1);
  I2c.setSpeed(0);
  pinMode(5,OUTPUT);
  digitalWrite(5,HIGH);
 Serial.println("test start");
}

char str_buffer[270];
byte byte_buffer[133];

void loop() {
  if (Serial.available()) {
    memset(str_buffer,0,270);
    memset(byte_buffer,0xff,132);
    int num_read = Serial.readBytesUntil('\n', str_buffer, 270);
	if (num_read < 270) {
	    Serial.print("Num read: ");
	    Serial.println(num_read,DEC);
	    Serial.println(str_buffer);
	    if (str_buffer[0]==':') {
	      int count = convertHexStr(str_buffer[1],str_buffer[2]);
	      Serial.print("Count: ");
	      Serial.println(count,DEC);
	      int address = (convertHexStr(str_buffer[3],str_buffer[4]) << 8) | convertHexStr(str_buffer[5],str_buffer[6]);
	      Serial.print("Address: ");
	      Serial.println(address,DEC);
		  
	      int type = convertHexStr(str_buffer[7],str_buffer[8]);
	      Serial.print("Type: ");
	      Serial.println(type,DEC);
	      //Serial.print("Data: ");
	      for(int i=9,j=0;i<num_read-1;i+=2,j++) {
	        byte_buffer[j+3] = convertHexStr(str_buffer[i],str_buffer[i+1]);
	        Serial.print(byte_buffer[j+3],HEX);
	        Serial.print(" ");
	      }
		  data_cmd(byte_buffer,(num_read>>1)-4,address);
	      Serial.println(" ");
	    }
		if (str_buffer[0]=='b') {
			Serial.println("Boot");
		    I2c.write(I2C_ADDR,1,0x80);
		}
		if (str_buffer[0]=='v') {
			Serial.println("Version");
			uint8_t recvData[16];
		    I2c.read(I2C_ADDR,1,16,recvData);
		   
			for(int i = 0; i < 16; ++i)
			{
				Serial.print(char(recvData[i]));
				Serial.print(" ");
			}
		}
		
		// Reset pc6
		if (str_buffer[0]=='R') {
			Serial.println("Reset");
			digitalWrite(5,HIGH);
			delay(1);
			digitalWrite(5,LOW);
			delay(1);
			digitalWrite(5,HIGH);
		}
		
		if (str_buffer[0]=='r') {
			Serial.println("Read");
			
  	     	uint16_t address = (convertHexStr(str_buffer[2],str_buffer[3]) << 8) | convertHexStr(str_buffer[4],str_buffer[5]);
			Serial.print("Raw Address: ");
			Serial.println(&str_buffer[2]);
  	      	Serial.print("Read Address: ");
  	      	Serial.println(address,DEC);
			
			uint8_t cmd[3] = {1,0,0};
		    cmd[1] = str_buffer[3];
		    cmd[2] = str_buffer[2];
			memset(byte_buffer,0xff,132);
		    I2c.write(I2C_ADDR,2,cmd,3);
			I2c.read(I2C_ADDR,128,byte_buffer);

			for(int i = 0; i < 128; ++i)
			{
				Serial.print(byte_buffer[i],HEX);
				Serial.print(" ");
			}
		}
		/*
		if (str_buffer[0]=='i') {
			Serial.println("Info");
		    Wire.beginTransmission(I2C_ADDR);
			Wire.write(2);
		    Wire.write(0);
			Wire.write(0);
			Wire.write(0);
		    Wire.endTransmission();
			Wire.requestFrom(I2C_ADDR,8);
			for(int i = 0; i < 8; ++i)
			{
				Serial.print(Wire.read(),HEX);
				Serial.print(" ");
			}
		}*/
	}
  }
}

void address_cmd(uint16_t address) {
 /* Wire.beginTransmission(I2C_ADDR);
  Wire.write(CMD_SET_ADDRESS);
  Wire.write((address << 8)&0xff);
  Wire.write(address & 0xff);
  Wire.endTransmission();*/
}

void data_cmd(uint8_t * data, uint8_t size, uint16_t address) {
	Serial.println("\nSize: ");
	Serial.println(size,DEC);
	data[0] = 1;
    data[1] = (address >> 8) & 0xff;
    data[2] = (address & 0xff);
    I2c.write(I2C_ADDR,2,data,size+3);
}

byte getVal(char c)
{
   if(c >= '0' && c <= '9')
     return (byte)(c - '0');
   else
     return (byte)(c-'A'+10);
}

byte convertHexStr(char c1, char c2) {
  return (getVal(c1) << 4) | getVal(c2);
}
