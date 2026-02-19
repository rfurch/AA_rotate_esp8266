//========================= SNMP ===================================

EthernetUDP *udpServerSNMP  = nullptr;
SNMPAgent *snmp = nullptr;  // declare pointer, not instance

//const char* rocommunity = "public";  // Read only community string
//const char* rwcommunity = "private"; // Read Write community string for set commands

// RFC1213-MIB (System)
const char* oidSysDescr = ".1.3.6.1.2.1.1.1.0";    // OctetString SysDescr
const char* oidSysUptime = ".1.3.6.1.2.1.1.3.0";   // TimeTicks sysUptime (hundredths of seconds)
const char* oidSysName = ".1.3.6.1.2.1.1.5.0";     // OctetString SysName
std::string sysDescr = "SNMP Agent";

// Numbers used to response to Get requests
int changingNumber = 1;
int settableNumber = 0;
uint32_t tensOfMillisCounter = 0;
const char* changingString;

// arbitrary data will be stored here to act as an OPAQUE data-type
uint8_t* stuff = 0;

// value for callbacks to be defined
ValueCallback* changingNumberOID = nullptr;
ValueCallback* settableNumberOID = nullptr;
TimestampCallback* timestampCallbackOID;
std::string staticString = "This value will never change";

extern persistConfig pconfig; 

//========================= SNMP ===================================
//-----------------------------------------------------------------------------------------------


void addRFC1213MIBHandler() {
  // Add SNMP Handlers of correct type to each OID
  snmp->addReadOnlyStaticStringHandler(oidSysDescr, sysDescr);
  snmp->addReadOnlyStaticStringHandler(oidSysName, pconfig.stationName.c_str());
  //snmp->addReadOnlyStaticStringHandler(oidSysObjectID, sysObjectID);
  //snmp->addIntegerHandler(oidSysServices, &sysServices);
  snmp->addTimestampHandler(oidSysUptime, &sysUptime);
  // Add Settable Handlers
  //snmp->addReadWriteStringHandler(oidSysContact, &sysContact, 25, true);
  //snmp->addReadWriteStringHandler(oidSysName, &sysName, 25, true);
  //snmp->addReadWriteStringHandler(oidSysLocation, &sysLocation, 25, true);
}


//-----------------------------------------------------------------------------------------------

int snmpInit() {

  unsigned int i=0;
  
  if (udpServerSNMP == nullptr) {
    udpServerSNMP = new EthernetUDP();
    Serial.println("Creating UDP server for SNMP from EthernetUDP.");
    }

  delay (200);    
  udpServerSNMP->begin(161);   // SNMP / UDP area
  delay (200); 

  if (snmp == nullptr) {
    snmp = new SNMPAgent(pconfig.snmpROCommunity.c_str(), pconfig.snmpWRCommunity.c_str());
    snmp->setUDP(udpServerSNMP);   // give snmp a pointer to the UDP object
    snmp->begin();
    Serial.println("SNMP Agent started.");
    }

  // add 'callback' for an OID - pointer to an integer
  //changingNumberOID = snmp->addIntegerHandler(".1.3.6.1.2.1.99.1.1.1.9.1", &changingNumber);
  addRFC1213MIBHandler();      // RFC1213-MIB (System)

  // init variables for SNMP handling
  for (i=0 ; i<VAR_COUNT ; i++)
      variableInit( & ( vars[i] ), snmp);
  
  // Ensure to sortHandlers after adding/removing and OID callbacks - this makes snmpwalk work
  snmp->sortHandlers();

  return(0);
  }

//-----------------------------------------------------------------------------------------------

int   processSNMP() {

  snmp->loop();

  if (vars[VAR_AA1_FORCED].snmpCallback->setOccurred) {
    Serial.print("Variable changed: ");
    Serial.println(vars[VAR_AA1_FORCED].name);
    __coils[COIL_VAR_AA1_FORCED] = (vars[VAR_AA1_FORCED].value > 0) ? 1 : 0;
    vars[VAR_AA1_FORCED].snmpCallback->resetSetOccurred();
    }

  if (vars[VAR_AA2_FORCED].snmpCallback->setOccurred) {
    Serial.print("Variable changed: ");
    Serial.println(vars[VAR_AA2_FORCED].name);
    __coils[COIL_VAR_AA2_FORCED] = (vars[VAR_AA2_FORCED].value > 0) ? 1 : 0;
    vars[VAR_AA2_FORCED].snmpCallback->resetSetOccurred();
    }

  // restart SNMP server every    'pconfig.snmpRestartMinutes'  if configured
  if (pconfig.snmpRestartMinutes > 0)  {   // time to reset and restart udp server  for SNMP.  zero ->  no restart
    static long int lastSNMPRestart = 0;
    if (millis() - lastSNMPRestart >= ( pconfig.snmpRestartMinutes * 60UL * 1000UL )) {  // every X minute -> restart UDP for SNMP
      Serial.println(F("Restarting SNMP UDP"));
      udpServerSNMP->stop();   // SNMP / UDP area
      delay (100); 
      udpServerSNMP->begin(161);   // SNMP / UDP area
      lastSNMPRestart = millis();
      }
    }  

  return(0);
}

// ================================================================================================
// ================================================================================================
