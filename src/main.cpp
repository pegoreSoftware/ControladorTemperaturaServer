#include <ESP8266WiFi.h>       // Servidor Wifi
#include <ESP8266WebServer.h>  // WebServer
#include <WiFiClient.h>        // Cliente Wifi
#include <ArduinoJson.h>       //Serializa e desserializa Json
#include <OneWire.h>           // Sensor um fio
#include <DallasTemperature.h> //Sensor Dallas usado o DS18B20
#include <ArduinoOTA.h>

IPAddress staticIP(192, 168, 2, 200); //ESP static ip
IPAddress gateway(192, 168, 2, 1);    //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);   //Subnet mask
IPAddress dns(181, 213, 132, 2);      //DNS

ESP8266WebServer server(80);
const char *wifiName = "LINO-WIFI";
const char *wifiPass = "22051974";

const int oneWireBus = 12;           // D6 GPIO12 onde o sensor está conectado
OneWire oneWire(oneWireBus);         // Objeto oneWire para comunicação com o sensor
DallasTemperature sensors(&oneWire); // Passando a referencia do sensor Dallas para o objeto onewire
//Conexões rele
const int LEDAzul = 5;          //D1  GPIO5
const int LEDVerde = 4;         //D2  GPIO4
const int LEDVermelho = 0;      //D3  GPIO0
const int releResfriamento = 2; //D4  GPIO2
const int releAquecimento = 14; //D5  GPIO14

float temperaturaEsperada = 18;

void getTemperatura()
{
  sensors.requestTemperatures();
  float temperaturaAtual = sensors.getTempCByIndex(0);
  Serial.print("Get: ");
  Serial.print(temperaturaAtual);
  Serial.println(" ºC");

  const size_t capacity = JSON_ARRAY_SIZE(3) + 5 * JSON_OBJECT_SIZE(2);
  DynamicJsonDocument doc(capacity);

  JsonObject Atual = doc.createNestedObject("Atual");
  Atual["Temperatura"] = temperaturaAtual;
  Atual["Setup"] = temperaturaEsperada;

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
  const size_t capacity = JSON_ARRAY_SIZE(3) + 5 * JSON_OBJECT_SIZE(2);
  DynamicJsonDocument doc(capacity);
  DeserializationError error = deserializeJson(doc, server.arg("plain"));
  Serial.println(server.arg("plain"));
  server.sendHeader("Access-Control-Allow-Origin", "*");
  if (error)
  {
    server.send(400, "application/json", "{\"msg\":\"Erro no request\"");
    return;
  }
  temperaturaEsperada = doc["Setup"];
  server.send(200, "application/json", server.arg("plain"));
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
  WiFi.config(staticIP, subnet, gateway, dns);
  WiFi.begin(wifiName, wifiPass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.print('.');
  }
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready OTA ESP8266");
  Serial.println('\n');
  Serial.print("Conectado a: ");
  Serial.print(WiFi.SSID());
  Serial.println('\n');
  Serial.print("Endereço IP: ");
  Serial.print(WiFi.localIP());
  Serial.println('\n');

  Serial.println('\n');
  Serial.print("Inicializando o Sensor na porta: ");
  Serial.print(oneWireBus);
  Serial.println('\n');
  sensors.begin();
  Serial.println("Inicializando o rele: ");
  Serial.print("Rele: ");
  pinMode(releResfriamento, OUTPUT);
  pinMode(releAquecimento, OUTPUT);
  pinMode(LEDAzul, OUTPUT);
  pinMode(LEDVerde, OUTPUT);
  pinMode(LEDVermelho, OUTPUT);
  Serial.println("Ok");

  // Chama a função 'handleRoot' quando buscar o endereço 'http://enderecoControlador'
  server.on("/", handleRoot);

  // Chama a função 'getTemperatura' quando buscar o endereço 'http://enderecoControlador/temperatura' e usar método GET
  server.on("/temperatura", HTTP_GET, getTemperatura);

  //  Chama a função 'setTemperatura' quando buscar o endereço 'http://enderecoControlador/temperatura' e usar método POST
  server.on("/temperaturaPost", HTTP_POST, setTemperatura);

  //  Chama a função 'setRampa' quando buscar o endereço 'http://enderecoControlador/rampa' e usar método POST
  server.on("/rampa", HTTP_POST, setRampa);

  // Retorna um 404 quando tiver uma url desconhecida
  server.onNotFound(handleNotFound);

  server.begin(); //Inicializa o servidor
  Serial.println("Servidor HTTP inicializado");
}

void loop(void)
{
  ArduinoOTA.handle();
  server.handleClient();
  sensors.requestTemperatures();
  float temperaturaAtual = (int)sensors.getTempCByIndex(0);
  Serial.println(temperaturaAtual);
  if (temperaturaAtual >= temperaturaEsperada + 1)
  {
    digitalWrite(releResfriamento, LOW);
    digitalWrite(releAquecimento, HIGH);
    digitalWrite(LEDAzul, LOW);
    digitalWrite(LEDVerde, LOW);
    digitalWrite(LEDVermelho, HIGH);
    return;
  }
  if (temperaturaAtual <= temperaturaEsperada - 1)
  {

    digitalWrite(releResfriamento, HIGH);
    digitalWrite(releAquecimento, LOW);
    digitalWrite(LEDAzul, HIGH);
    digitalWrite(LEDVerde, LOW);
    digitalWrite(LEDVermelho, LOW);
    return;
  }
  digitalWrite(LEDAzul, LOW);
  digitalWrite(LEDVerde, HIGH);
  digitalWrite(LEDVermelho, LOW);
}