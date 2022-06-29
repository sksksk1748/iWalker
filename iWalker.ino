#include "I2Cdev.h"
#include "MPU6050.h"
#include <SoftwareSerial.h>
//#include <MFRC522.h>     // 引用程式庫
#include <LiquidCrystal_I2C.h>
//#include <Wire.h>  

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

SoftwareSerial I2CBT(10,11); //定義PIN10及PIN11分別為RX及TX腳位
MPU6050 accelgyro;

int16_t ax, ay, az; //加速度
int16_t gx, gy, gz; //角速度

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
#define RST_PIN      A0  // 讀卡機的重置腳位
#define SS_PIN       10  // 晶片選擇腳位

#define OUTPUT_READABLE_ACCELGYRO

#define LED_PIN 13
bool blinkState = false;
int num=0;
int LED = 2;
/*
void LCDdisplay(bool flag) {
    if(flag){
      lcd.print("Open door");
    } else{
      lcd.print("Close door");
    }

    delay(10);
}*/


void setup() {

    lcd.begin(16, 2);
    lcd.backlight();

    I2CBT.begin(9600);                     //BT boud rate (有的可能會不一定，要注意一下)
    //Serial.begin(38400);                       //Arduino起始鮑率：9600 
    pinMode(LED, OUTPUT);    
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    
    Serial.begin(38400);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    // configure Arduino LED pin for output
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    // read raw accel/gyro measurements from device
    lcd.setCursor(0, 0); // 設定游標位置在第一行行首
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    #ifdef OUTPUT_READABLE_ACCELGYRO
        // display tab-separated accel/gyro x/y/z values
        /*Serial.print("a/g:\t");
        Serial.print("ax ");Serial.print(ax); Serial.print("\t");
        Serial.print("ay ");Serial.print(ay); Serial.print("\t");
        Serial.print("az ");Serial.print(az); Serial.print("\t");
        Serial.print("gx ");Serial.print(gx); Serial.print("\t");
        Serial.print("gy ");Serial.print(gy); Serial.print("\t");
        Serial.print("gz ");Serial.println(gz);*/
        if((ax >= -20000 && ax <=2000)&&(ay >= -10000 && ay <=3000)&&(az >= 9000 && az <=20000)&&(gx >= -20000 && gx <= 20000)&&(gy >= -6000 && gy <= 10000)&&(gz >= -23000 && gx <= 28000)){
            num++;
            Serial.print(num);
            lcd.print("       ");lcd.print(num);
            //lcd.clear(); //顯示清除
        }
        
    #endif

        // 檢查是否有資料可供讀取
    if (I2CBT.available() > 0) {
    // 讀取進來的 byte
    int inByte = I2CBT.read();
    // 根據收到的字元決定要打開或關掉 LED
    Serial.println(inByte);
    switch (inByte) {
    case 97: //98
      digitalWrite(LED, HIGH);
      Serial.print("a");
      delay(15);
      break;
    case 98: //97
       digitalWrite(LED, LOW);
       Serial.print("b");
       delay(15);
      break;
    case 99:// c
        char hum_buf[10], tem_buf[10];
        if (num !=0 && num >=10) {
            dtostrf(num, 3, 0, hum_buf);
            for(int i=0; i<=1; i++) {
              I2CBT.write(hum_buf[i]);
              I2CBT.write(hum_buf[i]);
            }
        }
        delay(15);
        break;
    }
  }

    #ifdef OUTPUT_BINARY_ACCELGYRO
        Serial.write((uint8_t)(ax >> 8)); Serial.write((uint8_t)(ax & 0xFF));
        Serial.write((uint8_t)(ay >> 8)); Serial.write((uint8_t)(ay & 0xFF));
        Serial.write((uint8_t)(az >> 8)); Serial.write((uint8_t)(az & 0xFF));
        Serial.write((uint8_t)(gx >> 8)); Serial.write((uint8_t)(gx & 0xFF));
        Serial.write((uint8_t)(gy >> 8)); Serial.write((uint8_t)(gy & 0xFF));
        Serial.write((uint8_t)(gz >> 8)); Serial.write((uint8_t)(gz & 0xFF));
    #endif

    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
    delay(150);
}
