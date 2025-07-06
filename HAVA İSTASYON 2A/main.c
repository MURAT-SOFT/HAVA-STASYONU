// HAVA ÝSTASYONU HG-2 A PIC16F877A  DS18B20  sensor
// C Code for CCS C compiler.

//LCD module connections
#define LCD_RS_PIN      PIN_D0
#define LCD_RW_PIN      PIN_D1
#define LCD_ENABLE_PIN  PIN_D2
#define LCD_DATA4       PIN_D3
#define LCD_DATA5       PIN_D4
#define LCD_DATA6       PIN_D5
#define LCD_DATA7       PIN_D6
//End LCD module connections

#include <16F877A.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP                       
#use delay(clock = 8MHz)
#include <lcd.c>
#define DS18B20_PIN PIN_B1                         // DS18B20 Data pin is connected to pin RB1

signed int16 raw_temp;
float temp;

int1 ds18b20_start(){
  output_low(DS18B20_PIN);                         // Send reset pulse to the DS18B20 sensor
  output_drive(DS18B20_PIN);                       // Configure DS18B20_PIN pin as output
  delay_us(500);                                   // Wait 500 us
  output_float(DS18B20_PIN);                       // Configure DS18B20_PIN pin as input
  delay_us(100);                                   //wait to read the DS18B20 sensor response
  if (!input(DS18B20_PIN)) {
    delay_us(400);                                 // Wait 400 us
    return TRUE;                                   // DS18B20 sensor is present
  }
  return FALSE;
}

void ds18b20_write_bit(int1 value){
  output_low(DS18B20_PIN);
  output_drive(DS18B20_PIN);                       // Configure DS18B20_PIN pin as output
  delay_us(2);                                     // Wait 2 us
  output_bit(DS18B20_PIN, value);
  delay_us(80);                                    // Wait 80 us
  output_float(DS18B20_PIN);                       // Configure DS18B20_PIN pin as input
  delay_us(2);                                     // Wait 2 us
}

void ds18b20_write_byte(int8 value){
  int8 i;
  for(i = 0; i < 8; i++)
    ds18b20_write_bit(bit_test(value, i));
}

int1 ds18b20_read_bit(void) {
  int1 value;
  output_low(DS18B20_PIN);
  output_drive(DS18B20_PIN);                       // Configure DS18B20_PIN pin as output
  delay_us(2);
  output_float(DS18B20_PIN);                       // Configure DS18B20_PIN pin as input
  delay_us(5);                                     // Wait 5 us
  value = input(DS18B20_PIN);
  delay_us(100);                                   // Wait 100 us
  return value;
}

int8 ds18b20_read_byte(void) {
  int8 i, value = 0;
  for(i = 0; i  < 8; i++)
    shift_right(&value, 1, ds18b20_read_bit());
  return value;
}

int1 ds18b20_read(int16 *raw_temp_value) {
  if (!ds18b20_start())                     // Send start pulse
    return FALSE;
  ds18b20_write_byte(0xCC);                 // Send skip ROM command
  ds18b20_write_byte(0x44);                 // Send start conversion command
  while(ds18b20_read_byte() == 0);          // Wait for conversion complete
  if (!ds18b20_start())                     // Send start pulse
    return FALSE;                           // Return 0 if error
  ds18b20_write_byte(0xCC);                 // Send skip ROM command
  ds18b20_write_byte(0xBE);                 // Send read command
  *raw_temp_value = ds18b20_read_byte();    // Read temperature LSB byte and store it on raw_temp_value LSB byte
  *raw_temp_value |= (int16)(ds18b20_read_byte()) << 8;     // Read temperature MSB byte and store it on raw_temp_value MSB byte
  return TRUE;                                // OK --> return 1
}

void main() {
  lcd_init();                                    // Initialize LCD module
  lcd_putc('\f');                                // Clear LCD
  lcd_gotoxy(3, 1);                              // Go to column 3 row 1
  printf(lcd_putc, "Temperature:");
  while(TRUE) {
    if(ds18b20_read(&raw_temp)) {
      temp = (float)raw_temp / 16;               // Convert temperature raw value into degree Celsius (temp in °C = raw/16)
      lcd_gotoxy(5, 2);                          // Go to column 5 row 2
      printf(lcd_putc, "%f", temp);
      lcd_putc(223);                             // Print degree symbol ( ° )
      lcd_putc("C ");                            // Print 'C '
    }
    else {
      lcd_gotoxy(5, 2);                          // Go to column 5 row 2
      printf(lcd_putc, " Error! ");
    }
    delay_ms(1000);                              // Wait 1 second
  }
}
// End of code.
