#include <ArduinoBLE.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  Serial.println("Arduino 2 scanning for SensorArduino...");

  BLE.scanForName("SensorArduino");
}

void loop() {
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    Serial.print("Found device: ");
    Serial.println(peripheral.localName());

    if (peripheral.localName() == "SensorArduino") {
      BLE.stopScan();

      connectToSensor(peripheral);

      BLE.scanForName("SensorArduino");
    }
  }
}

void connectToSensor(BLEDevice peripheral) {
  Serial.println("Connecting...");

  if (!peripheral.connect()) {
    Serial.println("Connection failed");
    return;
  }

  Serial.println("Connected");

  if (!peripheral.discoverAttributes()) {
    Serial.println("Attribute discovery failed");
    peripheral.disconnect();
    return;
  }

  BLECharacteristic sensorCharacteristic =
    peripheral.characteristic("19B10001-E8F2-537E-4F6C-D104768A1214");

  if (!sensorCharacteristic) {
    Serial.println("Sensor characteristic not found");
    peripheral.disconnect();
    return;
  }

  while (peripheral.connected()) {
    if (sensorCharacteristic.canRead()) {
      int sensorValue = 0;

      sensorCharacteristic.readValue(sensorValue);

      Serial.print("Received sensor value: ");
      Serial.println(sensorValue);
    }

    delay(500);
  }

  Serial.println("Peripheral disconnected");
}