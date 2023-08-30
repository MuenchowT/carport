char * makeUptimeString(char* inString, unsigned long uptime)
{
// uptime in sekunden -> tage
unsigned long d = uptime / (60 * 60 * 24);   

uptime -= d * (60 * 60 * 24);  // 39499

unsigned long h = uptime / 3600;     // 10,9719

uptime -= h * (60 * 60);  // 3499

unsigned long m = uptime / 60;  // 58.31
unsigned long s = uptime % 60; // 19

sprintf (inString, "%4dT %02d:%02d:%02d", d, h, m, s);
if (DebugLevel >= 1)
  Homie.getLogger() << "Uptime: " << inString << endl;
return inString;
}


bool RefreshHandler(const HomieRange& range, const String& value) {
  RefreshInterval = strtol(const_cast<char*>(value.c_str()), NULL, 0);
  carportNode.setProperty("RefreshInterval").send(value);
  Homie.getLogger() << "RefreshInterval is set to " << value << endl;
return true;
  }

bool DebugHandler(const HomieRange& range, const String& value) {
  DebugLevel = strtol(const_cast<char*>(value.c_str()), NULL, 0);
  carportNode.setProperty("DebugLevel").send(value);
  Homie.getLogger() << "DebugLevel is set to " << value << endl;
  return true;
  }

bool rel1Handler(const HomieRange& range, const String& value) {
  if (value != "ON" && value != "OFF") {
      Homie.getLogger() << "Relais 1 received " << value << " - returning" << endl;
    return false;
  }
  relays[0].state = value;
  Homie.getLogger() << "Relais 1 (pin " << relays[0].pin << ") val: " << (value == "ON" ? HIGH : LOW ) << endl;
  digitalWrite(relays[0].pin, (value == "ON" ? LOW : HIGH  ));
  carportNode.setProperty("Relais_1").send(value);
  Homie.getLogger() << "Relais 1 is set to " << value << endl;
return true;
}

bool rel2Handler(const HomieRange& range, const String& value) {
  if (value != "ON" && value != "OFF") 
    return false;
  relays[1].state = value;
  digitalWrite(relays[1].pin, (value == "ON" ? LOW : HIGH ));
  carportNode.setProperty("Relais_2").send(value);
  Homie.getLogger() << "Relais 2 is set to " << value << endl;
return true;
  }

bool rel3Handler(const HomieRange& range, const String& value) {
  if (value != "ON" && value != "OFF") 
    return false;
  relays[2].state = value;
  digitalWrite(relays[2].pin, (value == "ON" ? LOW : HIGH ));
  carportNode.setProperty("Relais_3").send(value);
  Homie.getLogger() << "Relais 3 is set to " << value << endl;
return true;
  }

bool rel4Handler(const HomieRange& range, const String& value) {
  if (value != "ON" && value != "OFF") 
    return false;
  relays[3].state = value;
  digitalWrite(relays[3].pin, (value == "ON" ? LOW : HIGH ));
  carportNode.setProperty("Relais_4").send(value);
  Homie.getLogger() << "Relais 4 is set to " << value << endl;
return true;
  }

bool MaxZistValHandler(const HomieRange& range, const String& value) {
  maxZistVol = strtol(const_cast<char*>(value.c_str()), NULL, 0);
  carportNode.setProperty("MaxZistVol").send(value);
  Homie.getLogger() << "Max Zist Volumen set to" << value << endl;
return true;
  }

  
