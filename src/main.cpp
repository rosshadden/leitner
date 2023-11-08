#include "Arduino.h"
#include "WiFiNINA.h"

#include "secrets.h"

const int btnPin = 2;

char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

int status = WL_IDLE_STATUS;
bool busy = false;

WiFiClient client;

void printWifiStatus() {
	Serial.print("\nSSID: ");
	Serial.println(WiFi.SSID());
	Serial.print("IP Address: ");
	Serial.println(WiFi.localIP());
	Serial.print("signal strength (RSSI):");
	Serial.print(WiFi.RSSI());
	Serial.println(" dBm\n");
}

void setup() {
	pinMode(btnPin, INPUT_PULLUP);
	pinMode(LED_BUILTIN, OUTPUT);

	Serial.begin(9600);

	// TODO: temp @debug
	while (!Serial) { ; }
	Serial.println("starting...");

	while (status != WL_CONNECTED) {
		Serial.print("connecting: ");
		Serial.println(ssid);
		status = WiFi.begin(ssid, pass);
		delay(1000);
	}

	printWifiStatus();
}

void loop() {
	if (!busy && digitalRead(btnPin) == LOW) {
		busy = true;
		delay(10);
	} else if (busy && digitalRead(btnPin) == HIGH) {
		busy = false;
		delay(10);
	}
}
