/*
 * Segundo programa de seteo de horómetros, válido para grúas STS 05-08 y RTGs 03-06
 * A diferencia del otro, en este programa se deben ajustar las fracciones de hora,
 * usando regla de 3 simple, ejemplo:
 * El horómetro marca una 45,5 en los números rojos, por lo que se debe multiplicar
 * 45,5*3600000/100= Fracciones a utilizar.
 * o más simple: 45,5*36000.
 * Una vez calculadas todas las fracciones y con la grúa con el Control botado, esperar
 * 4 segundos y cargar el programa del horómetro.
 */
#include <EEPROM.h>
                 // C_ON      BOOM      HOIST   GANTRY    TROLLEY
long time_a[5] = { 684000,28800, 0, 0, 28800 }; //tiempo acumulado 0 1 2 3 4
//calcular con regla de 3 simple con horómetros en la grúa
long boom = 2441;//2268000
long hoist = 28855;//216000
long control_on = 17833;//1573200
long gantry = 2182;//2516400
long trolley = 25327;//432000
long aux1, aux2, aux3, aux4;


void setup() {
  Serial.begin(9600);
  delay(2000);

  EEPROM.write(10, control_on);
  EEPROM.write(11, control_on >> 8);
  EEPROM.write(12, control_on >> 16);
  EEPROM.write(13, control_on >> 24);

  EEPROM.write(20, boom);
  EEPROM.write(21, boom >> 8);
  EEPROM.write(22, boom >> 16);
  EEPROM.write(23, boom >> 24);

  EEPROM.write(30, hoist);
  EEPROM.write(31, hoist >> 8);
  EEPROM.write(32, hoist >> 16);
  EEPROM.write(33, hoist >> 24);

  EEPROM.write(40, gantry);
  EEPROM.write(41, gantry >> 8);
  EEPROM.write(42, gantry >> 16);
  EEPROM.write(43, gantry >> 24);

  EEPROM.write(50, trolley);
  EEPROM.write(51, trolley >> 8);
  EEPROM.write(52, trolley >> 16);
  EEPROM.write(53, trolley >> 24);

  //Control_On
  EEPROM.write(60, time_a[0]);
  EEPROM.write(61, time_a[0] >> 8);
  EEPROM.write(62, time_a[0] >> 16);
  //Boom
  EEPROM.write(70, time_a[1]);
  EEPROM.write(71, time_a[1] >> 8);
  EEPROM.write(72, time_a[1] >> 16);
  //Hoist
  EEPROM.write(80, time_a[2]);
  EEPROM.write(81, time_a[2] >> 8);
  EEPROM.write(82, time_a[2] >> 16);
  //Gantry
  EEPROM.write(90, time_a[3]);
  EEPROM.write(91, time_a[3] >> 8);
  EEPROM.write(92, time_a[3] >> 16);
  //Trolley
  EEPROM.write(100, time_a[4]);
  EEPROM.write(101, time_a[4] >> 8);
  EEPROM.write(102, time_a[4] >> 16);

  aux1 = EEPROM.read(10); aux2 = EEPROM.read(11); aux3 = EEPROM.read(12); aux4 = EEPROM.read(13);
  Serial.print("CONTROL ON = ");
  Serial.print(aux4 << 24 | aux3 << 16 | aux2 << 8 | aux1);
  Serial.print(" Horas + ");
  aux3 = EEPROM.read(62); aux2 = EEPROM.read(61); aux1 = EEPROM.read(60);
  Serial.print(aux3 << 16 | aux2 << 8 | aux1);
  Serial.println(" Fragmentos.");

  aux1 = EEPROM.read(20); aux2 = EEPROM.read(21); aux3 = EEPROM.read(22); aux4 = EEPROM.read(23);
  Serial.print("BOOM = ");
  Serial.print(aux4 << 24 | aux3 << 16 | aux2 << 8 | aux1);
  Serial.print(" Horas + ");
  aux3 = EEPROM.read(72); aux2 = EEPROM.read(71); aux1 = EEPROM.read(70);
  Serial.print(aux3 << 16 | aux2 << 8 | aux1);
  Serial.println(" Fragmentos.");

  aux1 = EEPROM.read(30); aux2 = EEPROM.read(31); aux3 = EEPROM.read(32); aux4 = EEPROM.read(33);
  Serial.print("HOIST = ");
  Serial.print(aux4 << 24 | aux3 << 16 | aux2 << 8 | aux1);
  Serial.print(" Horas + ");
  aux3 = EEPROM.read(82); aux2 = EEPROM.read(81); aux1 = EEPROM.read(80);
  Serial.print( aux3 << 16 | aux2 << 8 | aux1);
  Serial.println(" Fragmentos.");

  aux1 = EEPROM.read(40); aux2 = EEPROM.read(41); aux3 = EEPROM.read(42); aux4 = EEPROM.read(43);
  Serial.print("GANTRY = ");
  Serial.print(aux4 << 24 | aux3 << 16 | aux2 << 8 | aux1);
  Serial.print(" Horas + ");
  aux3 = EEPROM.read(92); aux2 = EEPROM.read(91); aux1 = EEPROM.read(90);
  Serial.print( aux3 << 16 | aux2 << 8 | aux1);
  Serial.println(" Fragmentos.");

  aux1 = EEPROM.read(50); aux2 = EEPROM.read(51); aux3 = EEPROM.read(52); aux4 = EEPROM.read(53);
  Serial.print("TROLLEY = ");
  Serial.print(aux4 << 24 | aux3 << 16 | aux2 << 8 | aux1);
  Serial.print(" Horas + ");
  aux3 = EEPROM.read(102); aux2 = EEPROM.read(101); aux1 = EEPROM.read(100);
  Serial.print( aux3 << 16 | aux2 << 8 | aux1);
  Serial.println(" Fragmentos.");
}

void loop() {
  // put your main code here, to run repeatedly:

}
