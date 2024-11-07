#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Wi-Fi Credentials
const char* ssid = "narzo 30 Pro 5G";
const char* password = "eb528b498867";

// Server IP and Static IP Configuration
IPAddress serverIP(192, 168, 43, 99);  // ESP1 IP Address
IPAddress local_IP(192, 168, 43, 100);  // ESP2 Static IP
IPAddress gateway(192, 168, 43, 1);
IPAddress subnet(255, 255, 255, 0);

// IR Sensor Pins
const int irSensor1 = D5;
const int irSensor2 = D6;

void setup() {
  Serial.begin(9600);

  pinMode(irSensor1, INPUT);
  pinMode(irSensor2, INPUT);

  // Configure Static IP
  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(ssid, password);

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! Client IP Address: " + WiFi.localIP().toString());
}

void loop() {
  int vehicleCount = getVehicleCount();
  String density = getDensity(vehicleCount);

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    String url = "http://" + serverIP.toString() + "/density";
    http.begin(client, url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.setTimeout(1000);  // Set timeout to 10 seconds

    String postData = "density=" + density;

    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Server Response: " + response);
    } else {
      Serial.println("Sending Density... ");
     // Serial.print("Error on sending POST: ");
     // Serial.println(httpResponseCode);
      //Serial.println("Error: " + http.errorToString(httpResponseCode));  // Print error description
      
    }

    http.end();  // Close connection
  }

  delay(5000);  // Wait 5 seconds before next request
}

// Function to get vehicle count from IR sensors
int getVehicleCount() {
  int count = 0;

  if (digitalRead(irSensor1) == LOW) {  // Vehicle detected on lane 1
    count++;
  }

  if (digitalRead(irSensor2) == LOW) {  // Vehicle detected on lane 2
    count++;
  }

  return count;
}

// Function to determine traffic density
String getDensity(int vehicleCount) {
  if (vehicleCount >= 2) {
    return "high";
  } else if (vehicleCount == 1) {
    return "medium";
  } else {
    return "low";
  }
}
