#include <List.hpp>
#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
#if !defined(CONFIG_BT_SPP_ENABLED)
  #error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

class object {
    public:
      float angle;
      float distance;
      object() {};
      object(float _angle, float _distance);
};

object::object(float _angle, float _distance){
  angle = _angle;
  distance = _distance;
}

String device_name = "ESP32-BT-Slave";
BluetoothSerial SerialBT;
unsigned long previousMillis = 0;
List<object> objetosAlrededor;

String radSender(float angulo, float distancia) {
  String baseString = "";
  for(int i = 0; i < objetosAlrededor.getSize(); i++){
    object inList = objetosAlrededor.get(i);
    if(inList.angle == angulo){
      objetosAlrededor.remove(i);
      if(distancia < 1 || distancia > 180){
          baseString = "RD=";
      }
      else{
        baseString = "RU=";
        objetosAlrededor.add(object(angulo, distancia));
      }
      return (baseString + String(angulo) + "," + String(distancia));
    }
  }
  baseString = "RA=";
  objetosAlrededor.add(object(angulo, distancia));
  return (baseString + String(angulo) + "," + String(distancia));
}

String TSSSender(float val){
  return ("TSS=" + String(val));
}

String TDSSender(float val){
  return ("TDS=" + String(val));
}

String PHSender(float val){
  return ("PH=" + String(val));
}

String TMPSender(float val){
  return ("TMP=" + String(val));
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin(device_name); //Bluetooth device name
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
}

float randomFloat(int min, int max){
  float a = float(random(min, max));
  float b = float(random(0, 10)) / 10.0;
  float c = float(random(0, 10)) / 100.0;
  return float(a + b + c);
}

void loop() {
  unsigned long currentMillis = millis();
  //Codigo envío
  if(currentMillis - previousMillis > 5000){
    previousMillis = currentMillis;
    String env = "";
    int opSelector = random(0, 5);
    switch (opSelector) {
      case 0: {
        env = radSender(float(random(0, 360)), randomFloat(0, 200));
        break;
      }
      case 1: {
        env = TSSSender(randomFloat(0, 1024));
        break;
      }
      case 2: {
        env = TDSSender(randomFloat(0, 1024));
        break;
      }
      case 3: {
        env = PHSender(randomFloat(0, 14));
        break;
      }
      case 4: {
        env = TMPSender(randomFloat(0, 25));
        break;
      }
    }
    if(env != ""){
      for(char a : env){
        SerialBT.write(a);
      }
      SerialBT.write(10);
    }
  }
  //Codigo Recepción
  if(SerialBT.available()){
      Serial.write(SerialBT.read());
  }
}
