/* 
 * @타이틀 : PMS7003 미세먼지 센서 젠어
 * @파일명 : segPMS7003.ino  
 */

#include <SoftwareSerial.h>

#define  COUNT_NUM  5

int pmsTxPin = 3;   
int pmsRxPin = 2;

SoftwareSerial PMS(pmsRxPin, pmsTxPin);

int incomingByte = 0;

const int MAX_FRAME_LEN = 64;
char frameBuf[MAX_FRAME_LEN];
int detectOff = 0;
int frameLen = MAX_FRAME_LEN;
bool inFrame = false;
char printbuf[256];

unsigned int calcChecksum = 0;
unsigned int pm2_5 = 0, pm10_0 = 0;
byte cnt = 0;
int d1, d2, d3;  


/**
 * @설명      : 측정된 센서의 값을 저장하는 구조체 
*/
struct PMS7003_framestruct {
  byte  frameHeader[2];
  unsigned int  frameLen = MAX_FRAME_LEN;
  unsigned int  concPM2_5_CF1;
  unsigned int  concPM10_0_CF1;
  unsigned int  checksum;
} thisFrame;


void setupSensor() {
  Serial.begin(57600);
  delay(1000);
  Serial.println("PMS7003 Initializing...");
}

void loopSensor () {

  getSensorValue();

  if (cnt == COUNT_NUM) { // cnt 회수 만큼 센서의 값을 측정
    getSensorResult();
    parserDigital();
    displaySensor();
    pm2_5 = pm10_0 = cnt = 0;
  }
}


/**
 * @함수명    : getSensorValue
 * @설명      : 센서의 값을 cnt 회수 만큼 반복해서 읽어온다. 
*/
void getSensorValue() {
  if (pms7003_read()) {

    pm2_5 += thisFrame.concPM2_5_CF1;
    pm10_0 += thisFrame.concPM10_0_CF1;
    cnt++;

    Serial.print(cnt);
  } else {
    Serial.print("*");
  }
}

/**
 * @함수명    : pms7003_read
 * @설명      : 센서의 값을 읽어온다. 
 * @리턴값    : bool  
*/
bool pms7003_read() {
  PMS.begin(9600);
  bool packetReceived = false;
  calcChecksum = 0;

  while (!packetReceived) {
    if (PMS.available() > 32) {
      int drain = PMS.available();

      for (int i = drain; i > 0; i--) {
        PMS.read();
      }
    }

    if (PMS.available() > 0) {
      incomingByte = PMS.read();

      if (!inFrame) {
        if (incomingByte == 0x42 && detectOff == 0) {
          frameBuf[detectOff] = incomingByte;
          thisFrame.frameHeader[0] = incomingByte;
          calcChecksum = incomingByte; // Checksum init!
          detectOff++;
        } else if (incomingByte == 0x4D && detectOff == 1) {
          frameBuf[detectOff] = incomingByte;
          thisFrame.frameHeader[1] = incomingByte;
          calcChecksum += incomingByte;
          inFrame = true;
          detectOff++;
        } else {
          Serial.print("----- Frame syncing... -----");
          Serial.print(incomingByte, HEX);
          Serial.println();
        }
      } else {
        frameBuf[detectOff] = incomingByte;
        calcChecksum += incomingByte;
        detectOff++;
        unsigned int  val = (frameBuf[detectOff - 1] & 0xff) + (frameBuf[detectOff - 2] << 8);

        switch (detectOff) {
          case 4:
            thisFrame.frameLen = val;
            frameLen = val + detectOff;
            break;

          case 8:
            thisFrame.concPM2_5_CF1 = val;
            break;
          case 10:
            thisFrame.concPM10_0_CF1 = val;
            break;

          case 32:
            thisFrame.checksum = val;
            calcChecksum -= ((val >> 8) + (val & 0xFF));
            break;
          default:
            break;
        }

        if (detectOff >= frameLen) {
          packetReceived = true;
          detectOff = 0;
          inFrame = false;
        }
      }
    }
  }

  PMS.end();
  return (calcChecksum == thisFrame.checksum);
}


/**
 * @함수명    : getSensorResult
 * @설명      : 평균 센서값을 얻어온다 
*/
void getSensorResult() {
  avrPM2_5 = pm2_5  / COUNT_NUM;
  avrPM10_0 = pm10_0 / COUNT_NUM ;
}


/**
 * @함수명    : parserDigital
 * @설명      : LED에 표시하기 위해 센서값을 파싱한다.
*/
void parserDigital() {
  int k = avrPM2_5;
  if (k > 0) {
    d3 = k % 10;
    k = k / 10;
    d2 = k % 10;
    k = k / 10;
    d1 = k % 10;
  }
}


/**
 * @함수명    : displaySensor
 * @설명      : displayDigital()을 통해 센서의 숫자값에 따라 LED를 표시한다. 
*/
void displaySensor() {
  sprintf(printbuf, "PM2.5 = %d, PM10 = %d", avrPM2_5, avrPM10_0);

  displayDigital(d1, d2, d3);   // WS2812B LED에 매핑하여 숫자를 표시

  Serial.println();
  Serial.println("*************");
  Serial.println(printbuf);
  Serial.print(d1);
  Serial.print(d2);
  Serial.println(d3);


  //  delay(20000);
}

