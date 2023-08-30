// DS18b20
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Homie.h>

// Software specifications
#define FW_NAME    "esp8266-carport"
#define FW_VERSION "1.0"

#if defined(ESP8266)
// Beschriftung : Nummer im Code
// https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
#define D0 (16)   // WKUP 
#define D1 (5)    // rts
#define D2 (4)    // ow_bus
#define D3 (0)    // FLASH
#define D4 (2)    // LED + relais 1
#define D5 (14)   // rx
#define D6 (12)   // tx
#define D7 (13)   // relais 2
#define D8 (15)   // hm, hm
#define RX (3)    // USB
#define TX (1)    // USB
#define D9 (11)
#define D10 (12)
#define A0  (A0)  // Analog
#endif

#define DEBUG           3
#define ONE_WIRE_BUS    4    
#define OW_ADR_LEN      17

typedef struct {
  char address[17];
  char ow_name[32];
  DeviceAddress rawAdr;
  float value;
} owDevice;

#define MAX_SENSORS 16

typedef struct {
  int pin;
  String state;
  const char* name;
  bool (* callbackFunc)(const HomieRange& range, const String& value);
} relayType;

bool rel1Handler(const HomieRange& range, const String& value);
bool rel2Handler(const HomieRange& range, const String& value);
bool rel3Handler(const HomieRange& range, const String& value);
bool rel4Handler(const HomieRange& range, const String& value);

relayType relays[] = {
    {D1, "OFF", "Relais1", rel1Handler},
    {D5, "OFF", "Relais2", rel2Handler},
    {D6, "OFF", "Relais3", rel3Handler},
    {D7, "OFF", "Relais4", rel4Handler}
  };
const uint8_t NUM_RELAYS  = sizeof(relays)/sizeof(relayType);
