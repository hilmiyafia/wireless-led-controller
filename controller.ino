#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

const char* ssid = "SSID";
const char* password = "PASSWORD";

String ledStatus = "Off";
int ledPin = 16;

ESP8266WebServer server(80);

void handleRoot() {
  String page = R"=====(
  <!DOCTYPE html>
  <html>
    <body>
      <br><br>
      <center>
      <font size="10"><b>The Lamp is )=====" + ledStatus + R"=====(</b></font><br><br>
      <font size="6"><a href="on">Turn On</a>&emsp;<a href="off">Turn Off</a></font>
      </center>
    </body>
  </html>
  )=====";
  server.send(200, "text/html", page);
}

void handleOn() {
  ledStatus = "On";
  digitalWrite(ledPin, HIGH);
  handleRoot();
}

void handleOff() {
  ledStatus = "Off";
  digitalWrite(ledPin, LOW);
  handleRoot();
}

void setup() {
  Serial.begin(9600);
  
  // Initialize LED
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  Serial.print("\nConnecting to ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(" .");
  }
  Serial.print("\nConnected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Set up MDNS responder
  if (MDNS.begin("house"))
    Serial.println("MDNS responder started");
  
  // Start TCP (HTTP) server
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.begin();
  Serial.println("HTTP server started");

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
}

void loop() {
  MDNS.update();
  server.handleClient();
}
