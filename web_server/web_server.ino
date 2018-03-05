#include <ESP8266WiFi.h>

// Declaracion e inicializacion de variables utilizadas para
// realizar la conexion a la red Wifi
const char WiFiSSID[] = "SSID"; //SSID de la red Wifi
const char WiFiPSK[] = "Password";  //Contraseña WPA, WPA2 o WEP


// Inicializacion de pines
const int LED_PIN = 5;      
const int ANALOG_PIN = A0;
const int DIGITAL_PIN = 12; 

// Declaracion del objeto que actua como servidor y configura 
// el puerto 80 que es la que respondera a solicitudes HTTP
WiFiServer server(80);

void setup() 
{
  //Se inicializa el puerto serial a 9600 baudios
  Serial.begin(9600); 

  //Se configuran los pines fisicos de entradas y salidas
  //No es necesario configurar los pines de entradas analogicas
  pinMode(DIGITAL_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  //Se configura y se realiza la conexion a la red Wifi
  connectWiFi();
  server.begin();
}

void loop() 
{
   
  // Se prueba si el dispositivo cliente ha realizado una solicitud
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Lectura de la primera linea de la solicitud
  String req = client.readStringUntil('\r'); //Se lee la solicitud hasta \r
  Serial.println(req); //Se muestra la solicitud realizada en el puerto Serial
  client.flush(); //Se descartan los datos que han sido escritos por el cliente pero aun no se leen
  
  int val = -1; 
  if (req.indexOf("/?var=led_on") != -1)
    val = 1; // Escribira LED en alto
  else if (req.indexOf("/?var=led_off") != -1)
    val = 0; // Escribira LED en bajo
  else if (req.indexOf("/?var=read") != -1)
    val = -2; // Se imprime las lecturas de los pines

  // Se configura GPIO5 de acuerdo a la solicitud
  if (val >= 0)
    digitalWrite(LED_PIN, val);

  client.flush();

  // Se inicia la escritura de la pagina web
  String s = "<!DOCTYPE HTML>";
  s += "<html>";
  // Se prepara la respuesta. Iniciamos con un header comun: 
  if (val >= 0)
  {
    // Si se ha realizado una orden de encendido/apagado de LED, entonces 
    //se imprime un mensaje diciendo que su estado
    s += "<head><title>Puertos de salida.</title>";
    s += "<body><h1> Estado de LED</h1>";
    s += "El LED esta ";
    s += (val)?"encendido":"apagado";
  }
  else if (val == -2)
  { // Se si ha ordenado la lectura de los pines, entonces
    // se realiza la lectura:
    s += "<head><title>Puertos de entrada.</title>";
    s += "<meta http-equiv=\"refresh\" content=\"1\"></head>"; //Funcion que realiza el refrescamiento cada segundo
    s += "<body><h1> Lecturas Analogica y Digital</h1>";
    s += "Pin Analogico = ";
    s += String(analogRead(ANALOG_PIN));
    s += "<br>"; // Go to the next line.
    s += "Pin Digital 12 = ";
    s += String(digitalRead(DIGITAL_PIN));
    s += "<p><em> La pagina se actualiza cada segundo.</em></p></body>"; 
  }
  else
  {
    //En el caso de que la consulta no se valida se imprime el mensaje
    s += "Consulta invalida.<br> ";
  }

  s += "</html>"; //Se termina de imprimir la pagina web
  // Se envia la respuesta al cliente
  client.print(s);
  delay(1);
  Serial.println("Client desconectado");
}

void connectWiFi()
{
  //Configuraciones para conectar el dispositivo a la red Wifi
  byte ledStatus = LOW;
  Serial.println();
  Serial.println("Conectando a: " + String(WiFiSSID));
  // Configuracion de Wifi en modo station 
  WiFi.mode(WIFI_STA);

  // WiFI.begin([ssid], [passkey]) inicializa una conexion WiFI
  // para el ID [ssid], utilizando el [passkey] como una contraseña 
  // WPA, WPA2,o WEP.
  WiFi.begin(WiFiSSID, WiFiPSK);

  // Utilizaos la funcion WiFi.status()  para chequear si el ESP8266
  // esta conectado a una red WiFi.
  while (WiFi.status() != WL_CONNECTED)
  {
    // Blink the LED
    digitalWrite(LED_PIN, ledStatus); // Escribe LED high/low
    ledStatus = (ledStatus == HIGH) ? LOW : HIGH;
    delay(100);
  }
  //Se imprime mensaje de conexion realizada y
  //la direccion IP correspondiente a ESP8266
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}



