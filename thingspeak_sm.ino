#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ThingSpeak.h>

#define DHTPIN D3          // GPIO Pin where the DHT sensor is connected
#define DHTTYPE DHT11      // DHT sensor type
DHT dht(DHTPIN, DHTTYPE);
int moisturePin = A0;     // Moisture sensor pin
const int motorPin = D2;

// WiFi credentials
const char* WIFI_NAME = "realme 8 5G";
const char* WIFI_PASSWORD = "monika0000";

// ThingSpeak channel details
const int myChannelNumber =  2473568;
const char* myApiKey = "KST858TOPXOPJAFY";

float moisturePercentage;  // Moisture reading
float h;                    // Humidity reading
float t;                    // Temperature reading
WiFiClient client;  // Declare WiFi client
void setup()
{
  Serial.begin(115200);
  delay(10);
  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, LOW); // Keep motor off initially
  dht.begin();

  // Connect to WiFi
  WiFi.begin(WIFI_NAME, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Wifi not connected");
  }
  Serial.println("Wifi connected !");
Serial.println("Local IP: " + WiFi.localIP().toString());

  // Initialize ThingSpeak client
  ThingSpeak.begin(client);
}

void loop()
{
  delay(3000);

  h = dht.readHumidity();     // Read humidity
  t = dht.readTemperature();  // Read temperature

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
  }

  moisturePercentage = (100.00 - ((analogRead(moisturePin) / 1023.00) * 100.00));

  Serial.print("Soil Moisture is  = ");
  Serial.print(moisturePercentage);
  Serial.println("%");
  Serial.print("Temperature is  = ");
  Serial.println(t);
  Serial.print("Humidity is  = ");
  Serial.println(h);

  if (moisturePercentage < 20 ) {
    digitalWrite(motorPin, HIGH);  // Turn on motor
  } else {
    if (t > 31) {
      digitalWrite(motorPin, HIGH);
    } else {
      digitalWrite(motorPin, LOW);
    }
  }

  // Update ThingSpeak fields
  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);
  ThingSpeak.setField(3, moisturePercentage);

  // Send data to ThingSpeak
  int x = ThingSpeak.writeFields(myChannelNumber, myApiKey);

  // Print temperature and humidity to serial monitor
  Serial.println("Data sent to ThingSpeak");

  // Check if data was successfully pushed to ThingSpeak
  if (x == 200) {
    Serial.println("Data pushed successfully");
  } else {
    Serial.println("Push error: " + String(x));
  }
  Serial.println("---");

  // Delay for 10 seconds before the next iteration
  delay(5000);
}