/*

TODO: force AA via SNMP and MODBUS
set alarms
consider UDP LOG
change update times in NTP and MQTT if no connection

*/

// Global Variables declareation  
#define UPTIME_UPDATE_INTERVAL     3000 // ms = 1 second
#define SENSOR_UPDATE_INTERVAL     5000 // ms = 5 Seconds

#define NTP_CYCLE_IF_LAST_OK       1800 // every 30 minutes
#define NTP_CYCLE_IF_LAST_NOT_OK   7200 // every  2 hours 
#define NTP_MAX_RESPONSE_T_ms      1500 // 1.5 seconds.  if response time is slower,  NTP_CYCLE_IF_LAST_NOT_OK apply

#define MQTT_CYCLE_IF_LAST_OK      30   // every 30 seconds
#define MQTT_CYCLE_IF_LAST_NOT_OK  1800 // every  30 minutes 
#define MQTT_MAX_RESPONSE_T_ms     1500 // 1.5 seconds.  if response time is slower,  MQTT_CYCLE_IF_LAST_NOT_OK apply

#define CYCLE_HOURS_MIN            6
#define CYCLE_HOURS_MAX            240  // 10 days

#define SLOW_VARS_CYCLEPCUNTER    1000  // we evaluate variables evry execution 1000 cycles 

#define MIN_TEMP_CHECK_TIME       3     // check temperature every 3 minutes, no action taken in between   

#define TEMP_THRESHOLD_LOW        0
#define TEMP_THRESHOLD_HIGH       5000  // temp * 100  ->  t = 50 celsius
#define TEMP_ERROR                111

#define MAX_MQTT_TAG_LEN          100



// ---------- CONFIG RED ----------

#define SPI_CLK     13
#define SPI_MISO    12 
#define SPI_MOSI    14
#define SPI_CS      4

#define W5500_CS_PIN 4 // Ajustar según tu placa

// --- DS18S20 setup ---
//#define ONE_WIRE_BUS 0   // GPIO = D3 on NodeMCU
#define ONE_WIRE_BUS 02   // GPI2 = D4 on NodeMCU

// ----------------------- Modbus
#define MODBUS_CLIENT_TIMEOUT 120000 // 120s timeout inactividad

#define NTP_PORT 123

// variables 
extern persistConfig pconfig; 

extern EthernetServer modbusServer;
extern EthernetClient clients[];
extern unsigned long clientLastActivity[];

extern bool __coils[];
extern uint16_t __holdingRegs[];

extern uint32_t sysUptime;

extern int changingNumber;
extern int settableNumber;

//extern NTPClient *timeClient;

extern ValueCallback *settableNumberOID;


#define RELAY_PIN_1 16  // GPIO16
#define RELAY_PIN_2 5  // GPIO2

// limits and constant declaration 

#define MAX_COILS 2000     // según tu chequeo anterior
#define MAX_HREGS 125      // limite modbus read holding reg (por tu chequeo)

#define MODBUS_PORT 502
#define MAX_CLIENTS 8   // límite por W5500. Cambia sólo si tienes más chips W5500.


uint64_t uptimeMillis();
int variablesUpdate();

//-----------------------------------------------------------------------------------------------

int   initPins(persistConfig &pconfig) { 

  Serial.println("\n\n=== Modbus TCP Server - W5500 (Ethernet_Generic) ===");
  Serial.println("CS Pin: D2 (GPIO4)");
  
  // inicializar arrays
  memset(__coils, 0, MAX_COILS * sizeof(bool));
  for (int i = 0; i < MAX_HREGS; i++) __holdingRegs[i] = 0;
  for (int i = 0; i < MAX_CLIENTS; i++) clientLastActivity[i] = 0;
  
  Serial.println("Inicializando SPI...");
  // Configurar CS
  pinMode(RELAY_PIN_1, OUTPUT);
  digitalWrite(RELAY_PIN_1, HIGH); // Turn relay ON
  pinMode(RELAY_PIN_2, OUTPUT);
  digitalWrite(RELAY_PIN_2, HIGH); // Turn relay ON
  return(0);
}

//-----------------------------------------------------------------------------------------------

int getUptime() {
    return (int)(uptimeMillis() / 10); // Convert milliseconds to timeticks (hundredths of a second)
}  

//-----------------------------------------------------------------------------------------------


