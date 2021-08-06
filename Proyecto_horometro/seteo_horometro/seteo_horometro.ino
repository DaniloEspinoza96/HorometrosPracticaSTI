/*
 * Programa para ajustar los tiempos de los horómetros para
 * su puesta en marcha, válido para STS 01-04 y RTGs 07-08.
 * Sólo se debe escribir la hora de los 
 * horómetros presentes en la grúa y cargar en el Arduino, 
 * esperar 4 segundos una vez subido,
 * luego cargar el programa del horómetro y listo.
 */

#include <EEPROM.h>

long boom=2559;
long hoist=34846;
long control_on=59134;
long gantry=2800;
long trolley=31353;
long aux1,aux2,aux3,aux4;

void setup() {
  Serial.begin(9600);
  delay(2000);
  
  EEPROM.write(10, control_on);
  EEPROM.write(11, control_on>>8);
  EEPROM.write(12, control_on>>16);
  EEPROM.write(13, control_on>>24);

  EEPROM.write(20, boom);
  EEPROM.write(21, boom>>8);
  EEPROM.write(22, boom>>16);
  EEPROM.write(23, boom>>24);

  EEPROM.write(30, hoist);
  EEPROM.write(31, hoist>>8);
  EEPROM.write(32, hoist>>16);
  EEPROM.write(33, hoist>>24);

  EEPROM.write(40, gantry);
  EEPROM.write(41, gantry>>8);
  EEPROM.write(42, gantry>>16);
  EEPROM.write(43, gantry>>24);

  EEPROM.write(50, trolley);
  EEPROM.write(51, trolley>>8);
  EEPROM.write(52, trolley>>16);
  EEPROM.write(53, trolley>>24);

  aux1=EEPROM.read(10);
  aux2=EEPROM.read(11);
  aux3=EEPROM.read(12);
  aux4=EEPROM.read(13);
  Serial.print("CONTROL ON = ");
  Serial.print(aux4<<24|aux3<<16|aux2<<8|aux1);
  Serial.println();
  aux1=EEPROM.read(20);
  aux2=EEPROM.read(21);
  aux3=EEPROM.read(22);
  aux4=EEPROM.read(23);
  Serial.print("BOOM = ");
  Serial.print(aux4<<24|aux3<<16|aux2<<8|aux1);
  Serial.println();
  aux1=EEPROM.read(30);
  aux2=EEPROM.read(31);
  aux3=EEPROM.read(32);
  aux4=EEPROM.read(33);
  Serial.print("HOIST = ");
  Serial.print(aux4<<24|aux3<<16|aux2<<8|aux1);
  Serial.println();
  aux1=EEPROM.read(40);
  aux2=EEPROM.read(41);
  aux3=EEPROM.read(42);
  aux4=EEPROM.read(43);
  Serial.print("GANTRY = ");
  Serial.print(aux4<<24|aux3<<16|aux2<<8|aux1);
  Serial.println();
  aux1=EEPROM.read(50);
  aux2=EEPROM.read(51);
  aux3=EEPROM.read(52);
  aux4=EEPROM.read(53);
  Serial.print("TROLLEY = ");
  Serial.print(aux4<<24|aux3<<16|aux2<<8|aux1);
  Serial.println();
 
}

void loop() {
}
