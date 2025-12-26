/* Arduino Nano 16 MHz programmer which can read/write/erase/identity 24,28,32 pins chips 8-bit EPROM/Flash 27,28,29-Series and testing RAM, logic tester 14, 16 pins chips */
/* Version 3.0 */

/* Voltage control (for programming chips) */
#define rTop 12000.0
#define rBottom 430.0
#define vRef 1.092
#define voltageControl A6
 
/* 74HC595 control (address lines) */
#include <SPI.h>
#define PIN_SS 10

/* Chip control panel (ZIF32) */
#define chipEnable           A5 //(CE)
#define outputEnable         A4 //(OE)
#define programVoltageEnable A3 // VPP for 27, 28F-Series (E_VPP)
#define identityEraseEnable  A2 //Identity and Erase pin (E_A9) 

/* DATA EPROM pins (ZIF32) */
#define D0     4    //PD4
#define PIN_D0 PD4
#define portD0 PORTD
#define pinD0  PIND
#define D1     3    //PD3
#define PIN_D1 PD3
#define portD1 PORTD
#define pinD1  PIND
#define D2     2    //PD2
#define PIN_D2 PD2
#define portD2 PORTD
#define pinD2  PIND
#define D3     9    //PB1
#define PIN_D3 PB1
#define portD3 PORTB
#define pinD3  PINB
#define D4     8    //PB0
#define PIN_D4 PB0
#define portD4 PORTB
#define pinD4  PINB
#define D5     7    //PD7
#define PIN_D5  PD7
#define portD5  PORTD
#define pinD5   PIND
#define D6     6    //PD6
#define PIN_D6 PD6
#define portD6 PORTD
#define pinD6  PIND
#define D7     5    //PD5
#define PIN_D7 PD5
#define portD7 PORTD
#define pinD7  PIND

/* DRAM pins (ZIF16) */
#define DO      11   //PB3
#define PIN_DO  PB3
#define portDO  PORTB
#define pinDO   PINB
#define DI      8    //PB0
#define PIN_DI  PB0
#define portDI  PORTB
#define CAS     12   //PB4
#define PIN_CAS PB4
#define portCAS PORTB
#define RAS     6    //PD6
#define PIN_RAS PD6
#define portRAS PORTD
#define WE      7    //PD7
#define PIN_WE  PD7
#define portWE  PORTD
#define DA0     5    //PD5
#define PIN_DA0 PD5
#define portDA0 PORTD
#define DA1     3    //PD3
#define PIN_DA1 PD3 
#define portDA1 PORTD
#define DA2     4    //PD4
#define PIN_DA2 PD4
#define portDA2 PORTD
#define DA3     A0   //PC0
#define PIN_DA3 PC0
#define portDA3 PORTC
#define DA4     A1   //PC1
#define PIN_DA4 PC1
#define portDA4 PORTC
#define DA5     A5   //PC5
#define PIN_DA5 PC5
#define portDA5 PORTC
#define DA6     10   //PB2
#define PIN_DA6 PB2
#define portDA6 PORTB
#define DA7     2    //PD2
#define PIN_DA7 PD2
#define portDA7 PORTD
#define DA8     9    //PB1
#define PIN_DA8 PB1
#define portDA8 PORTB
#define E_DVCC  13   //PB5

/* LOGIC pins (ZIF16) */
#define P1  2
#define P2  A5
#define P3  A1
#define P4  A0
#define P5  10
#define P6  11
#define P7  12
#define P8  A7   //A7 used to fill an array
#define P9  9
#define P10 8
#define P11 7
#define P12 6
#define P13 5
#define P14 4
#define P15 3
#define E_P16 13 //D13 only output

typedef enum chipType {
  NONE,
  E27C16,  E27C32,  E27C64,  E27C128, E27C256, E27C512, E27C010, E27C020, E27C040, E27C080,
  E28C04,  E28C16,  E28C17,  E28C64,  E28C256, E28C512, E28C010, E28C020, E28C040,
  F28F256, F28F512, F28F010, F28F020,
  F29C256, F29C512, F29C010, F29C020, F29C040,
  F29F512, F29F010, F29F020, F29F040,
  SRAM16,  SRAM32,  SRAM64,  SRAM128, SRAM256, SRAM512, SRAM010, SRAM020, SRAM040
} Chip;

typedef enum mode {
  READ,
  WRITE,
  ERASE,
  ERASE28F,
  SOFT_ERASE,
  IDENTITY,
  SOFT_IDENTITY,
  EXTRA_ROW_READ,
  EXTRA_ROW_WRITE,
  TEST_DRAM, 
  TEST_LOGIC,
  AUTO_TEST_LOGIC,
  VOLTAGE,
  WAIT,
} Modes;

void prog_mode(void);
void select_chip(chipType new_chip);
void read_mode(void);
void write_mode(void);
uint32_t gen_address(uint32_t address);
void set_address(uint32_t address);
uint8_t get_data(void);
void set_data(uint8_t data);
uint8_t read_byte(uint32_t address);
void write_byte(uint32_t address, uint8_t data);
void data_protection_disable(void);
void data_protection_enable(void);
void dram_mode(void);
void setBus_Dram(uint16_t a);
void writeAddress_Dram(uint16_t r, uint16_t c, uint8_t v);
uint8_t readAddress_Dram(uint16_t r, uint16_t c);
void logic_mode(void);
boolean test_logic(void); 
float get_voltage(void);

chipType chip = NONE;
Modes mode = WAIT;
uint8_t pt = 0;
uint8_t type = 0; //27-27C, 28-28C, 58-28F, 29-29C, 59-29F, 66-SRAM 
uint8_t dip = 0;
uint32_t end_address = 0x00000;
uint8_t buf[4][64];
uint16_t page = 0;
uint8_t module = 0;
uint8_t mfour = 0;
uint16_t CEPulse = 0;
uint8_t sdp = 0;
uint8_t erow_page = 32;
uint8_t error = 0;
float VEE = 0.0;
uint8_t type_Dram = 0;
uint8_t busSize_Dram = 0;
const uint8_t Pin14[14] = {P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15};
const uint8_t Pin16[16] = {P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15,E_P16};
uint8_t PinOut[16];
uint8_t Signal[16];
uint8_t Pin_max = 0;
uint8_t Test_count = 0;
boolean Part = LOW;
uint8_t test_low[16];
uint8_t single_test = 0;

void setup() {
  prog_mode();
  pt = 1;
 
  pinMode(outputEnable, OUTPUT);
  digitalWrite(outputEnable, HIGH);
  pinMode(programVoltageEnable, OUTPUT);
  digitalWrite(programVoltageEnable, LOW);
  pinMode(identityEraseEnable, OUTPUT);
  digitalWrite(identityEraseEnable, LOW);

  analogReference(INTERNAL);
  analogRead(voltageControl);
  
  Serial.begin(115200);
  Serial.print("APT");
}

void loop() {
  switch(mode) {
// Read mode
    case READ:
      if(chip == NONE || type == 0) {
        //return 0x45 to say it error
        Serial.write(0x45);
        mode = WAIT;
        break;
      }
      read_mode();
      set_address(0); // ~WE high
      digitalWrite(chipEnable, LOW);
      digitalWrite(outputEnable, LOW);
      for(uint32_t i = 0; i <= end_address; i++) {
        uint8_t data = read_byte(i);
        Serial.write(&data, sizeof(data));
      }
      digitalWrite(chipEnable, HIGH);
      digitalWrite(outputEnable, HIGH);
      mode = WAIT;
      set_address(0);
      break;
   
// Write mode  
    case WRITE:    
      if(chip == NONE || type == 0) {
        //return 0x45 to say it error
        Serial.write(0x45);
        sdp = 0;
        module = 0;
        mode = WAIT;
        break;
      }
      if(type == 27) {
        float VPPmax = 0.0;
        if(chip == E27C16 || chip == E27C32) {VPPmax = 26.0;} else
        if(chip == E27C64 || chip == E27C128 || chip == E27C256) {VPPmax = 22.0;}
        else {VPPmax = 14.0;}
        if(get_voltage() < 11.5 || get_voltage() > VPPmax) {
          //return 0x46 to say it incorrect voltage
          Serial.write(0x46);
          mode = WAIT;
          break;
        }
      }
      if(type == 58) {
        if(get_voltage() < 11.5 || get_voltage() > 12.5) {
          //return 0x46 to say it incorrect voltage
          Serial.write(0x46);
          mode = WAIT;
          break;
        }
      }
      digitalWrite(chipEnable, HIGH);
      digitalWrite(outputEnable, HIGH);
      write_mode();
      if(type == 27 || type == 58) {
        if(chip == E27C16) {digitalWrite(chipEnable, LOW);}
        digitalWrite(programVoltageEnable, HIGH);
      }     
      //SDP Disable
      if(sdp && (type == 28 || type == 29)) {data_protection_disable();}
      for(uint32_t i = 0; i < end_address; i += 256) {
        for(uint8_t n = 0; n < 4; n++) {
          //return 0x53 to start write block
          Serial.write(0x53);     
          uint8_t count = Serial.readBytes(buf[n], 64);
          if(count != 64) {
            //return 0x44 to say it error write block
            Serial.write(0x44);
            error = 1;
            break;
          }
        }
        if(error == 1) {break;}
        //SDP Disable (Module)
        if(module == 1 && sdp && type == 28) {
          if(chip == E28C010) {
            if(i == 32768) {
              if(sdp == 2) {mfour = 0; data_protection_enable();}
              mfour = 1; data_protection_disable(); mfour = 0;
            } else
            if(i == 65536) {
              if(sdp == 2) {mfour = 1; data_protection_enable();}
              mfour = 2; data_protection_disable(); mfour = 0;
            } else 
            if(i == 98304) {
              if(sdp == 2) {mfour = 2; data_protection_enable();}
              mfour = 3; data_protection_disable(); mfour = 0;
            }
          } else
          if(chip == E28C020) {
            if(i == 65536) {
              if(sdp == 2) {mfour = 0; data_protection_enable();}
              mfour = 1; data_protection_disable(); mfour = 0;
            } else 
            if(i == 131072) {
              if(sdp == 2) {mfour = 1; data_protection_enable();}
              mfour = 2; data_protection_disable(); mfour = 0;
            } else 
            if(i == 196608) {
              if(sdp == 2) {mfour = 2; data_protection_enable();}
              mfour = 3; data_protection_disable(); mfour = 0;
            } 
          } else
          if(chip == E28C040) {
            if(i == 131072) {
              if(sdp == 2) {mfour = 0; data_protection_enable();}
              mfour = 1; data_protection_disable(); mfour = 0;
            } else 
            if(i == 262144) {
              if(sdp == 2) {mfour = 1; data_protection_enable();}
              mfour = 2; data_protection_disable(); mfour = 0;
            } else 
            if(i == 393216) {
              if(sdp == 2) {mfour = 2; data_protection_enable();}
              mfour = 3; data_protection_disable(); mfour = 0;
            }
          }
        }
        uint32_t ni = i;
        for(uint8_t n = 0; n < 4; n++) { 
          for(uint8_t j = 0; j < 64; j++) {
            if(type == 58) {
              for(uint8_t c = 0; c < 25; c++) {
                write_byte(ni + j, 0x40);
                write_byte(ni + j, buf[n][j]);        
                write_byte(ni + j, 0xC0);
                mode = READ;
                set_address(ni + j); // ~WE high
                read_mode();
                digitalWrite(chipEnable, LOW);
                digitalWrite(outputEnable, LOW);
                uint8_t data = get_data();
                digitalWrite(chipEnable, HIGH);
                digitalWrite(outputEnable, HIGH);
                mode = ERASE28F;
                write_mode();    
                if(data == buf[n][j]) {break;}
                if(c == 24) {
                  //return 0x45 to say writing failed
                  Serial.write(0x45);
                  error = 1;
                }          
              }
            } else {
              if(type == 59) {
                write_byte(0x5555, 0xAA);
                write_byte(0x2AAA, 0x55);
                write_byte(0x5555, 0xA0);
              }
              write_byte(ni + j, buf[n][j]);
              if(type == 28 || type == 29) {
                if(page == 1) {delay(10);}
                else if(page == 16 && (j == 15 || j == 31 || j == 47 || j == 63)) {delay(10);}
                else if(page == 32 && (j == 31 || j == 63)) {delay(10);}
                else if(page == 64 && j == 63) {delay(10);}
                else if(page == 128 && j == 63 && (n == 1 || n == 3)) {delay(10);}
                else if(page == 256 && j == 63 && n == 3) {delay(10);}  
              }
              if(type == 59) {delayMicroseconds(100);}
            }
            if(error == 1) {break;}
          }
          ni += 64;
          if(error == 1) {break;}  
        }
        if(error == 1) {break;}
        if(i >= end_address-256) { 
          //return 0x4F to say writing complete
          Serial.write(0x4F);
        }
      }
      //SDP Enabled
      if(sdp == 2 && (type == 28 || type == 29)) {
        if(module == 1) {mfour = 3;}
        data_protection_enable();
      }    
      if(type == 58) {
        write_byte(0, 0xFF);
        write_byte(0, 0xFF);
      }
      if(type == 59) {
        write_byte(0, 0xF0);
      }
      read_mode();
      digitalWrite(programVoltageEnable, LOW); 
      if(chip == E27C16) {digitalWrite(chipEnable, HIGH);}
      sdp = 0;
      module = 0;
      mfour = 0;
      error = 0;
      mode = WAIT;
      set_address(0);
      break;

// Erase mode  
    case ERASE:
      if(chip == NONE || (type != 27 && type != 28)) {
        //return 0x45 to say it error
        Serial.write(0x45);
        mode = WAIT;
        break;
      } 
      if(type == 27) {VEE = 14.5;} else {VEE = 12.5;}
      if(get_voltage() < 11.5 || get_voltage() > VEE) {
        //return 0x46 to say it incorrect voltage
        Serial.write(0x46);
        mode = WAIT;
        break;
      }
      digitalWrite(chipEnable, HIGH);
      digitalWrite(outputEnable, HIGH); 
      read_mode();
      if(type == 28) {
        mode = READ;
        set_address(0); // ~WE high
        mode = ERASE;
        digitalWrite(programVoltageEnable, HIGH);
        digitalWrite(chipEnable, LOW);
        set_address(0); // ~WE low
        delay(20);
        mode = READ;
        set_address(0); // ~WE high
        mode = ERASE;
        digitalWrite(chipEnable, HIGH);
        digitalWrite(programVoltageEnable, LOW);
      } else if(type = 27) {
        set_address(0);
        digitalWrite(programVoltageEnable, HIGH);
        digitalWrite(identityEraseEnable, HIGH);
        digitalWrite(chipEnable, LOW);
        delay(105);
        digitalWrite(chipEnable, HIGH);
        digitalWrite(identityEraseEnable, LOW);
        digitalWrite(programVoltageEnable, LOW);
      }
      //return 0x4F to say it chip erase passed 
      Serial.write(0x4F);
      mode = WAIT;
      set_address(0); 
      break;
      
// Erase mode for 28Fxxx  
    case ERASE28F: 
      if(chip == NONE || type != 58) {
        //return 0x45 to say it error
        Serial.write(0x45);
        mode = WAIT;
        break;
      }
      if(get_voltage() < 11.5 || get_voltage() > 12.5) {
        //return 0x46 to say it incorrect voltage
        Serial.write(0x46);
        mode = WAIT;
        break;
      }
      uint32_t p5, p20, p35, p50, p80, p90;
      p5  = end_address/13;
      p20 = end_address*10/33;
      p35 = end_address*10/19;
      p50 = end_address*10/13;
      p80 = end_address*10/23;
      p90 = end_address*10/13;
      digitalWrite(chipEnable, HIGH);
      digitalWrite(outputEnable, HIGH);
      write_mode();
      digitalWrite(programVoltageEnable, HIGH);
      for(uint32_t i = 0; i <= end_address; i++) {  
        for(uint8_t n = 0; n < 25; n++) {
          write_byte(i, 0x40);
          write_byte(i, 0x00);         
          write_byte(i, 0xC0);
          mode = READ;
          set_address(i); // ~WE high
          read_mode();
          digitalWrite(chipEnable, LOW);
          digitalWrite(outputEnable, LOW);
          uint8_t data = get_data();
          digitalWrite(chipEnable, HIGH);
          digitalWrite(outputEnable, HIGH);
          mode = ERASE28F;
          write_mode();    
          if(data == 0x00) {break;}
          if(n == 24) {
            //return 0x44 to say writing failed
            Serial.write(0x44);
            error = 1;
          }          
        }
        if(error == 1) {break;}
        if(i == p5) { 
          //return 0x50 to say 5% progress
          Serial.write(0x50);
        }
        if(i == p20) { 
          //return 0x51 to say 20% progress
          Serial.write(0x51);
        }
        if(i == p35) { 
          //return 0x52 to say 35% progress
          Serial.write(0x52);
        }
        if(i == p50) { 
          //return 0x53 to say 50% progress
          Serial.write(0x53);
        }
      }
      write_byte(0, 0xFF);
      write_byte(0, 0xFF);     
      if(error != 1) {
        //return 0x54 to say writing complete
        Serial.write(0x54);
        uint32_t lstErAdr = 0;      
        for(uint16_t n = 0; n < 1000; n++) {
          write_byte(0, 0x20);;
          write_byte(0, 0x20);
          delay(10);
          for(uint32_t i = lstErAdr; i <= end_address; i++) {
            write_byte(i, 0xA0);
            mode = READ;
            set_address(i); // ~WE high
            read_mode(); 
            digitalWrite(chipEnable, LOW);
            digitalWrite(outputEnable, LOW);
            uint8_t data = get_data();
            digitalWrite(chipEnable, HIGH);
            digitalWrite(outputEnable, HIGH);
            mode = ERASE28F;
            write_mode();
            if(data != 0xFF) {break;}
            lstErAdr++;
            if(i == p80) { 
              //return 0x55 to say 80% progress
              Serial.write(0x55);
            }
            if(i == p90) { 
              //return 0x56 to say 90% progress
              Serial.write(0x56);
            }
          }
          if(lstErAdr > end_address) {
            //return 0x4F to say erasing complete
            Serial.write(0x4F);
            break;
          }
          if(n == 999) {
            //return 0x47 to say erasing failed
            Serial.write(0x47);
          }
        }        
        write_byte(0, 0xFF);
        write_byte(0, 0xFF);
      }
      read_mode();  
      digitalWrite(programVoltageEnable, LOW);
      error = 0;
      mode = WAIT;
      set_address(0);
      break;

// Software chip erase
    case SOFT_ERASE:
      if(chip == NONE || (type != 28 && type != 29 && type != 59)) {
        //return 0x45 to say it error
        Serial.write(0x45);
        mode = WAIT;
        break;
      }
      digitalWrite(chipEnable, HIGH);
      digitalWrite(outputEnable, HIGH);
      write_mode();
      write_byte(0x5555, 0xAA);
      write_byte(0x2AAA, 0x55);
      write_byte(0x5555, 0x80);
      write_byte(0x5555, 0xAA);
      write_byte(0x2AAA, 0x55);
      write_byte(0x5555, 0x10);
      if(type == 59) {
        delay(5000);
      } else {
        delay(50);
      }
      //return 0x4F to say it chip erase passed
      Serial.write(0x4F);
      if(type == 59) {
        write_byte(0, 0xF0);
      }
      read_mode();
      mode = WAIT;
      set_address(0);
      break;

// Identity mode 27,28F,29C,29F-Series (Autoselect)
    case IDENTITY:
      if(type == 0 || type == 28 || type == 66) {
        //return 0x45 to say it error
        Serial.write(0x45);
        mode = WAIT;
        break;
      }   
      if(get_voltage() < 11.5 || get_voltage() > 12.5) {
        //return 0x46 to say it incorrect voltage
        Serial.write(0x46);
        mode = WAIT;
        break;
      }
      read_mode();     
      mode = READ;
      set_address(0x48000); // ~WE high
      digitalWrite(chipEnable, LOW);
      digitalWrite(outputEnable, LOW);
      digitalWrite(identityEraseEnable, HIGH);      
      if(dip == 24) {
        for(uint8_t n = 1; n <= 2; n++) {       
          if(n == 1) {chip = E27C16;}
          if(n == 2) {chip = E27C32;}
          for(uint8_t i = 0; i <= 1; i++) {
            uint8_t data = read_byte(i);
            Serial.write(&data, sizeof(data));
          }      
        }
      } else if(dip == 28) {
        for(uint8_t n = 1; n <= 3; n++) {
          if(n == 1) {chip = E27C64;}  //E27C128
          if(n == 2) {chip = E27C256;} //F29C256
          if(n == 3) {chip = E27C512;}        
          for(uint8_t i = 0; i <= 1; i++) {
            uint8_t data = read_byte(i);
            Serial.write(&data, sizeof(data));
          }     
        }
      } else if(dip == 32) {
        for(uint8_t n = 1; n <= 4; n++) {
          if(n == 1) {chip = E27C010;} //E27C020, F29F010, F29F020
          if(n == 2) {chip = E27C040;} 
          if(n == 3) {chip = E27C080;}
          if(n == 4) {chip = F28F256;} //F28F512, F28F010, F28F020, F29C512, F29C010, F29C020, F29C040, F29F512, F29F040
          for(uint8_t i = 0; i <= 1; i++) {
            uint8_t data = read_byte(i);
            Serial.write(&data, sizeof(data));
          }
        }
      }               
      digitalWrite(identityEraseEnable, LOW);
      digitalWrite(chipEnable, HIGH);
      digitalWrite(outputEnable, HIGH);
      chip = NONE;
      mode = WAIT;
      set_address(0);
      break;

// Software Identity mode 29C-Series
    case SOFT_IDENTITY:
      if(type != 29) {
        //return 0x45 to say it error
        Serial.write(0x45);
        mode = WAIT;
        break;
      } 
      set_address(0x48000); // ~WE high
      digitalWrite(outputEnable, HIGH);
      digitalWrite(chipEnable, LOW);
      write_mode();
      set_address(0x4D555);
      set_data(0xAA);
      set_address(0x5555);
      set_address(0x4AAAA);
      set_data(0x55);
      set_address(0x2AAA);
      set_address(0x4D555);
      set_data(0x90);
      set_address(0x5555);
      set_address(0x48000); // ~WE high
      delay(10);
      mode = READ;
      read_mode();
      digitalWrite(outputEnable, LOW);
      if(dip == 28) {
        chip = F29C256;
        for(uint8_t i = 0; i <= 1; i++) {
          uint8_t data = read_byte(i);
          Serial.write(&data, sizeof(data));
        }
      } else if(dip == 32) {
        chip = F29C512; //F29C010, F29C020, F29C040
        for(uint8_t i = 0; i <= 1; i++) {
          uint8_t data = read_byte(i);         
          Serial.write(&data, sizeof(data));
        }
      }
      mode = SOFT_IDENTITY;               
      digitalWrite(chipEnable, HIGH);
      digitalWrite(outputEnable, HIGH);
      write_mode();
      write_byte(0x5555, 0xAA);
      write_byte(0x2AAA, 0x55);
      write_byte(0x5555, 0xF0);
      read_mode();
      chip = NONE; 
      mode = WAIT;
      set_address(0);
      break;

// Device Identification Read 28-Series
    case EXTRA_ROW_READ:
      if(chip == NONE || type != 28) {
        //return 0x45 to say it error
        Serial.write(0x45);
        mode = WAIT;
        break;
      }
      if(get_voltage() < 11.5 || get_voltage() > 12.5) {
        //return 0x46 to say it incorrect voltage
        Serial.write(0x46);
        mode = WAIT;
        break;
      }
      read_mode();
      mode = READ;
      set_address(0); // ~WE high
      digitalWrite(chipEnable, LOW);
      digitalWrite(outputEnable, LOW);
      digitalWrite(identityEraseEnable, HIGH);
      if(chip == E28C16 || chip == E28C17) {  
        for(uint16_t i = 0x7E0; i <= 0x7FF; i++) {
          uint8_t data = read_byte(i);
          Serial.write(&data, sizeof(data));
        }
      } else if(chip == E28C64) {
        if(erow_page == 32) {
          for(uint16_t i = 0x1FE0; i <= 0x1FFF; i++) {
            uint8_t data = read_byte(i);
            Serial.write(&data, sizeof(data));
          }
        } else {
          for(uint16_t i = 0x1FC0; i <= 0x1FFF; i++) {
            uint8_t data = read_byte(i);
            Serial.write(&data, sizeof(data));
          }
        }
      } else if(chip == E28C256) {
        for(uint16_t i = 0x7FC0; i <= 0x7FFF; i++) {
          uint8_t data = read_byte(i);
          Serial.write(&data, sizeof(data));
        }
      } else if(chip == E28C010) {
        for(uint32_t i = 0x1FF80; i <= 0x1FFFF; i++) {
          uint8_t data = read_byte(i);
          Serial.write(&data, sizeof(data));
        }
      } else if(chip == E28C040) {
        for(uint32_t i = 0x7FF80; i <= 0x7FFFF; i++) {
          uint8_t data = read_byte(i);
          Serial.write(&data, sizeof(data));
        }  
      }
      digitalWrite(identityEraseEnable, LOW);
      digitalWrite(outputEnable, HIGH);
      digitalWrite(chipEnable, HIGH);
      mode = WAIT;
      set_address(0);
      break;
             
// Device Identification Write 28-Series
    case EXTRA_ROW_WRITE:
      if(chip == NONE || type != 28) {
        //return 0x45 to say it error
        Serial.write(0x45);
        mode = WAIT;      
        break; 
      } 
      if(get_voltage() < 11.5 || get_voltage() > 12.5) {
        //return 0x46 to say it incorrect voltage
        Serial.write(0x46);
        mode = WAIT;
        break;
      }
      digitalWrite(chipEnable, HIGH);
      digitalWrite(outputEnable, HIGH);
      write_mode();
      digitalWrite(identityEraseEnable, HIGH);
      //return 0x53 to start write
      Serial.write(0x53);  
      uint16_t b;
      b = 0; 
      if(chip == E28C16 || chip == E28C17) {
        uint8_t count = Serial.readBytes(buf[0], 32);
        if(count == 32) {
          for(uint16_t i = 0x7E0; i <= 0x7FF; i++) {
            write_byte(i, buf[0][b]);
            b++;
            delay(10);
          }
        } else {error = 1;}
      } else if(chip == E28C64) {
        if(erow_page == 32) {
          uint8_t count = Serial.readBytes(buf[0], 32);  
          if(count == 32) {
            for(uint16_t i = 0x1FE0; i <= 0x1FFF; i++) {
              write_byte(i, buf[0][b]);
              b++;
              delay(10);
            }
          } else {error = 1;} 
        } else {  
          uint8_t count = Serial.readBytes(buf[0], 64);  
          if(count == 64) {
            for(uint16_t i = 0x1FC0; i <= 0x1FFF; i++) {
              write_byte(i, buf[0][b]);
              b++;
              delay(10);
            }
          } else {error = 1;} 
        }       
      } else if(chip == E28C256) {
        uint8_t count = Serial.readBytes(buf[0], 64); 
        if(count == 64) {
          for(uint16_t i = 0x7FC0; i <= 0x7FFF; i++) {
            write_byte(i, buf[0][b]);
            b++;
          }
          delay(10);
        } else {error = 1;} 
      } else if(chip == E28C010) {
        uint8_t count = Serial.readBytes(buf[0], 64); 
        if(count == 64) {
          for(uint32_t i = 0x1FF80; i <= 0x1FFBF; i++) {
            write_byte(i, buf[0][b]);
            b++;
          }
          delay(10);
        } else {error = 1;} 
        if(error != 1) {
          //return 0x54 to continue write
          Serial.write(0x54);
          b = 0; 
          count = Serial.readBytes(buf[0], 64); 
          if(count == 64) {
            for(uint32_t i = 0x1FFC0; i <= 0x1FFFF; i++) {
              write_byte(i, buf[0][b]);
              b++;
            }
            delay(10);
          } else {error = 1;}   
        }
      } else if(chip == E28C040) {
        uint8_t count = Serial.readBytes(buf[0], 64); 
        if(count == 64) {
          for(uint32_t i = 0x7FF80; i <= 0x7FFBF; i++) {
            write_byte(i, buf[0][b]);
            b++;
          }
          delay(10);
        } else {error = 1;} 
        if(error != 1) {
          //return 0x54 to continue write
          Serial.write(0x54);
          b = 0; 
          count = Serial.readBytes(buf[0], 64); 
          for(uint32_t i = 0x7FFC0; i <= 0x7FFFF; i++) {
            write_byte(i, buf[0][b]);
            b++;
          }
          delay(10);
        }  
      }
      if(error == 1) {
        //return 0x47 to say it error write block
        Serial.write(0x47);
      } else {
        //return 0x4F to say it complete block passed
        Serial.write(0x4F);
      }    
      digitalWrite(identityEraseEnable, LOW);
      read_mode(); 
      mode = WAIT;
      set_address(0);
      error = 0;
      break;

// Testing DRAM IC
    case TEST_DRAM:
      if(pt != 2 || type_Dram == 0) {
        //return 0x45 to say it error
        Serial.write(0x45);
        mode = WAIT;
        break;
      }
      digitalWrite(E_DVCC, LOW);
      digitalWrite(WE, HIGH);
      digitalWrite(RAS, HIGH);
      digitalWrite(CAS, HIGH);
      delayMicroseconds(250); //Initial DRAM startup delay 250us
      for(uint8_t i = 0; i < 8; i++) {
          digitalWrite(RAS, LOW);
          digitalWrite(RAS, HIGH);
      }
      noInterrupts();
      uint32_t sendErr;
      sendErr = 0;    
      uint8_t v;
      //Test 1
      v = 0; 
      for(uint16_t c = 0; c < (1<<busSize_Dram); c++) {
        for(uint16_t r = 0; r < (1<<busSize_Dram); r++) {
          writeAddress_Dram(r, c, v);
          if(v != readAddress_Dram(r, c)) {
            sendErr = ((uint32_t)c << busSize_Dram) + r;
            error = 1;
            break;    
          }
          v = !v;
        }
        if(error == 1) {break;}
      }
      //Test 2
      if(!error) {
        interrupts();
        //return 0x50 to say 20% progress
        Serial.write(0x50);
        noInterrupts();
        v = 1;
        for(uint16_t c = 0; c < (1<<busSize_Dram); c++) {
          for(uint16_t r = 0; r < (1<<busSize_Dram); r++) {
            writeAddress_Dram(r, c, v);
            if(v != readAddress_Dram(r, c)) {
              sendErr = ((uint32_t)c << busSize_Dram) + r;
              error = 1;
              break;
            }
            v = !v;
          }
          if(error == 1) {break;}
        }
      }
      //Test 3
      if(!error) {
        interrupts();
        //return 0x51 to say 40% progress
        Serial.write(0x51);
        noInterrupts();
        v = 0;
        for(uint16_t c = 0; c < (1<<busSize_Dram); c++) {
          for(uint16_t r = 0; r < (1<<busSize_Dram); r++) {
            writeAddress_Dram(r, c, v);
            v = !v;
          }
        }
        v = 0;
        for(uint16_t c = 0; c < (1<<busSize_Dram); c++) {
          for(uint16_t r = 0; r < (1<<busSize_Dram); r++) {
            if(v != readAddress_Dram(r, c)) {
              sendErr = ((uint32_t)c << busSize_Dram) + r;
              error = 1;
              break;             
            }
            v = !v;
          }
          if(error == 1) {break;}
        }
      }
      //Test 4
      if(!error) {
        interrupts();
        //return 0x52 to say 60% progress
        Serial.write(0x52);
        noInterrupts();
        v = 1;
        for(uint16_t c = 0; c < (1<<busSize_Dram); c++) {
          for(uint16_t r = 0; r < (1<<busSize_Dram); r++) {
            writeAddress_Dram(r, c, v);
            v = !v;
          }
        }
        v = 1;
        for(uint16_t c = 0; c < (1<<busSize_Dram); c++) {
          for(uint16_t r = 0; r < (1<<busSize_Dram); r++) {
            if(v != readAddress_Dram(r, c)) {
              sendErr = ((uint32_t)c << busSize_Dram) + r;
              error = 1;
              break;             
            }
            v = !v;
          }
          if(error == 1) {break;}
        }
      }
      //Test 5
      if(!error) {
        interrupts();
        //return 0x53 to say 80% progress
        Serial.write(0x53);
        noInterrupts();
        v = 0;
        for(uint16_t c = 0; c < (1<<busSize_Dram); c++) {
          for(uint16_t r = 0; r < (1<<busSize_Dram); r++) {
            writeAddress_Dram(r, c, v);
            if(v != readAddress_Dram(r, c)) {
              sendErr = ((uint32_t)c << busSize_Dram) + r;
              error = 1;
              break;
            }       
          }
          if(error == 1) {break;}
        }
      }
      interrupts();
      if(!error) {
        //return 0x4F to say it all tests result ok
        Serial.write(0x4F);
      } else {
        //return 0x46 to say it result test error
        Serial.write(0x46);
        Serial.print(sendErr);
      }
      dram_mode();
      error = 0;
      mode = WAIT;
      break;    

// Testing Logic IC
    case TEST_LOGIC:  
      if(pt != 3) {
        //return 0x45 to say it error
        Serial.write(0x45);
        mode = WAIT;
        break;
      }
      single_test = 1;
      //return 0x42 to begin
      Serial.write(0x42);
      Signal[0] = 0;
      Signal[1] = 0;              
      Serial.readBytes(Signal, 2);
      Pin_max = Signal[0];
      Test_count = Signal[1];
      if((Pin_max == 14 || Pin_max == 16) && Test_count > 1 && Test_count < 50) {
        if(Pin_max == 14) {
          for(uint8_t i = 0; i < 14; i++) {PinOut[i] = Pin14[i];}
        } else
        if(Pin_max == 16) {
          for(uint8_t i = 0; i < 16; i++) {PinOut[i] = Pin16[i];}
        }
        //return 0x43 to continue
        Serial.write(0x43);
        for(uint8_t i = 0; i < Test_count; i++) {
          uint8_t count = Serial.readBytes(Signal, Pin_max);
          if(count != Pin_max) {
            //return 0x44 to data transmission error
            Serial.write(0x44);
            break;
          }         
          Part = test_logic();
          if(Part == LOW) {
            //return to say it result test error
            Serial.write(test_low, Pin_max);
          } else 
          if(Part == HIGH) {
            //return 0x4F to say it test result ok
            Serial.write(0x4F);                                          
          }
        }
      } else {
         //return 0x44 to data transmission error
         Serial.write(0x44);    
      }
      single_test = 0;
      logic_mode();
      mode = WAIT;
      break;

// AUTO TEST_LOGIC 
    case AUTO_TEST_LOGIC:
      if(pt != 3) {
        //return 0x45 to say it error
        Serial.write(0x45);
        mode = WAIT;
        break;
      }
      single_test = 0;
      //return 0x42 to begin
      Serial.write(0x42);
      while(true) {
        Signal[0] = 0;
        Signal[1] = 0;              
        Serial.readBytes(Signal, 2);
        if(Signal[0] == 1 && Signal[1] == 1) {
          break;
        }      
        Pin_max = Signal[0];
        Test_count = Signal[1];
        if((Pin_max == 14 || Pin_max == 16) && Test_count > 1 && Test_count < 50) {
          if(Pin_max == 14) {
            for(uint8_t i = 0; i < 14; i++) {PinOut[i] = Pin14[i];}
          } else
          if(Pin_max == 16) {
            for(uint8_t i = 0; i < 16; i++) {PinOut[i] = Pin16[i];}
          }
          //return 0x43 to continue
          Serial.write(0x43);
          for(uint8_t i = 0; i < Test_count; i++) { 
            uint8_t count = Serial.readBytes(Signal, Pin_max);
            if(count != Pin_max) {
              //return 0x44 to data transmission error
              Serial.write(0x44);
              break;
            }
            Part = test_logic();
            if(Part == LOW) {
              //return 0x41 to say it result test error
              Serial.write(0x41);
              break;
            } else 
            if(Part == HIGH) {
              //return 0x4F to say it test result ok
              Serial.write(0x4F);                                         
            }            
          }
        } else {
          //return 0x44 to data transmission error
          Serial.write(0x44);             
        } 
        logic_mode();   
      }
      logic_mode();
      mode = WAIT;
      break;

// Voltage 
    case VOLTAGE:
      Serial.print(get_voltage(), 1);
      delay(1);
      mode = WAIT;
      break;

    default:  
      do{} while(Serial.available() == 0);
      char incomingByte = Serial.read();     
      switch (incomingByte) {       
        case '1':
          if(type==27) {select_chip(E27C16);}
          else if(type==28){select_chip(E28C04);}
          else if(type==58){select_chip(F28F256);}
          else if(type==29){select_chip(F29C256);}
          else if(type==59){select_chip(F29F512);}
          else if(type==66){select_chip(SRAM16);}
          break; 
        case '2':
          if(type==27) {select_chip(E27C32);}
          else if(type==28){select_chip(E28C16);}
          else if(type==58){select_chip(F28F512);}
          else if(type==29){select_chip(F29C512);}
          else if(type==59){select_chip(F29F010);}
          else if(type==66){select_chip(SRAM32);}
          break;
        case '3':
          if(type==27) {select_chip(E27C64);}
          else if(type==28){select_chip(E28C17);}
          else if(type==58){select_chip(F28F010);}
          else if(type==29){select_chip(F29C010);}
          else if(type==59){select_chip(F29F020);}
          else if(type==66){select_chip(SRAM64);}
          break; 
        case '4':
          if(type==27) {select_chip(E27C128);}
          else if(type==28){select_chip(E28C64);}
          else if(type==58){select_chip(F28F020);}
          else if(type==29){select_chip(F29C020);}
          else if(type==59){select_chip(F29F040);}
          else if(type==66){select_chip(SRAM128);}
          break; 
        case '5':
          if(type==27){select_chip(E27C256);}
          else if(type==28){select_chip(E28C256);}
          else if(type==29){select_chip(F29C040);}
          else if(type==66){select_chip(SRAM256);}
          break;
        case '6':
          if(type==27){select_chip(E27C512);}
          else if(type==28){select_chip(E28C512);}
          else if(type==66){select_chip(SRAM512);}
          break;              
        case '7':
          if(type==27){select_chip(E27C010);}
          else if(type==28){select_chip(E28C010);}
          else if(type==66){select_chip(SRAM010);}
          break;
        case '8':
          if(type==27){select_chip(E27C020);}
          else if(type==28){select_chip(E28C020);}
          else if(type==66){select_chip(SRAM020);}
          break;          
        case '9':
          if(type==27){select_chip(E27C040);}
          else if(type==28){select_chip(E28C040);}
          else if(type==66){select_chip(SRAM040);}
          break; 
        case '0':
          if(type==27){select_chip(E27C080);}
          break;
        case 'a': type = 27; chip = NONE; break; 
        case 'b': type = 28; chip = NONE; break;
        case 'c': type = 58; chip = NONE; break;
        case 'd': type = 29; chip = NONE; break;
        case 'f': type = 59; chip = NONE; break; 
        case 'g': type = 66; chip = NONE; break;         
        case 'x': dip = 24; chip = NONE; break;
        case 'y': dip = 28; chip = NONE; break;
        case 'z': dip = 32; chip = NONE; break;                     
        case 'r': mode = READ; break;
        case 'w': mode = WRITE; break;
        case 'e': if(type==58) {mode = ERASE28F;} else {mode = ERASE;} break;       
        case 's': mode = SOFT_ERASE; break;  
        case 'i': mode = IDENTITY; chip = NONE; break;
        case 'h': mode = SOFT_IDENTITY; chip = NONE; break;
        case 'o': mode = EXTRA_ROW_READ; break;
        case 'p': mode = EXTRA_ROW_WRITE; break;
        case '~': mode = TEST_DRAM; break; 
        case 'u': mode = TEST_LOGIC; break;
        case 'n': mode = AUTO_TEST_LOGIC; break;    
        case 'v': mode = VOLTAGE; break;
        case 'm': if(pt != 1) {prog_mode(); pt = 1;} break;
        case 'j': type_Dram = 0; if(pt != 2) {if(pt == 1) {SPI.end();} dram_mode(); type = 0; chip = NONE; pt = 2;} break;    
        case 'l': if(pt != 3) {if(pt == 1) {SPI.end();} logic_mode(); type = 0; chip = NONE; pt = 3;} break;
        case 'q': sdp = 1; break;
        case 't': sdp = 2; break;
        case 'k': module = 1; break;       
        case '(': CEPulse = 25; break;
        case ')': CEPulse = 50; break;
        case '{': CEPulse = 100; break;
        case '}': CEPulse = 200; break;
        case '[': CEPulse = 500; break;
        case ']': CEPulse = 1000; break;
        case '<': CEPulse = 5000; break;
        case '>': CEPulse = 10000; break;
        case '|': CEPulse = 50000; break;
        case ':': page = 1; break;
        case ';': page = 16; break;
        case '.': page = 32; break;
        case ',': page = 64; break;
        case '!': page = 128; break;
        case '?': page = 256; break;
        case '@': erow_page = 32; break;
        case '%': erow_page = 64; break;
        case '-': type_Dram = 1; busSize_Dram = 8; break; //4164
        case '+': type_Dram = 2; busSize_Dram = 9; break; //41256    
      }
  }
}

//Setup pins for prog
void prog_mode(void) {
  // 74HC595
  digitalWrite(PIN_SS, HIGH);
  pinMode(PIN_SS, OUTPUT);
  SPI.begin();
  // Chip control
  digitalWrite(chipEnable, HIGH);
  pinMode(chipEnable, OUTPUT);
  // Data pins
  read_mode();
}

void select_chip(chipType new_chip) {
  switch(new_chip) {
    case E27C16:  chip = E27C16;  end_address = 0x007ff; break;
    case E27C32:  chip = E27C32;  end_address = 0x00fff; break;
    case E27C64:  chip = E27C64;  end_address = 0x01fff; break;
    case E27C128: chip = E27C128; end_address = 0x03fff; break;
    case E27C256: chip = E27C256; end_address = 0x07fff; break;
    case E27C512: chip = E27C512; end_address = 0x0ffff; break;   
    case E27C010: chip = E27C010; end_address = 0x1ffff; break;
    case E27C020: chip = E27C020; end_address = 0x3ffff; break;
    case E27C040: chip = E27C040; end_address = 0x7ffff; break;
    case E27C080: chip = E27C080; end_address = 0xfffff; break;
    case E28C04:  chip = E28C04;  end_address = 0x001ff; break;          
    case E28C16:  chip = E28C16;  end_address = 0x007ff; break;
    case E28C17:  chip = E28C17;  end_address = 0x007ff; break;      
    case E28C64:  chip = E28C64;  end_address = 0x01fff; break;
    case E28C256: chip = E28C256; end_address = 0x07fff; break;
    case E28C512: chip = E28C512; end_address = 0x0ffff; break;
    case E28C010: chip = E28C010; end_address = 0x1ffff; break;
    case E28C020: chip = E28C020; end_address = 0x3ffff; break;
    case E28C040: chip = E28C040; end_address = 0x7ffff; break;
    case F28F256: chip = F28F256; end_address = 0x07fff; break;
    case F28F512: chip = F28F512; end_address = 0x0ffff; break;
    case F28F010: chip = F28F010; end_address = 0x1ffff; break;
    case F28F020: chip = F28F020; end_address = 0x3ffff; break;                       
    case F29C256: chip = F29C256; end_address = 0x07fff; break; 
    case F29C512: chip = F29C512; end_address = 0x0ffff; break;
    case F29C010: chip = F29C010; end_address = 0x1ffff; break;
    case F29C020: chip = F29C020; end_address = 0x3ffff; break;
    case F29C040: chip = F29C040; end_address = 0x7ffff; break; 
    case F29F512: chip = F29F512; end_address = 0x0ffff; break;
    case F29F010: chip = F29F010; end_address = 0x1ffff; break;
    case F29F020: chip = F29F020; end_address = 0x3ffff; break;
    case F29F040: chip = F29F040; end_address = 0x7ffff; break;                
    case SRAM16:  chip = SRAM16;  end_address = 0x007ff; break;
    case SRAM32:  chip = SRAM32;  end_address = 0x00fff; break;
    case SRAM64:  chip = SRAM64;  end_address = 0x01fff; break;
    case SRAM128: chip = SRAM128; end_address = 0x03fff; break;
    case SRAM256: chip = SRAM256; end_address = 0x07fff; break;
    case SRAM512: chip = SRAM512; end_address = 0x0ffff; break;
    case SRAM010: chip = SRAM010; end_address = 0x1ffff; break;
    case SRAM020: chip = SRAM020; end_address = 0x3ffff; break;
    case SRAM040: chip = SRAM040; end_address = 0x7ffff; break;
    default:      chip = NONE;    end_address = 0x00000;
  }
}

void read_mode(void) {
  pinMode(D0, INPUT_PULLUP);
  pinMode(D1, INPUT_PULLUP);
  pinMode(D2, INPUT_PULLUP);
  pinMode(D3, INPUT_PULLUP);
  pinMode(D4, INPUT_PULLUP);
  pinMode(D5, INPUT_PULLUP);
  pinMode(D6, INPUT_PULLUP);
  pinMode(D7, INPUT_PULLUP);
}

void write_mode(void) {
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
}

uint32_t gen_address(uint32_t address) {
  switch(chip) {
    case E27C16:
    case E28C04:      
    case E28C16:
    case SRAM16:
      if(mode == READ) {
        address |= (1 << 11); // VPP(~WE) high
      }
      break;
    case E27C32:
    case E27C512:
    case E27C080:
      break;
    case E27C64:
    case E27C128:
      if(mode == READ) {
        address |= (1 << 14); // ~PGM high
        address |= (1 << 15); //  VPP high 
      }
      break;
    case E27C256:
    case F29C256:
      if(mode == READ) {
        address |= (1 << 15); // VPP(~WE) high      
      }
      break;  
    case E27C010:
    case E27C020:
      if(mode == READ) {
        address |= (1L << 18); // ~PGM high
        address |= (1L << 19); // VPP high      
      }
      break;
    case E27C040:
      if(mode == READ) {
        address |= (1L << 19); // VPP high      
      }
      break; 
    case E28C17:
    case E28C64:
    case SRAM128:
      if(mode == READ) {
        address |= (1 << 14); // ~WE high
      }
      break;   
    case E28C256:
    case SRAM256:
      if(address & (1 << 14)) {address |= (1 << 15);} // Set A14
      if(mode == READ) {       
        address |= (1 << 14); // ~WE high
      } else {
        address &= ~(1 << 14); // ~WE low
      }
      break;  
    case E28C512:
    case F28F256:
    case F28F512:
    case F28F010:
    case F28F020:
    case F29C512:
    case F29C010:
    case F29C020:
    case F29F512:
      if(mode == READ) {
        address |= (1L << 18); // ~WE high    
      }
      break;
    case F29F010:
    case F29F020:
      if(mode == READ) {
        address |= (1L << 18); // ~WE high    
      }
      if(mode != WAIT) {
        address |= (1L << 19); // ~RES high    
      }
      break;        
    case E28C010:
      if(mfour == 1) {address |= (1 << 15);} // module sdp
      else if(mfour == 2) {address |= (1L << 16);} // module sdp
      else if(mfour == 3) {address |= (1 << 15); address |= (1L << 16);} // module sdp
      if(mode == READ) {
        address |= (1L << 18); // ~WE high    
      }
      break;
    case E28C020:
      if(mfour == 1) {address |= (1L << 16);} // module sdp
      else if(mfour == 2) {address |= (1L << 17);} // module sdp
      else if(mfour == 3) {address |= (1L << 16); address |= (1L << 17);} // module sdp
      if(mode == READ) {
        address |= (1L << 18); // ~WE high    
      }
      break;
    case E28C040:
      if(mfour == 1) {address |= (1L << 17);} // module sdp
      else if(mfour == 2) {address |= (1L << 18);} // module sdp
      else if(mfour == 3) {address |= (1L << 17); address |= (1L << 18);} // module sdp
      if(address & (1L << 18)) {address |= (1L << 19);} // Set A18
      if(mode == READ) {
        address |= (1L << 18); // ~WE high    
      } else {
        address &= ~(1L << 18); // ~WE low
      }
      break; 
    case F29C040:
    case F29F040:
      if(address & (1L << 18)) {address |= (1L << 19);} // Set A18
      if(mode == READ) {
        address |= (1L << 18); // ~WE high    
      } else {
        address &= ~(1L << 18); // ~WE low
      }
      break;      
    case SRAM32:
    case SRAM64:
      if(mode != WAIT) {
        address |= (1 << 13); // CE2 high
      }  
      if(mode == READ) {
        address |= (1 << 14); // ~WE high
      }
      break;
    case SRAM512:
    case SRAM010:
      if(address & (1 << 15)) {address |= (1L << 18);} // Set A15
      if(address & (1 << 14)) {address |= (1 << 15);} else {address &= ~(1 << 15);} // Set A14
      if(mode == READ) {       
        address |= (1 << 14); // ~WE high
      } else {
        address &= ~(1 << 14); // ~WE low
      }
      if(mode != WAIT) {
        address |= (1L << 17); // CE2 high
      }
      break;
    case SRAM020:
      if(address & (1 << 15)) {address |= (1L << 18);} // Set A15
      if(address & (1 << 14)) {address |= (1 << 15);} else {address &= ~(1 << 15);} // Set A14
      if(mode == READ) {
        address |= (1 << 14); // ~WE high
      } else {
        address &= ~(1 << 14); // ~WE low
      }
      break;
    case SRAM040:
      if(address & (1L << 18)) {address |= (1L << 19);} // Set A18
      if(address & (1 << 15)) {address |= (1L << 18);} else {address &= ~(1L << 18);} // Set A15
      if(address & (1 << 14)) {address |= (1 << 15);} else {address &= ~(1 << 15);} // Set A14
      if(mode == READ) {       
        address |= (1 << 14); // ~WE high
      } else {
        address &= ~(1 << 14); // ~WE low
      }
      break;    
  }
  return address;
}

void set_address(uint32_t address) {
  address = gen_address(address);
  digitalWrite(PIN_SS, LOW);  // shift register selection
  SPI.transfer((address>>16)&0xFF); 
  SPI.transfer((address>>8)&0xFF);
  SPI.transfer(address&0xFF);
  digitalWrite(PIN_SS, HIGH);  // end of transmission
}

uint8_t get_data(void) {
  uint8_t data = 0;
  (pinD0 & (1 << PIN_D0)) ? data |= 1 << 0 : 0;
  (pinD1 & (1 << PIN_D1)) ? data |= 1 << 1 : 0;
  (pinD2 & (1 << PIN_D2)) ? data |= 1 << 2 : 0;
  (pinD3 & (1 << PIN_D3)) ? data |= 1 << 3 : 0;
  (pinD4 & (1 << PIN_D4)) ? data |= 1 << 4 : 0;
  (pinD5 & (1 << PIN_D5)) ? data |= 1 << 5 : 0;
  (pinD6 & (1 << PIN_D6)) ? data |= 1 << 6 : 0;
  (pinD7 & (1 << PIN_D7)) ? data |= 1 << 7 : 0;
  return data;
}

void set_data(uint8_t data) {
  (data & 1)   ? portD0 |= 1 << PIN_D0 : portD0 &= ~(1 << PIN_D0);
  (data & 2)   ? portD1 |= 1 << PIN_D1 : portD1 &= ~(1 << PIN_D1);
  (data & 4)   ? portD2 |= 1 << PIN_D2 : portD2 &= ~(1 << PIN_D2);
  (data & 8)   ? portD3 |= 1 << PIN_D3 : portD3 &= ~(1 << PIN_D3);
  (data & 16)  ? portD4 |= 1 << PIN_D4 : portD4 &= ~(1 << PIN_D4);
  (data & 32)  ? portD5 |= 1 << PIN_D5 : portD5 &= ~(1 << PIN_D5);
  (data & 64)  ? portD6 |= 1 << PIN_D6 : portD6 &= ~(1 << PIN_D6);
  (data & 128) ? portD7 |= 1 << PIN_D7 : portD7 &= ~(1 << PIN_D7);
}

uint8_t read_byte(uint32_t address) {
  set_address(address);
  delayMicroseconds(2);
  return get_data();
}

void write_byte(uint32_t address, uint8_t data) {
  set_address(address);
  set_data(data);
  switch(chip) {
    case E27C16:
      digitalWrite(chipEnable, HIGH);
      if(CEPulse == 50000) {
        delay(50);
      } else {
        delayMicroseconds(CEPulse);
      }
      digitalWrite(chipEnable, LOW);
      break;
    case E27C32:
    case E27C64:
    case E27C128:
    case E27C256:
    case E27C512:
    case E27C010:
    case E27C020:
    case E27C040:
    case E27C080:
      digitalWrite(chipEnable, LOW);
      if(CEPulse == 50000) {
        delay(50);
      } else {
        delayMicroseconds(CEPulse);
      }
      digitalWrite(chipEnable, HIGH);      
      break;
    case E28C04:
    case E28C16:
    case E28C17:
    case E28C64:
    case E28C256:
    case E28C512:
    case E28C010:
    case E28C020:
    case E28C040:
    case F28F256:
    case F28F512:
    case F28F010:
    case F28F020:
    case F29C256: 
    case F29C512:
    case F29C010:
    case F29C020:
    case F29C040:
    case F29F512:
    case F29F010:
    case F29F020:
    case F29F040:
    case SRAM16:
    case SRAM32:
    case SRAM64:
    case SRAM128:
    case SRAM256:
    case SRAM512:
    case SRAM010:
    case SRAM020:
    case SRAM040:
      digitalWrite(chipEnable, LOW);
      digitalWrite(chipEnable, HIGH);
      break;
  }
}

// Software Data Protection Disable
void data_protection_disable(void) {
  switch(chip) {
    case E28C16:
      write_byte(0x555, 0xAA);
      write_byte(0x2AA, 0x55);
      write_byte(0x555, 0x80);
      write_byte(0x555, 0xAA);
      write_byte(0x2AA, 0x55);
      write_byte(0x555, 0x20);
      break;
   case E28C64:
      write_byte(0x1555, 0xAA);
      write_byte(0x0AAA, 0x55);
      write_byte(0x1555, 0x80);
      write_byte(0x1555, 0xAA);
      write_byte(0x0AAA, 0x55);
      write_byte(0x1555, 0x20);
      break;
   case E28C256:
   case E28C512:
   case E28C010:
   case E28C020:
   case E28C040:
   case F29C256:
   case F29C512:
   case F29C010:
   case F29C020:
   case F29C040:
      write_byte(0x5555, 0xAA);
      write_byte(0x2AAA, 0x55);
      write_byte(0x5555, 0x80);
      write_byte(0x5555, 0xAA);
      write_byte(0x2AAA, 0x55);
      write_byte(0x5555, 0x20);
      break;
   }
   delay(10);
}

// Software Data Protection Enable
void data_protection_enable(void) {
  switch(chip) {
    case E28C16:
      write_byte(0x555, 0xAA);
      write_byte(0x2AA, 0x55);
      write_byte(0x555, 0xA0);
      break;
   case E28C64:
      write_byte(0x1555, 0xAA);
      write_byte(0x0AAA, 0x55);
      write_byte(0x1555, 0xA0);
      break;
   case E28C256:
   case E28C512:
   case E28C010:
   case E28C020:
   case E28C040:
   case F29C256:
   case F29C512:
   case F29C010:
   case F29C020:
   case F29C040:
      write_byte(0x5555, 0xAA);
      write_byte(0x2AAA, 0x55);
      write_byte(0x5555, 0xA0);
      break;
   }
   delay(10);
}

//Setup pins for test DRAM
void dram_mode(void) {
  pinMode(DO, INPUT_PULLUP);
  digitalWrite(DI, LOW);
  pinMode(DI, OUTPUT);  
  digitalWrite(CAS, LOW);
  pinMode(CAS, OUTPUT);
  digitalWrite(RAS, LOW);
  pinMode(RAS, OUTPUT);
  digitalWrite(WE, LOW);
  pinMode(WE, OUTPUT);
  digitalWrite(DA0, LOW);
  pinMode(DA0, OUTPUT);
  digitalWrite(DA1, LOW);
  pinMode(DA1, OUTPUT);
  digitalWrite(DA2, LOW);
  pinMode(DA2, OUTPUT);
  digitalWrite(DA3, LOW);
  pinMode(DA3, OUTPUT);
  digitalWrite(DA4, LOW);
  pinMode(DA4, OUTPUT);
  digitalWrite(DA5, LOW);
  pinMode(DA5, OUTPUT);          
  digitalWrite(DA6, LOW);
  pinMode(DA6, OUTPUT);
  digitalWrite(DA7, LOW);
  pinMode(DA7, OUTPUT);
  digitalWrite(DA8, LOW);
  pinMode(DA8, OUTPUT);
  digitalWrite(E_DVCC, HIGH);
  pinMode(E_DVCC, OUTPUT);
}

void setBus_Dram(uint16_t a) {
  (a & 1)   ? portDA0 |= 1 << PIN_DA0 : portDA0 &= ~(1 << PIN_DA0);
  (a & 2)   ? portDA1 |= 1 << PIN_DA1 : portDA1 &= ~(1 << PIN_DA1);
  (a & 4)   ? portDA2 |= 1 << PIN_DA2 : portDA2 &= ~(1 << PIN_DA2);
  (a & 8)   ? portDA3 |= 1 << PIN_DA3 : portDA3 &= ~(1 << PIN_DA3);
  (a & 16)  ? portDA4 |= 1 << PIN_DA4 : portDA4 &= ~(1 << PIN_DA4);
  (a & 32)  ? portDA5 |= 1 << PIN_DA5 : portDA5 &= ~(1 << PIN_DA5);
  (a & 64)  ? portDA6 |= 1 << PIN_DA6 : portDA6 &= ~(1 << PIN_DA6);
  (a & 128) ? portDA7 |= 1 << PIN_DA7 : portDA7 &= ~(1 << PIN_DA7);
  (a & 256) ? portDA8 |= 1 << PIN_DA8 : portDA8 &= ~(1 << PIN_DA8); 
}

void writeAddress_Dram(uint16_t r, uint16_t c, uint8_t v) {
  setBus_Dram(r);
  portRAS &= ~(1 << PIN_RAS);
  portWE &= ~(1 << PIN_WE);
  (v & 1) ? portDI |= 1 << PIN_DI : portDI &= ~(1 << PIN_DI);
  setBus_Dram(c);
  portCAS &= ~(1 << PIN_CAS);
  __asm__("nop\n\t"); //62.5ns
  __asm__("nop\n\t");
  __asm__("nop\n\t");
  __asm__("nop\n\t");
  portWE |= 1 << PIN_WE;
  portCAS |= 1 << PIN_CAS;
  portRAS |= 1 << PIN_RAS;  
}

uint8_t readAddress_Dram(uint16_t r, uint16_t c) {
  uint8_t ret = 0;
  setBus_Dram(r);
  portRAS &= ~(1 << PIN_RAS);
  __asm__("nop\n\t"); //62.5ns
  setBus_Dram(c);
  portCAS &= ~(1 << PIN_CAS);
  __asm__("nop\n\t");
  __asm__("nop\n\t");
  __asm__("nop\n\t");
  __asm__("nop\n\t");
  ret = pinDO & (1 << PIN_DO);
  portCAS |= 1 << PIN_CAS;
  portRAS |= 1 << PIN_RAS; 
  return ret >>= PIN_DO;
}

//Setup pins for test logic
void logic_mode(void) {
  for(uint8_t i = 0; i < 15; i++) {
    digitalWrite(Pin16[i], LOW);
    pinMode(Pin16[i], OUTPUT);
  }
  digitalWrite(E_P16, HIGH);
  pinMode(E_P16, OUTPUT);
}

//Testing logic IC
boolean test_logic(void) {  
  boolean result = HIGH;
  for(uint8_t i = 0; i < Pin_max; i++) {
    if(Signal[i] == 'V') {
      if(i == 15) {
        digitalWrite(PinOut[i], LOW);
      } else {
        digitalWrite(PinOut[i], HIGH);
      }  
      pinMode(PinOut[i], OUTPUT);       
    } else
    if(Signal[i] == 'L') {
      pinMode(PinOut[i],INPUT_PULLUP);
    } else
    if(Signal[i] == 'H') {
      pinMode(PinOut[i],INPUT_PULLUP);
    }
  }
  delay(5); 
  //Set Signals
  for(uint8_t i = 0; i < Pin_max; i++) {
    if(Signal[i] == '0') {
      if(i == 15) {
        digitalWrite(PinOut[i], HIGH);
      } else {
        digitalWrite(PinOut[i], LOW);
      }
      pinMode(PinOut[i],OUTPUT);
    } else
    if(Signal[i] == 'C') {
      digitalWrite(PinOut[i], LOW); 
      pinMode(PinOut[i],OUTPUT);
    } else
    if(Signal[i] == '1') {
      if(i == 15) {
        digitalWrite(PinOut[i], LOW);
      } else {
        digitalWrite(PinOut[i], HIGH);
      } 
      pinMode(PinOut[i],OUTPUT);
    }
  }
  //Trigger Clock
  for(uint8_t i = 0; i < Pin_max; i++) {
    if(Signal[i] == 'C') {
      digitalWrite(PinOut[i], HIGH);  
    }
  }
  delay(10); 
  for(uint8_t i = 0; i < Pin_max; i++) {               
    if(Signal[i] == 'C') {
      digitalWrite(PinOut[i], LOW);
    }
  }
  delay(5);
  //Read Outputs
  for(uint8_t i = 0; i < Pin_max; i++) {
    if((Signal[i] == 'H')&&(digitalRead(PinOut[i]) == LOW)) {
      result = LOW; 
      if(single_test == 1) {test_low[i] = 0x4C;/*L*/}      
    } else
    if((Signal[i] == 'L')&&(digitalRead(PinOut[i]) == HIGH)) {
      result = LOW;      
      if(single_test == 1) {test_low[i] = 0x48;/*H*/}
    } else {      
      if(single_test == 1) {test_low[i] = Signal[i];}
    }
  }
  return result;
}

float get_voltage(void) {
  float vADC = (analogRead(voltageControl) / 1024.) * vRef;
  float current = vADC / rBottom;
  return (current * (rTop + rBottom));
}
