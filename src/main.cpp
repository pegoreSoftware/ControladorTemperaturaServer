#include <ESP8266WiFi.h>       // Servidor Wifi
#include <ESP8266WebServer.h>  // WebServer
#include <ESP8266mDNS.h>       //Servidor dns
#include <WiFiClient.h>        // Cliente Wifi
#include <ArduinoJson.h>       //Serializa e desserializa Json
#include <OneWire.h>           // Sensor um fio
#include <DallasTemperature.h> //Sensor Dallas usado o DS18B20

ESP8266WebServer server(80);
const char *wifiName = "LINO-WIFI";
const char *wifiPass = "22051974";

const int oneWireBus = 5;            // GPIO onde o sensor está conectado
OneWire oneWire(oneWireBus);         // Objeto oneWire para comunicação com o sensor
DallasTemperature sensors(&oneWire); // Passando a referencia do sensor Dallas para o objeto onewire
//Conexões LED
const int LEDVermelho = 4; //D2  GPIO4
const int LEDVerde = 0;    //D3  GPIO0
const int LEDAzul = 2;     //D4  GPIO2

void getTemperatura()
{
  sensors.requestTemperatures();
  float temperaturaAtual = sensors.getTempCByIndex(0);
  Serial.print(temperaturaAtual);
  Serial.println("ºC");

  const size_t capacity = JSON_ARRAY_SIZE(3) + 5 * JSON_OBJECT_SIZE(2);
  DynamicJsonDocument doc(capacity);

  JsonObject Atual = doc.createNestedObject("Atual");
  Atual["Temperatura"] = temperaturaAtual;
  Atual["Setup"] = 18;

  JsonArray Rampas = doc.createNestedArray("Rampas");

  JsonObject Rampas_0 = Rampas.createNestedObject();
  Rampas_0["temperatura"] = 10;
  Rampas_0["data"] = "2020-04-30";

  JsonObject Rampas_1 = Rampas.createNestedObject();
  Rampas_1["temperatura"] = 0;
  Rampas_1["data"] = "2020-05-05";

  JsonObject Rampas_2 = Rampas.createNestedObject();
  Rampas_2["temperatura"] = -0.5;
  Rampas_2["data"] = "2020-05-10";

  String json;
  serializeJson(doc, json);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

void setTemperatura()
{
  String retorno = "Nada";
  if (server.hasArg("plain") == true)
  {
    retorno = server.arg("plain");
  }
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "{\"Recebido\":" + retorno + "}");
}
void setRampa()
{
  String retorno = "Nada";
  if (server.hasArg("plain") == true)
  {
    retorno = server.arg("plain");
  }
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "{\"Recebido\":" + retorno + "}");
}
void handleRoot()
{
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "Hello world!"); // Send HTTP status 200 (Ok) and send some text to the browser/client
}

void handleNotFound()
{
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

void setup(void)
{
  Serial.begin(115200); // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println();
  Serial.println("Iniciando... ");
  Serial.printf("Conectando a rede wifi: %s ", wifiName);
  WiFi.begin(wifiName, wifiPass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.print("Conectado a: ");
  Serial.print(WiFi.SSID());
  Serial.println('\n');
  Serial.print("Endereço IP: ");
  Serial.print(WiFi.localIP());
  Serial.println('\n');
  if (MDNS.begin("ESP8266"))
  { // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  }
  else
  {
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println('\n');
  Serial.print("Inicializando o Sensor na porta: ");
  Serial.print(oneWireBus);
  Serial.println('\n');
  sensors.begin();
  Serial.print("Inicializando as portas do LED RGB: ");
  Serial.print("Vermelho: ");
  pinMode(LEDVermelho, OUTPUT);
  Serial.println("OK ");
  Serial.print("Verde: ");
  pinMode(LEDVerde, OUTPUT);
  Serial.println("OK ");
  Serial.print("Azul: ");
  pinMode(LEDAzul, OUTPUT);
  Serial.println("OK ");

  // Chama a função 'handleRoot' quando buscar o endereço 'http://enderecoControlador'
  server.on("/", handleRoot);

  // Chama a função 'getTemperatura' quando buscar o endereço 'http://enderecoControlador/temperatura' e usar método GET
  server.on("/temperatura", HTTP_GET, getTemperatura);

  //  Chama a função 'setTemperatura' quando buscar o endereço 'http://enderecoControlador/temperatura' e usar método POST
  server.on("/temperatura", HTTP_POST, setTemperatura);

  //  Chama a função 'setRampa' quando buscar o endereço 'http://enderecoControlador/rampa' e usar método POST
  server.on("/rampa", HTTP_POST, setRampa);

  // Retorna um 404 quando tiver uma url desconhecida
  server.onNotFound(handleNotFound);

  server.begin(); //Inicializa o servidor
  Serial.println("Servidor HTTP inicializado");
}

void loop(void)
{
  digitalWrite(LEDVermelho, LOW);
  digitalWrite(LEDVerde, LOW);
  digitalWrite(LEDAzul, LOW);
  //digitalWrite(LEDVermelho, HIGH);
  // digitalWrite(LEDVerde, LOW);
  // digitalWrite(LEDAzul, LOW);
  // delay(1000);
  // digitalWrite(LEDVermelho, LOW);
  // digitalWrite(LEDVerde, HIGH);
  // digitalWrite(LEDAzul, LOW);
  // delay(1000);
  // digitalWrite(LEDVermelho, LOW);
  // digitalWrite(LEDVerde, LOW);
  // digitalWrite(LEDAzul, HIGH);
  // delay(1000);
  // Aguarda requisições do cliente
  server.handleClient();
}