#include<VEML7700.h>
void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
}
VEML7700 myVEML(0x10);
void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Ambient:  " + String(myVEML.getAmbient()));
  Serial.println("White:  " + String(myVEML.getWhite()));
  Serial.println("#######################");
  delay(1000);
}
