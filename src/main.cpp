#include "Arduino.h"
#include "WiFiNINA.h"
#include "IRremote.hpp"

#include "secrets.h"

const int btnPin = 3;
const int irPin = 2;
const int ledPin = LED_BUILTIN;

char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;
char host[] = API_HOST;
int port = API_PORT;
char token[] = HA_TOKEN;

int status = WL_IDLE_STATUS;
bool busy = false;
bool debug = false;

WiFiClient client;

void dumpWifi() {
	Serial.print("\nSSID: ");
	Serial.println(WiFi.SSID());
	Serial.print("IP Address: ");
	Serial.println(WiFi.localIP());
	Serial.print("signal strength (RSSI):");
	Serial.print(WiFi.RSSI());
	Serial.println(" dBm\n");
}

void connectWifi() {
	while (status != WL_CONNECTED) {
		Serial.println("connecting wifi...");
		status = WiFi.begin(ssid, pass);
		delay(1000);
	}

	// TODO: blink while connecting
	digitalWrite(ledPin, HIGH);
	dumpWifi();
}

void makeRequest(String endpoint, String payload) {
	Serial.println("connecting server...");
	if (client.connect(host, port)) {
		Serial.print("connected: ");
		Serial.println(host);
		Serial.println();

		client.println("POST /api/services/" + endpoint + "/toggle");
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
	IrReceiver.begin(irPin);

	Serial.begin(9600);

	if (debug) while (!Serial) { ; }
	Serial.println("starting...");

	connectWifi();
}

void loop() {
	String endpoint = "switch";
	String payload;

	if (!busy && IrReceiver.decode()) {
		busy = true;

		if (debug) {
			Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
			IrReceiver.printIRResultShort(&Serial);
			IrReceiver.printIRSendUsage(&Serial);
		}

		switch (IrReceiver.decodedIRData.decodedRawData) {
			// all
			case 0xBA45FF00:
				payload = "{ \"entity_id\": \"switch.bedroom_lights\" }";
				break;

			// bedroom lamps
			case 0xF30CFF00:
				payload = "{ \"entity_id\": \"switch.bedroom_lamps\" }";
				break;

			// salt lamp
			case 0xE718FF00:
				payload = "{ \"entity_id\": \"switch.tp_link_power_strip_9993_salt_lamp\" }";
				break;

			// pineapple lamps
			case 0xA15EFF00:
				payload = "{ \"entity_id\": \"switch.bedroom_pineapple_lamps\" }";
				break;

			// DEBUG
			case 0xB847FF00:
				endpoint = "input_boolean";
				payload = "{ \"entity_id\": \"input_boolean.debug\" }";
				break;
		}

		if (payload.length() > 0) {
			makeRequest(endpoint, payload);
			delay(10);
		}

		IrReceiver.resume();
	} else if (busy) {
		while (client.available()) {
			char c = client.read();
			Serial.write(c);
		}
		Serial.println();

		if (!client.connected()) {
			client.stop();
			Serial.end();
		}

		busy = false;
		delay(10);
	}
}
