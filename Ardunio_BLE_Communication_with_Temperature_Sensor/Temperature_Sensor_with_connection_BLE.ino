#include <ArduinoBLE.h>
#include <OneWire.h>

OneWire ds(2);

// Custom BLE service and characteristic UUIDs
BLEService sensorService("19B10000-A8F2-537E-4F6C-D104768A1214");

BLEIntCharacteristic sensorCharacteristic(
  "19B10001-A8F2-537E-4F6C-D104768A1214",
  BLERead | BLENotify
);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("Group_A6");
  BLE.setAdvertisedService(sensorService);

  sensorService.addCharacteristic(sensorCharacteristic);
  BLE.addService(sensorService);

  sensorCharacteristic.writeValue(0);

  BLE.advertise();

  Serial.println("Arduino 1 is advertising as SensorArduino");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to: ");
    Serial.println(central.address());

    while (central.connected()) {
      // Read DS18B20 temperature
      byte d[9];
      ds.reset(); ds.write(0xCC); ds.write(0x44); delay(750);
      ds.reset(); ds.write(0xCC); ds.write(0xBE);
      for (byte i = 0; i < 9; i++) d[i] = ds.read();
      int sensorValue = ((d[1] << 8) | d[0]) / 16;

      sensorCharacteristic.writeValue(sensorValue);

      Serial.print("Sensor value sent: ");
      Serial.println(sensorValue);

      delay(500);
    }

    Serial.println("Disconnected");
  }
}