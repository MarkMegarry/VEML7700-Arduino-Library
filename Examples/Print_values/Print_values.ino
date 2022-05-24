#include<VEML7700.h>
void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
}
VEML7700 myVEML(0x10);
void loop() {
  // put your main code here, to run repeatedly:
  double ambient = myVEML.getAmbient();
  double white = myVEML.getWhite();
  if(ambient == -1 || white == -1){ //Error condition
    yourErrorHandling();
  }
  else{
    Serial.println("Ambient:  " + String(ambient));
    Serial.println("White:  " + String(white));
    Serial.println("#######################");
    delay(1000);
  }
}

void yourErrorHandling(){
  //Your error handling code here
}
