#include <EEPROM.h>

void setup() {
  delay(1000);
  Serial.begin(115200);

  EEPROM.begin(256);
  EEPROM.write(0, 0);
  EEPROM.commit();

  if (EEPROM.read(0) == 0) {
    Serial.print("Modo cambiado!!!");
  } else {
    Serial.print("Error al cambiar el modo");
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
