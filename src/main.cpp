#include "Arduino.h"
#include "WiFiNINA.h"

#include "secrets.h"

const int btnPin = 2;
const int ledPin = LED_BUILTIN;

char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;
char host[] = API_HOST;
int port = API_PORT;
char token[] = HA_TOKEN;

int status = WL_IDLE_STATUS;
bool busy = false;

WiFiClient client;

String payload = "{ \"entity_id\": \"input_boolean.debug\" }";
// String payload = "{ \"entity_id\": \"switch.bedroom_lights\" }";

void printWifiStatus() {
	Serial.print("\nSSID: ");
	Serial.println(WiFi.SSID());
	Serial.print("IP Address: ");
	Serial.println(WiFi.localIP());
	Serial.print("signal strength (RSSI):");
	Serial.print(WiFi.RSSI());
	Serial.println(" dBm\n");
}

void makeRequest() {
	Serial.println("connecting server...");
	if (client.connect(host, port)) {
		Serial.print("connected: ");
		Serial.println(host);
		Serial.println();

		// client.println("POST /api/services/switch/toggle");
		client.println("POST /api/services/input_boolean/toggle");
		client.print("Host: ");
		client.println(host);
		client.println("Content-Type: application/json");
		client.print("Authorization: Bearer ");
		client.println(token);
		client.print("Content-Length: ");
		client.println(payload.length());
		client.println();
		client.println(payload);
		client.println("Connection: close");
		client.println();
	}
}

void setup() {
	pinMode(btnPin, INPUT_PULLUP);
	pinMode(ledPin, OUTPUT);

	Serial.begin(9600);

	// TODO: temp @debug
	while (!Serial) { ; }
	Serial.println("starting...");

	while (status != WL_CONNECTED) {
		Serial.println("connecting wifi...");
		status = WiFi.begin(ssid, pass);
		delay(1000);
	}

	// TODO: blink while connecting
	digitalWrite(ledPin, HIGH);
	printWifiStatus();
}

void loop() {
	if (!busy && digitalRead(btnPin) == LOW) {
		busy = true;
		makeRequest();
		delay(10);
	} else if (busy && digitalRead(btnPin) == HIGH) {
		while (client.available()) {
			char c = client.read();
			Serial.write(c);
		}
		Serial.println();

		if (!client.connected()) {
			Serial.println("disconnecting server...\n");
			client.stop();
			Serial.end();
		}

		busy = false;
		delay(10);
	}
}
