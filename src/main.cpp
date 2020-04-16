#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h> // Include the WebServer library
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

ESP8266WebServer server(80);
const char *wifiName = "LINO-WIFI";
const char *wifiPass = "22051974";

void getTemperatura()
{
  const size_t capacity = JSON_ARRAY_SIZE(3) + 5 * JSON_OBJECT_SIZE(2);
  DynamicJsonDocument doc(capacity);

  JsonObject Atual = doc.createNestedObject("Atual");
  Atual["Temperatura"] = 20;
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
  server.send(200, "application/json", json);
}

void setTemperatura()
{
  String retorno = "Nada";
  if (server.hasArg("plain") == true)
  {
    retorno = server.arg("plain");
  }
  server.send(200, "application/json", "{\"Recebido\":" + retorno + "}");
}
void setRampa()
{
  String retorno = "Nada";
  if (server.hasArg("plain") == true)
  {
    retorno = server.arg("plain");
  }
  server.send(200, "application/json", "{\"Recebido\":" + retorno + "}");
}
void handleRoot()
{
  server.send(200, "text/plain", "Hello world!"); // Send HTTP status 200 (Ok) and send some text to the browser/client
}

void handleNotFound()
{
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
  // Aguarda requisições do cliente
  server.handleClient(); 
}

/*String retorno;
  float temperatura = 20.0;
  float setup = 18.0;
  float tempRampa01 = 10.0;
  float tempRampa02 = 0.0;
  float tempRampa03 = -0.5;
  String dataRampa01 = "2020-04-30";
  String dataRampa02 = "2020-05-05";
  String dataRampa03 = "2020-05-10";

  retorno = "{";
  retorno += "\"Atual\" : ";
  retorno += "            {";
  retorno += "              \"Temperatura\": ", temperatura, ",";
  retorno += "              \"Setup\"      : ", setup, ",";
  retorno += "            },";
  retorno += "\"Rampas\" : ";
  retorno += "             [";
  retorno += "               {";
  retorno += "                 \"temperatura\": %0", tempRampa01;
  retorno += "                 \"data\"       : ", dataRampa01;
  retorno += "                },";
  retorno += "                {";
  retorno += "                 \"temperatura\": ", tempRampa02;
  retorno += "                 \"data\"       : ", dataRampa02;
  retorno += "                },";
  retorno += "                {";
  retorno += "                 \"temperatura\": ", tempRampa03;
  retorno += "                 \"data\"       : ", dataRampa03;
  retorno += "                }";
  retorno += "               ]";
  retorno += "}";
*/