/*
 *  Horómetro para STS-06
*/
#include <OPC.h>
#include  <Bridge.h>
#include <TimeLib.h>
#include <Wire.h>
#include <DS1307RTC.h>
#include <EEPROM.h>
#include <Ethernet.h>
#include <SPI.h>

//Nombramiento de variables.
unsigned long time_1[5], time_0[5], time_a[5], time_f[5]; //Variables de comparación de tiempo
unsigned long millis_1[5], millis_0[5], millis_f[5]; //Variables de comparación de tiempos menores a 1 s.
unsigned long debounce[6]; //Antirrebote
unsigned long dia[5]; //Variable donde se guardará el día del RTC.
int a, b, c, d, e;  //Índices a utilizar
const byte estado_fuente = 3; //Nombre dado aun PIN para interrupción.
const int c_on = 22;   //Asignación de Pines para los movimientos de la grúa.
const int boom = 24;
const int hoist = 26;
const int gantry = 28;
const int trolley = 30;
unsigned long Time_Debounce = 50; //tiempo asignado para el antirrebote, en mS.
long horometro[5], aux1, aux2, aux3, aux4; //variables para conteo de horómetro.
bool k, c_c_on, c_boom, c_hoist, c_gantry, c_trolley, cambiodia[5], millisoverflow[5];//booleanas de propósito general.

//Configuración de shield ethernet
//NOTA: Importante ajustar la IP y la MAC para cada arduino y cada grúa.
byte mac[]  = { 0x90, 0xA2, 0xDA, 0x0F, 0x72, 0x5A};
IPAddress ip(192, 168, 129, 161);
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

void setup() {
delay(2000);
  // Ajuste de variables
  a = b = c = d = e = 0;
  c_c_on = c_boom = c_hoist = c_gantry = c_trolley = false;
  for (int i = 0; i < 5; i++) {
    k= false;
    cambiodia[i] = false;
    millisoverflow[i] = false;
    dia[i] = 0;
    time_1[i] = 0;
    time_0[i] = 0;
    time_a[i] = 0;
    time_f[i] = 0;
    millis_0[i] = 0;
    millis_1[i] = 0;
    millis_f[i] = 0;
  }

  // Obtener memorias EEPROM
  // De los horómetros
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

  // De los tiempos acumulados
  aux3 = EEPROM.read(62); aux2 = EEPROM.read(61); aux1 = EEPROM.read(60);
  time_a[0] = aux3 << 16 | aux2 << 8 | aux1;

  aux3 = EEPROM.read(72); aux2 = EEPROM.read(71); aux1 = EEPROM.read(70);
  time_a[1] = aux3 << 16 | aux2 << 8 | aux1;

  aux3 = EEPROM.read(82); aux2 = EEPROM.read(81); aux1 = EEPROM.read(80);
  time_a[2] = aux3 << 16 | aux2 << 8 | aux1;

  aux3 = EEPROM.read(92); aux2 = EEPROM.read(91); aux1 = EEPROM.read(90);
  time_a[3] = aux3 << 16 | aux2 << 8 | aux1;

  aux3 = EEPROM.read(102); aux2 = EEPROM.read(101); aux1 = EEPROM.read(100);
  time_a[4] = aux3 << 16 | aux2 << 8 | aux1;

  //Configurar Pines
  pinMode(c_on, INPUT);
  pinMode(boom, INPUT);
  pinMode(hoist, INPUT);
  pinMode(gantry, INPUT);
  pinMode(trolley, INPUT);
  pinMode(estado_fuente,INPUT);
  //Obtener información del RTC
  setSyncProvider(RTC.get);

  //Configuración de ethernet
  Ethernet.begin(mac, ip);
  server.begin();

  //Inicialización de OPC
  aOPCEthernet.setup(listen_port,mac,ip);

  //Inicialización de objetos OPC
  aOPCEthernet.addItem("Control_On_STS-06", opc_read, opc_float, callback_c_on);
  aOPCEthernet.addItem("Boom_STS-06", opc_read, opc_float, callback_boom);
  aOPCEthernet.addItem("Hoist_STS-06", opc_read, opc_float, callback_hoist);
  aOPCEthernet.addItem("Gantry_STS-06", opc_read, opc_float, callback_gantry);
  aOPCEthernet.addItem("Trolley_STS-06", opc_read, opc_float, callback_trolley);
  
  delay(2000);
}

void loop() {

  // PÁGINA WEB

  EthernetClient client = server.available();
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
          client.println("<tr>STS-6</tr>");
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

  // CONTEOS DE TIEMPO
  // CONTROL ON
  /*
     Para la medición de tiempo se toman 2 tiempos, uno inicial y uno final, y se restan, para saber cuando tiempo ha pasado
  */
  if (digitalRead(c_on) == HIGH) {
    if ((unsigned long)millis() - debounce[0] < 0 || (unsigned long)millis() - debounce[0] > Time_Debounce) {
      if (c_c_on == false) {// Nos aseguramos de que no entre nuevamente a esta función si ya entró por primera vez
        millis_0[0] = millis();//y tomamos todos los valores de comparación iniciales
        dia[0] = day();
        time_0[0] = hour() * 3600000 + minute() * 60000 + second() * 1000;
        c_c_on = true;
        cambiodia[0] = false;
      }
      //las variables de comparación de tiempo final se tomarán costantemente hasta que
      //se deje de hacer el movimiento.
      millis_1[0] = millis();
      time_1[0] = hour() * 3600000 + minute() * 60000 + second() * 1000;//se transforman todos los contadeores a milisegundos
      if ((millis_1[0] - millis_0[0]) < 0) {//cuando millis se sobrecargue (millis overflow)
        //la resta dará un número menor a 0, en ese caso se tomará el valor absoluto de lo que llevaba
        //se activa una booleana para que luego entre a otra función y se evita que el programa tenga un "bug".
        millis_0[0] = 4294967269 - millis_0[0];
        millisoverflow[0] = true;
      }
      //aquí se utiliza la booleana anterior, en caso de que la resta de millis de negativo se accederá a una función
      //de lo contrario se entrará a la otra.
      if (millisoverflow[0] == false) {
        millis_1[0] = millis_1[0] - millis_0[0];
      } else {
        millis_1[0] = millis_1[0] + millis_0[0];
      }
      if (dia[0] != day()) {//esta función ve si ha habido un cambio de día, en caso de ser cierto
        //hace una rutina muy parecia a la hecha con millis anteriormente.
        cambiodia[0] = true;
        time_0[0] = 86400000 - time_0[0];
      }//y aquí se utiliza la boleana para determinar a qué función acceder.
      if (cambiodia[0] == false) {
        time_f[0] = time_1[0] + time_a[0] - time_0[0] + (millis_1[0] - (time_1[0] - time_0[0]));
      } else {
        time_f[0] = time_1[0] + time_a[0] + time_0[0] + (millis_1[0] - (time_1[0] + time_0[0]));
      }
      if (time_f[0] >= (3600000 * (a + 1))) {//a esta función entra cada vez que la función de un valor
        //mayor a una hora, y luego se direcciona a una función que hace el incremento de hora de horómetro
        //y además aumenta en 1 el índice que sirve para acumular horas, es decir, una vez se cumpla la primera hora
        //no llamará nuevamente hasta que se cumpla otra hora completa.
        cambio_hora1();
        a++;
      }
      debounce[0] = millis();
    }
  }
  if (digitalRead(c_on) == LOW && c_c_on == true) {//función para cuando se suelta el movimiento
    //de la grúa, entrará sólo si previamente se entró el movimiento de la misma, sin la booleanta c_c_on, entraría desde el inicio del programa.
    if ((unsigned long)millis() - debounce[0] < 0 || (unsigned long)millis() - debounce[0] > Time_Debounce) {
      c_c_on = false;//se limpian las booleanas
      millisoverflow[0] = false;
      time_a[0] = (time_f[0] - 3600000 * a);//se re calcula el tiempo acumulado
      a = 0;//se limpia el índice de horas acumuladas sin parar.
      debounce[0] = millis();
    }
  }
//Para todos los otros movimientos se aplica el mismo algoritmo.
  // FIN CONTROL ON

  // BOOM

  if (digitalRead(boom) == HIGH) {
    if ((unsigned long)millis() - debounce[1] < 0 || (unsigned long)millis() - debounce[1] > Time_Debounce) {
      if (c_boom == false) {
        millis_0[1] = millis();
        dia[1] = day();
        time_0[1] = hour() * 3600000 + minute() * 60000 + second() * 1000;
        c_boom = true;
        cambiodia[1] = false;
      }
      millis_1[1] = millis();
      time_1[1] = hour() * 3600000 + minute() * 60000 + second() * 1000 ;
      if ((millis_1[1] - millis_0[1]) < 0) {
        millis_0[1] = 4294967269 - millis_0[1];
        millisoverflow[1] = true;
      }
      if (millisoverflow[1] == false) {
        millis_1[1] = millis_1[1] - millis_0[1];
      } else {
        millis_1[1] = millis_1[1] + millis_0[1];
      }
      if (dia[1] != day()) {
        cambiodia[1] = true;
        time_0[1] = 86400000 - time_0[1];
      }
      if (cambiodia[1] == false) {
        time_f[1] = time_1[1] + time_a[1] - time_0[1] + (millis_1[1] - (time_1[1] - time_0[1]));
      } else {
        time_f[1] = time_1[1] + time_a[1] + time_0[1] + (millis_1[1] - (time_1[1] + time_0[1]));
      }
      if (time_f[1] >= (3600000 * (b + 1))) {
        cambio_hora2();
        b++;
      }
      debounce[1] = millis();
    }
  }
  if (digitalRead(boom) == LOW && c_boom == true) {
    if ((unsigned long)millis() - debounce[1] < 0 || (unsigned long)millis() - debounce[1] > Time_Debounce) {
      c_boom = false;
      millisoverflow[1] = false;
      time_a[1] = (time_f[1] - 3600000 * b);
      b = 0;
      debounce[1] = millis();
    }
  }
  // FIN BOOM

  // HOIST

  if (digitalRead(hoist) == HIGH) {
    if ((unsigned long)millis() - debounce[2] < 0 || (unsigned long)millis() - debounce[2] > Time_Debounce) {
      if (c_hoist == false) {
        millis_0[2] = millis();
        dia[2] = day();
        time_0[2] = hour() * 3600000 + minute() * 60000 + second() * 1000;
        c_hoist = true;
        cambiodia[2] = false;
      }
      millis_1[2] = millis();
      time_1[2] = hour() * 3600000 + minute() * 60000 + second() * 1000;
      if ((millis_1[2] - millis_0[2]) < 0) {
        millis_0[2] = 4294967269 - millis_0[2];
        millisoverflow[2] = true;
      }
      if (millisoverflow[2] == false) {
        millis_1[2] = millis_1[2] - millis_0[2];
      } else {
        millis_1[2] = millis_1[2] + millis_0[2];
      }
      if (dia[2] != day()) {
        cambiodia[2] = true;
        time_0[2] = 86400000 - time_0[2];
      }
      if (cambiodia[2] == false) {
        time_f[2] = time_1[2]  + time_a[2] - time_0[2] + (millis_1[2] - (time_1[2] - time_0[2]));
      } else {
        time_f[2] = time_1[2]  + time_a[2] + time_0[2] + (millis_1[2] - (time_1[2] + time_0[2]));
      }
      if (time_f[2] >= (3600000 * (c + 1))) {
        cambio_hora3();
        c++;
      }
      debounce[2] = millis();
    }
  }
  if (digitalRead(hoist) == LOW && c_hoist == true) {
    //antirrebote
    if ((unsigned long)millis() - debounce[2] < 0 || (unsigned long)millis() - debounce[2] > Time_Debounce) {
      c_hoist = false;
      millisoverflow[2] = false;
      time_a[2] = (time_f[2] - 3600000 * c);
      c = 0;
      debounce[2] = millis();
    }
  }

  // FIN HOIST

  // GANTRY

  if (digitalRead(gantry) == HIGH) {
    if ((unsigned long)millis() - debounce[3] < 0 || (unsigned long)millis() - debounce[3] > Time_Debounce) {
      if (c_gantry == false) {
        millis_0[3] = millis();
        dia[3] = day();
        time_0[3] = hour() * 3600000 + minute() * 60000 + second() * 1000;
        c_gantry = true;
        cambiodia[3] = false;
      }
      millis_1[3] = millis();
      time_1[3] = hour() * 3600000 + minute() * 60000 + second() * 1000;
      if ((millis_1[3] - millis_0[3]) < 0) {
        millis_0[3] = 4294967269 - millis_0[3];
        millisoverflow[3] = true;
      }
      if (millisoverflow[3] == false) {
        millis_1[3] = millis_1[3] - millis_0[3];
      } else {
        millis_1[3] = millis_1[3] + millis_0[3];
      }
      if (dia[3] != day()) {
        cambiodia[3] = true;
        time_0[3] = 86400000 - time_0[3];
      }
      if (cambiodia[3] == false) {
        time_f[3] = time_1[3]  + time_a[3] - time_0[3] + (millis_1[3] - (time_1[3] - time_0[3]));
      } else {
        time_f[3] = time_1[3]  + time_a[3] + time_0[3] + (millis_1[3] - (time_1[3] + time_0[3]));
      }
      if (time_f[3] >= (3600000 * (d + 1))) {
        cambio_hora4();
        d++;
      }
      debounce[3] = millis();
    }
  }
  if (digitalRead(gantry) == LOW && c_gantry == true) {
    if ((unsigned long)millis() - debounce[3] < 0 || (unsigned long)millis() - debounce[3] > Time_Debounce) {
      c_gantry = false;
      millisoverflow[3] = false;
      time_a[3] = (time_f[3] - 3600000 * d);
      d = 0;
      debounce[3] = millis();
    }
  }

  // FIN GANTRY

  // TROLLEY

  if (digitalRead(trolley) == HIGH) {
    if ((unsigned long)millis() - debounce[4] < 0 || (unsigned long)millis() - debounce[4] > Time_Debounce) {
      if (c_trolley == false) {
        millis_0[4] = millis();
        dia[4] = day();
        time_0[4] = hour() * 3600000 + minute() * 60000 + second() * 1000;
        c_trolley = true;
        cambiodia[4] = false;
      }
      millis_1[4] = millis();
      time_1[4] = hour() * 3600000 + minute() * 60000 + second() * 1000;
      if ((millis_1[4] - millis_0[4]) < 0) {
        millis_0[4] = 4294967269 - millis_0[4];
        millisoverflow[4] = true;
      }
      if (millisoverflow[4] == false) {
        millis_1[4] = millis_1[4] - millis_0[4];
      } else {
        millis_1[4] = millis_1[4] + millis_0[4];
      }
      if (dia[4] != day()) {
        cambiodia[4] = true;
        time_0[4] = 86400000 - time_0[4];
      }
      if (cambiodia[4] == false) {
        time_f[4] = time_1[4]  + time_a[4] - time_0[4] + (millis_1[4] - (time_1[4] - time_0[4]));
      } else {
        time_f[4] = time_1[4]  + time_a[4] + time_0[4] + (millis_1[4] - (time_1[4] + time_0[4]));
      }
      if (time_f[4] >= (3600000 * (e + 1))) {
        cambio_hora5();
        e++;
      }
      debounce[4] = millis();
    }
  }
  if (digitalRead(trolley) == LOW && c_trolley == true) {
    //antirrebote
    if ((unsigned long)millis() - debounce[4] < 0 || (unsigned long)millis() - debounce[4] > Time_Debounce) {
      c_trolley = false;
      millisoverflow[4] = false;
      time_a[4] = (time_f[4] - 3600000 * e);
      e = 0;
      debounce[4] = millis();
    }
  }
  // FIN TROLLEY
  
  //OPC
  aOPCEthernet.processOPCCommands(); //comando que llama a la subrrutina del OPC cuando se solicita desde el servidor.
  if(digitalRead(estado_fuente)==HIGH){
    delay(2);
    if(digitalRead(estado_fuente)==HIGH){
      anti_apagon();
    }
  }else{
    k= false;
  }
}



// ANTI-APAGÓN

void anti_apagon() {//subrrutina para evitar que se pierdan las horas acumuladas.
  //Se guardan los valores de los tiempos acumulados, ya que si se guardasen cada vez que
  //se saca un contacto, se desgastarían muy rápido las EEPROM.
  if(k==false){
  if ((unsigned long)millis() - debounce[5] < 0 || (unsigned long)millis() - debounce[5] > Time_Debounce) {
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
    k=true;
  }
  }
}

// FIN ANTI-APAGÓN

// INCREMENTOS DE HORÓMETROS

void cambio_hora1() {//Subrrutina para incremento del horómetro, aquí se aumenta el horómero y se guarda en la EEPROM.
  horometro[0]++;
  EEPROM.write(10, horometro[0]);
  EEPROM.write(11, horometro[0] >> 8);
  EEPROM.write(12, horometro[0] >> 16);
  EEPROM.write(13, horometro[0] >> 24);
}
void cambio_hora2() {
  horometro[1]++;
  EEPROM.write(20, horometro[1]);
  EEPROM.write(21, horometro[1] >> 8);
  EEPROM.write(22, horometro[1] >> 16);
  EEPROM.write(23, horometro[1] >> 24);
}
void cambio_hora3() {
  horometro[2]++;
  EEPROM.write(30, horometro[2]);
  EEPROM.write(31, horometro[2] >> 8);
  EEPROM.write(32, horometro[2] >> 16);
  EEPROM.write(33, horometro[2] >> 24);
}
void cambio_hora4() {
  horometro[3]++;
  EEPROM.write(40, horometro[3]);
  EEPROM.write(41, horometro[3] >> 8);
  EEPROM.write(42, horometro[3] >> 16);
  EEPROM.write(43, horometro[3] >> 24);
}
void cambio_hora5() {
  horometro[4]++;
  EEPROM.write(50, horometro[4]);
  EEPROM.write(51, horometro[4] >> 8);
  EEPROM.write(52, horometro[4] >> 16);
  EEPROM.write(53, horometro[4] >> 24);
}
