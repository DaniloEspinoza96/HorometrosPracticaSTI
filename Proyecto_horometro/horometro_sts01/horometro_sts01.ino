/*
 * Programa horómetro para STS 01.
 */
#include <OPC.h>
#include <Bridge.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Ethernet.h>

const int c_on  = 22;       //Pines asociados a los movimientos de la grúa
const int boom = 24;
const int hoist  = 26;
const int gantry  = 28;
const int trolley  = 30;
long  horometro[5], aux1, aux2, aux3, aux4; //Variables donde se almacenarán las horas
                                            //que lleva funcionando la grúa.
unsigned long tiempo[5];    //Arreglo de variables que se utilizarán en el antirrebote (debounce).

//Configuración de shield ethernet
//NOTA: se debe ajustar una MAC y una IP adecuada para cada grúa.
byte mac[]  = { 0x90, 0xA2, 0xDA, 0x0F, 0x72, 0xF4}; //Configuración de MAC
IPAddress ip(192, 168, 129, 111); //configuración de IP
EthernetServer server(80);

//Declarar objeto OPC
OPCEthernet aOPCEthernet;

// Puerto para OPC
const int listen_port = 81;

//Elementos a enviar al OPC
float callback_c_on(const char *itemID, const opcOperation opcOP, const float value) {
  return horometro[0];
}
float callback_boom(const char *itemID, const opcOperation opcOP, const float value) {
  return horometro[1];
}
float callback_hoist(const char *itemID, const opcOperation opcOP, const float value) {
  return horometro[2];
}
float callback_gantry(const char *itemID, const opcOperation opcOP, const float value) {
  return horometro[3];
}
float callback_trolley(const char *itemID, const opcOperation opcOP, const float value) {
  return horometro[4];
}

void  setup() {
  delay(2000);  //Pausa para inicializar el shield ethernet.
  for (int i = 0; i <= 4; i++) {
    tiempo[i] = 0;
  }
  
  //Consulta de horas en la memoria del arduino.

  aux4 = EEPROM.read(13); aux3 = EEPROM.read(12); aux2 = EEPROM.read(11); aux1 = EEPROM.read(10);
  horometro[0] = aux4 << 24 | aux3 << 16 | aux2 << 8 | aux1;

  aux4 = EEPROM.read(23); aux3 = EEPROM.read(22); aux2 = EEPROM.read(21); aux1 = EEPROM.read(20);
  horometro[1] = aux4 << 24 | aux3 << 16 | aux2 << 8 | aux1;

  aux4 = EEPROM.read(33); aux3 = EEPROM.read(32); aux2 = EEPROM.read(31); aux1 = EEPROM.read(30);
  horometro[2] = aux4 << 24 | aux3 << 16 | aux2 << 8 | aux1;

  aux4 = EEPROM.read(43); aux3 = EEPROM.read(42); aux2 = EEPROM.read(41); aux1 = EEPROM.read(40);
  horometro[3] = aux4 << 24 | aux3 << 16 | aux2 << 8 | aux1;

  aux4 = EEPROM.read(53); aux3 = EEPROM.read(52); aux2 = EEPROM.read(51); aux1 = EEPROM.read(50);
  horometro[4] = aux4 << 24 | aux3 << 16 | aux2 << 8 | aux1;

  //Configuración de pines
  pinMode(c_on, INPUT);
  pinMode(boom, INPUT);
  pinMode(hoist, INPUT);
  pinMode(gantry, INPUT);
  pinMode(trolley, INPUT);
  
  //Configuración de ethernet
  Ethernet.begin(mac, ip);
  server.begin();
  
  //Inicialización de OPC
  aOPCEthernet.setup(listen_port,mac,ip);

  //Inicialización de objetos OPC
  aOPCEthernet.addItem("Control_On_STS-01", opc_read, opc_float, callback_c_on);
  aOPCEthernet.addItem("Boom_STS-01", opc_read, opc_float, callback_boom);
  aOPCEthernet.addItem("Hoist_STS-01", opc_read, opc_float, callback_hoist);
  aOPCEthernet.addItem("Gantry_STS-01", opc_read, opc_float, callback_gantry);
  aOPCEthernet.addItem("Trolley_STS-01", opc_read, opc_float, callback_trolley);
delay(2000);
}
void  loop() {//código para enviar datos por ethernet
  EthernetClient client = server.available(); //para que llame a esta subrrutina primero debe haber un cliente
  if (client) {
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n' && currentLineIsBlank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println("Refresh:10");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head>");
          client.println("<title>Horometro</title>");
          client.println("</head>");
          client.println("<body>");
          client.println("<table>");
          client.println("<tr>STS-1</tr>");
          client.print("<tr>");
          client.print("<td>CONTROL ON</td>");
          client.print("<td>");
          client.print(horometro[0]);
          client.print("</td>");
          client.print("</tr>");
          client.println("<tr>");
          client.print("<td>BOOM</td>");
          client.print("<td>");
          client.print(horometro[1]);
          client.print("</td>");
          client.println("</tr>");
          client.print("<tr>");
          client.print("<td>HOIST</td>");
          client.print("<td>");
          client.print(horometro[2]);
          client.print("</td>");
          client.println("</tr>");
          client.print("<tr>");
          client.print("<td>GANTRY</td>");
          client.print("<td>");
          client.print(horometro[3]);
          client.print("</td>");
          client.println("</tr>");
          client.print("<tr>");
          client.print("<td>TROLLEY</td>");
          client.print("<td>");
          client.print(horometro[4]);
          client.print("</td>");
          client.println("</tr>");
          client.println("</table>");
          client.println("</body>");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
  }
  
/*
 * Cuando se cumpla una hora, el horómetro físico enviará un pulso positivo 
 * y el arduino irá a las subrrutinas correspondientes de incremento de hora.
 */
  if (digitalRead(c_on) == HIGH) {
    inte1();
  }
  if (digitalRead(boom) == HIGH) {
    inte2();
  }
  if (digitalRead(hoist) == HIGH) {
    inte3();
  }
  if (digitalRead(gantry) == HIGH) {
    inte4();
  }
  if (digitalRead(trolley) == HIGH) {
    inte5();
  }
  aOPCEthernet.processOPCCommands(); //comando que llama a la subrrutina del OPC cuando se solicita desde el servidor.
}

void  inte1() {
  /*
   * Subrrutina de incremento de horas, aquí se hacen las sumas y además se guardan
   * los datos en las EEPROM.
   */
   /*
    * Para el debounce se utilizan dos criterios, si la resta entre millis y la variable
    * de comparación es mayor que 3 segundos o menor que cero, esto para evitar el millisoverflow.
    */
  if ((unsigned long)(millis() - tiempo[0]) < 0 || (unsigned long)(millis() - tiempo[0]) > 3000) {
    horometro[0]++;
    EEPROM.write(10, horometro[0]);
    EEPROM.write(11, horometro[0] >> 8);
    EEPROM.write(12, horometro[0] >> 16);
    EEPROM.write(13, horometro[0] >> 24);
    tiempo[0] = millis();
  }
}
void  inte2() {
  if ((unsigned long)(millis() - tiempo[1]) < 0 || (unsigned long)(millis() - tiempo[1]) > 3000) {
    horometro[1]++;
    EEPROM.write(20, horometro[1]);
    EEPROM.write(21, horometro[1] >> 8);
    EEPROM.write(22, horometro[1] >> 16);
    EEPROM.write(23, horometro[1] >> 24);
    tiempo[1] = millis();
  }
}
void  inte3() {
  if ((unsigned long)(millis() - tiempo[2]) < 0 || (unsigned long)(millis() - tiempo[2]) > 3000) {
    horometro[2]++;
    EEPROM.write(30, horometro[2]);
    EEPROM.write(31, horometro[2] >> 8);
    EEPROM.write(32, horometro[2] >> 16);
    EEPROM.write(33, horometro[2] >> 24);
    tiempo[2] = millis();
  }
}
void  inte4() {
  if ((unsigned long)(millis() - tiempo[3]) < 0 || (unsigned long)(millis() - tiempo[3]) > 3000) {
    horometro[3]++;
    EEPROM.write(40, horometro[3]);
    EEPROM.write(41, horometro[3] >> 8);
    EEPROM.write(42, horometro[3] >> 16);
    EEPROM.write(43, horometro[3] >> 24);
    tiempo[3] = millis();
  }
}
void  inte5() {
  if ((unsigned long)(millis() - tiempo[4]) < 0 || (unsigned long)(millis() - tiempo[4]) > 3000) {
    horometro[4]++;
    EEPROM.write(50, horometro[4]);
    EEPROM.write(51, horometro[4] >> 8);
    EEPROM.write(52, horometro[4] >> 16);
    EEPROM.write(53, horometro[4] >> 24);
    tiempo[4] = millis();
  }
}
