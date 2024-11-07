#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Wi-Fi Credentials
const char* ssid = "narzo 30 Pro 5G";
const char* password = "eb528b498867";

// Traffic Light Pins
const int redPin = D1;
const int yellowPin = D2;
const int greenPin = D3;

ESP8266WebServer server(80);  // HTTP server on port 80
String density = "low";  // Default traffic density

void setup() {
  Serial.begin(9600);

  // Set LED pins as OUTPUT
  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  // Static IP Configuration for ESP1
  IPAddress local_IP(192, 168, 43, 99);  
  IPAddress gateway(192, 168, 43, 1);    
  IPAddress subnet(255, 255, 255, 0);    

  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(ssid, password);

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP Address: " + WiFi.localIP().toString());

  // Define route to receive density data
  server.on("/density", HTTP_POST, []() {
    if (server.hasArg("density")) {
      density = server.arg("density");
      Serial.println("Received density: " + density);
      server.send(200, "text/plain", "Density updated");
    } else {
      server.send(400, "text/plain", "Bad Request: 'density' parameter missing");
    }
  });

  server.begin();  // Start the server
  Serial.println("Server started");
}

void loop() {
  server.handleClient();  // Handle HTTP requests

  if (density == "high") {
    runTrafficCycle(12000, 5000);  // High traffic: Long green, short red
  } else if (density == "medium") {
    runTrafficCycle(8000, 8000);   // Medium traffic: Balanced green and red
  } else {
    runTrafficCycle(5000, 12000);  // Low traffic: Short green, long red
  }
}

// Traffic light cycle function
void runTrafficCycle(int greenDuration, int redDuration) {
  greenLight(greenDuration);
  yellowLight(2000);  // Yellow light for 2s
  redLight(redDuration);
}

void greenLight(int duration) {
  digitalWrite(greenPin, HIGH);
  delay(duration);
  digitalWrite(greenPin, LOW);
}

void yellowLight(int duration) {
  digitalWrite(yellowPin, HIGH);
  delay(duration);
  digitalWrite(yellowPin, LOW);
}

void redLight(int duration) {
  digitalWrite(redPin, HIGH);
  delay(duration);
  digitalWrite(redPin, LOW);
}                                                                                                                                                                                                                                               
