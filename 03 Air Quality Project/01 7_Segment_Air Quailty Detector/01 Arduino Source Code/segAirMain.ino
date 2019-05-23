/* 
 * @타이틀 : PMS7003 미세먼지 센서 젠어
 * @파일명 : segPMS7003.ino   
 * @작성일 : 2019. 04. 18
 */
 

unsigned int avrPM2_5 = 0, avrPM10_0 = 0;


void setup() {   

  setupWS2812B();
  setupSensor();
}


void loop() {  
//  loopWS2812B();
  loopSensor();
}
 
