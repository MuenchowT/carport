#include "carport.h"

String statusMessage = "Start";
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature dtsensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.

DeviceAddress sensorAdr;
owDevice owSensors[MAX_SENSORS];

// Sensor consts/vars
HomieNode carportNode("CarportEdel42", "CarportEdel42", "CarportEdel42");

int RefreshInterval = 30; 
unsigned long lastMeasureSent = 0;
unsigned long DebugLevel = 1;
unsigned long Uptime = 0;
char * uptStr = "0000T 00:00:00";

const byte sensorPin = A0;      // ADC pin for the sensor
long adc = 0;
const uint16_t resistor = 150;  // used pull down resistor in Ohm
const byte vref = 33;  
const int minAdc = (0.004 * resistor * 1024 / (vref / 10.0));  // ca. 186
const int maxAdc = (0.020 * resistor * 1024 / (vref / 10.0));  // ca. 930
// max -min = ca. 744 (entspr. ca 4m Wasserhöhe) 
// test: volle Zisterne bei regenwetter = ca. 470
const int maxAdcVoll = 470;
unsigned long maxZistVol = 260;
const int cntTries = 3;

int cntSensors = 0;

int getZisterneValue(void)
{
int32_t value = 0;
// drei versuche werden gemittelt
for (int i = 1; i <= cntTries; i++) {
  adc = analogRead(sensorPin);
  //int32_t value = (adc - 186) * maxZistVol / (931 - 186);                        // for 1023*500 we need a long
  value += (adc - minAdc) * int32_t(maxZistVol) / (maxAdc - minAdc);
}
value /= cntTries;
if (DebugLevel >= 1) {
    Homie.getLogger() << "Raw value Zisterne: " << adc << ", umgerechnet: " << value << endl;
  }

if (value > 100) {
  maxZistVol = maxZistVol / value * 100;
  carportNode.setProperty("MaxZistVol").send(String(maxZistVol));
  Homie.getLogger() << "Max Zist Wasserstand set to" << value << endl;
  }
else if (value < 0) 
  value = 0;
  
return  value;
}

void init_ow_sensors(void) {
  dtsensors.begin();
  Homie.getLogger() << "DS18B20 OneWire bus on pin " << ONE_WIRE_BUS << endl;
  cntSensors = dtsensors.getDeviceCount() ;
  Homie.getLogger() << "Found " << cntSensors << " device(s) on bus" << endl;

//  carportNode.advertise("unit");
  for  (int s = 0; s < cntSensors; s++) { 
    char* buf = owSensors[s].ow_name;
    if (!dtsensors.getAddress(sensorAdr, s)) {  // get address of Sensor #s into "sensorAdr"
          Homie.getLogger() <<  "Unable to find address for sensor #" << s << endl;
          continue;
          }
//    *buf++ = 'T-';
    sprintf(owSensors[s].address, "OW%03d", s);
      for (uint8_t i = 0; i < 8; i++)  {
        owSensors[s].rawAdr[i] = sensorAdr[i];
        sprintf(buf, "%02X\0", sensorAdr[i]);
        buf += 2;
        }
    Homie.getLogger() <<  "sensor #" << s << ", HEX: " << owSensors[s].ow_name << endl;
    // apparently, the "ID" parameter of the function "advertise" must not be longer as 14 characters. "SetName" may set a lger name.
    carportNode.advertise((const char*)owSensors[s].address).setName((const char*)owSensors[s].ow_name).setDatatype("string");
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Homie loop Handler
void loopHandler() {

  if ( millis() - lastMeasureSent < RefreshInterval * 1000UL) 
    return;

if (DebugLevel >= 1) {
    Homie.getLogger() << "Refresh interval: " << RefreshInterval << endl;
    Homie.getLogger() << "debug level: " << DebugLevel << endl;
  }

    Homie.getLogger() << "Uptime: " << Uptime << endl;
    uptStr = makeUptimeString(uptStr, Uptime);

    carportNode.setProperty("RefreshInterval").send(String(RefreshInterval));
    carportNode.setProperty("DebugLevel").send(String(DebugLevel));
    carportNode.setProperty("UptimeStr").send(String(uptStr));
    carportNode.setProperty("Uptime").send(String(Uptime));
    carportNode.setProperty("ZistRaw").send(String(adc));
    carportNode.setProperty("MaxZistVol").send(String(maxZistVol));

    for (int r = 0; r < NUM_RELAYS; r++) {
      Homie.getLogger() << relays[r].name << ": " << relays[r].state << endl;
      carportNode.setProperty(relays[r].name).send(relays[r].state);
      }

    dtsensors.requestTemperatures();
    for  (int s = 0; s < cntSensors; s++) { 
      owSensors[s].value = dtsensors.getTempCByIndex(s); 
      Homie.getLogger() << owSensors[s].address << " " << owSensors[s].value << " °C" << endl;
      carportNode.setProperty(owSensors[s].address).send(String(owSensors[s].value));
    }

    int myValue = getZisterneValue(); 
    Homie.getLogger() << "Zisterne" << ": " << myValue << " %" << endl;
    carportNode.setProperty("Zisterne").send(String(myValue));
 
    Uptime += (millis() - lastMeasureSent)/1000UL;
    lastMeasureSent = millis();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Arduino Setup
void setup() {
  Homie.disableLedFeedback();
  Serial.begin(115200); // Required to enable serial output
  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie.setSetupFunction(init_ow_sensors);
  Homie.setLoopFunction(loopHandler);

  //carportNode.advertise("StatusMsg").setName("StatusMessage").setDatatype("string");
  carportNode.advertise("RefreshInterval").settable(RefreshHandler).setName("RefreshInterval").setDatatype("integer");
  carportNode.advertise("DebugLevel").settable(DebugHandler).setName("DebugLevel").setDatatype("integer");
  carportNode.advertise("MaxZistVol").settable(MaxZistValHandler).setName("MaxZistVol").setDatatype("integer");
    
  carportNode.advertise("Uptime").setName("Uptime").setDatatype("integer");
  carportNode.advertise("UptimeStr").setName("Uptime").setDatatype("string");
  carportNode.advertise("ZistRaw").setName("ZistRaw").setDatatype("string");
  carportNode.advertise("Zisterne").setName("Zisterne").setDatatype("integer");
  carportNode.setProperty("unit").send("c");
  
  for (int r = 0; r < NUM_RELAYS; r++) {
    pinMode(relays[r].pin, OUTPUT);
    digitalWrite(relays[r].pin, HIGH);
    carportNode.advertise(relays[r].name).settable(relays[r].callbackFunc).setName(relays[r].name).setDatatype("string");
  }
    
//  init_ow_sensors();
  
  Homie.setup();

// Watchdog:
//ESP.wdtDisable();
//ESP.wdtEnable(2000);

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Arduino Loop
void loop() {
  Homie.loop();
}
