#include <Arduino.h>
#include <SoftwareSerial.h>
#define sensor_baud_rate 9600 //sensor only works with baud rate of 9600 
#define ESP_baud_rate 115200 //separate baud rate for ESP32 can be changed
#define LED1 2 //InBuilt led pin of ESP32

SoftwareSerial SO2_serial (18,19); //Establising software 

byte SO2_qa_mode[] = {0xFF, 0x01, 0x78, 0x04, 0x00, 0x00, 0x00, 0x00, 0x83}; //Command to start O2 sensor in QnA mode
byte SO2_read[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79}; //command to read O2 concentration under QnA mode
byte SO2_received_bytes[9]; //Array to store sensor response

float read_SO2(float *gas_con); //function to read o2 concentration 
float SO2; //variable to store O2 value

void setup() {
  Serial.begin(ESP_baud_rate); //Initialise serial monitor from ESP32
  SO2_serial.begin(sensor_baud_rate);  //Initialise Uart communication with O2 sensor
  pinMode(LED1, OUTPUT); //declaring esp32 inbuilt led pin as output

  delay(1000);

  SO2_serial.write(SO2_qa_mode, sizeof(SO2_qa_mode));
  delay(250);
  SO2_serial.readBytes(SO2_received_bytes, sizeof(SO2_received_bytes));
  
  
  //prints the received bytes in serial monitor
  for (int i = 0; i <= 8; i++)
  {
    Serial.print(SO2_received_bytes[i], HEX); Serial.print(" ");
  }
  Serial.println();

  //sensor booting section
  Serial.println("Sensor is booting Please wait");
  int bootUP_time = 0;
  while(bootUP_time != 10)
  {
    Serial.print(".");
    delay(1000);
    bootUP_time += 1;
  }
  Serial.println();

}

void loop() {

  delay(1840);
  
  read_SO2(&SO2); //passing O2 variable address to write the o2 gas concentration value in it 

  Serial.printf("SO2 : %.2f \n",SO2); //prints O2 concentration in a serial monitor

  /*//code is just to print loading dots
  for(int i = 0;i<=10;i++)
  {
    Serial.print(".");
    delay(10);
  }
  Serial.println("");
  //End of loading dots print function */

  //simply just blinks Inbuilt LED in ESp32
  digitalWrite(LED1,HIGH);
  delay(150);
  digitalWrite(LED1,LOW);

}

float read_SO2(float *gas_con)
{
   if(SO2_serial.write(SO2_read,sizeof(SO2_read)) == 9) //send O2 read command to o2 sensor and checks whether it id fully sent
   {
    delay(250);
    SO2_serial.readBytes(SO2_received_bytes, sizeof(SO2_received_bytes));
    *gas_con = SO2_received_bytes[2] * 256 + SO2_received_bytes[3]; //Substituting sensor responses to the gas concentration formula
    *gas_con = *gas_con * 0.1;
   } 

   /* //debug code -> to print received bytes into serial monitor 
   for(byte j = 0;j<9;j++)
   {
     Serial.print(SO2_received_bytes[j], HEX); //Prints received byte from O2 sensor response
     Serial.print(" ");
   }
  Serial.println(); */
  return *gas_con; //returns the gas concentration of O2
}