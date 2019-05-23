/*
    @타이틀 : WS2812B LED 제어
    @파일명 : segWS2812B.ino
*/


#include <Adafruit_NeoPixel.h>

#define PIN            6
#define NUMPIXELS      21

#define AIR_GOOD            0    // BLUE
#define AIR_MODERATE        1    // GREEN
#define AIR_UNHEALTHY       2    // ORANGE
#define AIR_VERY_UNHEALTHY  3    // RED

#define BRIGHTNESS 50

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


int delayval = 500;

/*
 *     첫전째    두번째     세번째 LED
 *       15         08         01
 *     16  14     09  07     02  00  
 *       17         10         03
 *     20   18    13  11     06  04
 *       19         12         05 
 */
int SEGMENT[3][10][8] = {
  {
    // 첫번째 세그먼트    
    {6, 14, 15, 16, 18, 19, 20},      // LED가 켜져야 하는 개수 (총 6개) + 0
    {2, 14, 18},                      // 1
    {5, 14, 15, 17, 19, 20},          // 2
    {5, 14, 15, 17, 18, 19},          // 3
    {4, 14, 16, 17, 18},              // 4
    {5, 15, 16, 17, 18, 19},          // 5
    {6, 15, 16, 17, 18, 19, 20},      // 6
    {4, 14, 15, 16, 18},              // 7
    {7, 14, 15, 16, 17, 18, 19, 20},  // 8
    {6, 14, 15, 16, 17, 18, 19}       // 9
  },
  {
    // 두번째 세그먼트
    {6, 7, 8, 9, 11, 12, 13},
    {2, 7, 11},
    {5, 7, 8, 10, 12, 13},
    {5, 7, 8, 10, 11, 12},
    {4, 7, 9, 10, 11},
    {5, 8, 9, 10, 11, 12},
    {6, 8, 9, 10, 11, 12, 13},
    {4, 7, 8, 9, 11},
    {7, 7, 8, 9, 10, 11, 12, 13},
    {6, 7, 8, 9, 10, 11, 12}
  },
  {
    // 세번째 세그먼트
    {6, 0, 1, 2, 4, 5, 6},
    {2, 0, 4},
    {5, 0, 1, 3, 5, 6},
    {5, 0, 1, 3, 4, 5},
    {4, 0, 2, 3, 4},
    {5, 1, 2, 3, 4, 5},
    {6, 1, 2, 3, 4, 5, 6},
    {4, 0, 1, 2, 4},
    {7, 0, 1, 2, 3, 4, 5, 6},
    {6, 0, 1, 2, 3, 4, 5}
  }
};


void setupWS2812B() {
  pixels.begin();
  pixels.show();
}


void loopWS2812B() {

}

/**
   @함수명    : clearDigital
   @설명      : WS2812B LED OFF
*/
void clearDigital(int start, int pos) {
  for (int i = start; i <= pos; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    pixels.show();
  }
}



/**
   @함수명    : setDigital
   @설명      : 표시될 세그먼트와 숫자를 설정하면 LED ON (1자리 숫자)
   @매개변수 :  digital(세그먼트 위치) , num (표시될 미세먼지 숫자)
                PM25 (1) (2) (3) 
                     첫번째, 두번째, 세번째 자리                
*/
void setDigital(int digital, int num) {
  int pos = digital - 1;
  int cnt = SEGMENT[pos][num][0];
  uint32_t color = setColor(avrPM2_5);

  for (int i = 1; i <= cnt; i++) {
    pixels.setPixelColor(SEGMENT[pos][num][i], color);
    pixels.show();
  }
}


/**
   @함수명    : setColor
   @설명      : 초미세먼지값에 따라 4가지 컬러값을 표시
*/
uint32_t setColor(int pm) {

  if (pm <= 15) {
    return pixels.Color(0, 0, 255);
  } else if (pm <= 35) {
    return pixels.Color(0, 255, 0);
  } else if (pm <= 75) {
    return pixels.Color(253, 155, 90);
  } else {
    return pixels.Color(255, 0, 0);
  }
}



int tmp_d1 = -1, tmp_d2 = -1, tmp_d3 = -1;

/**
   @함수명    : displayDigital
   @설명      : 3자리 LED를 표시
*/
void displayDigital(int d1, int d2, int d3) {
  if (d3 != tmp_d3) {
    clearDigital(0, 6);
    tmp_d3 = d3;
  }
  setDigital(3, d3);

  if (d2 != tmp_d2) {
    clearDigital(7, 13);
    tmp_d2 = d2;
  }
  setDigital(2, d2);

  if (d1 != tmp_d1) {
    clearDigital(14, 20);
    tmp_d1 = d1;
  }
  setDigital(1, d1);
}


/**
   @함수명    : setBrightness
   @설명      : WS2812B LED 의 밝기 조정
*/
void setBrightness() {
  pixels.setBrightness(BRIGHTNESS);
}

