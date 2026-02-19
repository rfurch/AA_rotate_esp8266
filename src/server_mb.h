
#ifndef MY_HEADER_FILE_H // Replace MY_HEADER_FILE_H with a unique identifier for your header
#define MY_HEADER_FILE_H

#endif // MY_HEADER_FILE_H

//-----------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------

extern bool __coils[];
extern uint16_t __holdingRegs[];
extern unsigned long clientLastActivity[];

extern EthernetClient clients[];
extern unsigned long clientLastActivity[];

extern EthernetServer modbusServer;

//extern unsigned long MODBUS_CLIENT_TIMEOUT;

//-----------------------------------------------------------------------------------------------

// ---------- UTILIDADES CLIENTES ----------

int clientAlreadyRegistered (EthernetClient newClient) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if ( clients[i] == newClient )
      return(1);
   }  
  return(0);
} 

//-----------------------------------------------------------------------------------------------

int usedSlotsCount() {
  int n = 0;
  for (int i = 0; i < MAX_CLIENTS; i++) 
    if (clients[i] && clients[i].connected())
      n++;
  return (n);
}

//-----------------------------------------------------------------------------------------------

int findFreeClientSlot() {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (!clients[i] || !clients[i].connected()) {
      // asegurar que socket viejo esté cerrado
      if (clients[i]) clients[i].stop();
      return i;
    }
  }
  return -1;
}

//-----------------------------------------------------------------------------------------------

void closeClientSlot(int i) {
  if (clients[i]) {
    clients[i].stop();
  }
  clientLastActivity[i] = 0;
}

//-----------------------------------------------------------------------------------------------

#if defined(ESP32)
uint64_t uptimeMillis() {
    return (esp_timer_get_time() / 1000);
}
#else
uint64_t uptimeMillis() {
    // https://arduino.stackexchange.com/questions/12587/how-can-i-handle-the-millis-rollover
    static uint32_t low32, high32;
    uint32_t new_low32 = millis();
    if (new_low32 < low32)
        high32++;
    low32 = new_low32;
    return (uint64_t)high32 << 32 | low32;
}
#endif




//-----------------------------------------------------------------------------------------------
// ---------- PROTOS ---------
bool getCoil(uint16_t addr) {
  if (addr >= MAX_COILS) return false;
  return __coils[addr];
}

//-----------------------------------------------------------------------------------------------

void setCoil(uint16_t addr, bool val) {

  int intVal = 0;
  
  addr++;   // our addresses start in '1', not '0'

  if (addr >= MAX_COILS) return;
  __coils[addr] = val;

  if (val) 
    intVal = 1;

  if ( addr == COIL_VAR_AA1_FORCED )  
    vars[VAR_AA1_FORCED].value  = intVal;
  else if ( addr == COIL_VAR_AA2_FORCED )  
    vars[VAR_AA2_FORCED].value  = intVal;

  }
 
//-----------------------------------------------------------------------------------------------

// Tu función processModbus, adaptada para usar arrays globales y clientes
void processModbus(EthernetClient &client, int slotIndex) {
  uint8_t buffer[260];
  int len = 0;

  // Leer todo lo disponible (hasta 260 bytes)
  unsigned long start = millis();
  // dejamos un pequeño timeout de lectura para asegurar que llegue trama completa
  while (client.available() && len < (int)sizeof(buffer)) {
    buffer[len++] = client.read();
    // evitar bloqueos eternos
    if (millis() - start > 50) break;
  }

  if (len < 12) return;  // Trama muy corta

  // Actualizar tiempo de actividad al recibir datos
  clientLastActivity[slotIndex] = millis();

  // Parsear cabecera Modbus TCP
  //uint16_t transID = (buffer[0] << 8) | buffer[1];
  uint16_t protoID = (buffer[2] << 8) | buffer[3];
  uint8_t unitID = buffer[6];
  uint8_t funcCode = buffer[7];
  uint16_t regAddr = (buffer[8] << 8) | buffer[9];
  uint16_t regCount = (buffer[10] << 8) | buffer[11];

  if (protoID != 0) return;  // No es Modbus TCP

  uint8_t response[260];
  int respLen = 0;

  // Función 1: Read Coils
  if (funcCode == 0x01) {
    if ((uint32_t)regAddr + (uint32_t)regCount > MAX_COILS) {
      // Exception: Illegal Data Address (0x02)
      response[respLen++] = buffer[0];
      response[respLen++] = buffer[1];
      response[respLen++] = 0x00;
      response[respLen++] = 0x00;
      response[respLen++] = 0x00;
      response[respLen++] = 0x03;
      response[respLen++] = unitID;
      response[respLen++] = funcCode | 0x80;
      response[respLen++] = 0x02;
    } else {
      uint8_t byteCount = (regCount + 7) / 8;

      response[respLen++] = buffer[0];  // Transaction ID hi
      response[respLen++] = buffer[1];  // Transaction ID lo
      response[respLen++] = 0x00;  // Protocol ID hi
      response[respLen++] = 0x00;  // Protocol ID lo
      uint16_t dataLen = 3 + byteCount;
      response[respLen++] = (dataLen >> 8) & 0xFF;
      response[respLen++] = dataLen & 0xFF;
      response[respLen++] = unitID;
      response[respLen++] = funcCode;
      response[respLen++] = byteCount;

      // Empaquetar coils en bytes
      for (int i = 0; i < byteCount; i++) {
        uint8_t byteVal = 0;
        for (int bit = 0; bit < 8; bit++) {
          uint32_t coilAddr = (uint32_t)regAddr + (i * 8) + bit;
          if (coilAddr < (uint32_t)regAddr + regCount) {
            if (getCoil(coilAddr)) {
              byteVal |= (1 << bit);
            }
          }
        }
        response[respLen++] = byteVal;
      }
    }
  }

  // Función 5: Write Single Coil
  else if (funcCode == 0x05) {
    if (len < 12) return;
    uint16_t value = (buffer[10] << 8) | buffer[11];

    if (regAddr >= MAX_COILS) {
      // Error
      response[respLen++] = buffer[0];
      response[respLen++] = buffer[1];
      response[respLen++] = 0x00;
      response[respLen++] = 0x00;
      response[respLen++] = 0x00;
      response[respLen++] = 0x03;
      response[respLen++] = unitID;
      response[respLen++] = funcCode | 0x80;
      response[respLen++] = 0x02;
    } else {
      if (value == 0xFF00) setCoil(regAddr, true);
      else setCoil(regAddr, false);

      Serial.print("Write Single Coil: Addr=");
      Serial.print(regAddr);
      Serial.print(" Value=");
      Serial.println(value == 0xFF00 ? "ON" : "OFF");

      // Echo de la petición (12 bytes)
      for (int i = 0; i < 12; i++) response[respLen++] = buffer[i];
    }
  }

  // Función 15: Write Multiple Coils
  else if (funcCode == 0x0F && len >= 13) {
    uint8_t byteCount = buffer[12];

    if ((uint32_t)regAddr + (uint32_t)regCount > MAX_COILS || len < 13 + byteCount) {
      // Error
      response[respLen++] = buffer[0];
      response[respLen++] = buffer[1];
      response[respLen++] = 0x00;
      response[respLen++] = 0x00;
      response[respLen++] = 0x00;
      response[respLen++] = 0x03;
      response[respLen++] = unitID;
      response[respLen++] = funcCode | 0x80;
      response[respLen++] = 0x02;
    } else {
      for (int i = 0; i < regCount; i++) {
        uint16_t byteIndex = i / 8;
        uint8_t bitIndex = i % 8;
        bool bitValue = (buffer[13 + byteIndex] >> bitIndex) & 0x01;
        setCoil(regAddr + i, bitValue);
      }

      Serial.print("Write Multiple Coils: Addr=");
      Serial.print(regAddr);
      Serial.print(" Count=");
      Serial.println(regCount);

      // Respuesta (12 bytes echo header)
      for (int i = 0; i < 12; i++) response[respLen++] = buffer[i];
    }
  }

  // Función 3: Read Holding Registers
  else if (funcCode == 0x03) {
    if ((uint32_t)regAddr + (uint32_t)regCount > MAX_HREGS) {
      // Error: direccion ilegal
      response[respLen++] = buffer[0];
      response[respLen++] = buffer[1];
      response[respLen++] = 0x00;
      response[respLen++] = 0x00;
      response[respLen++] = 0x00;
      response[respLen++] = 0x03;
      response[respLen++] = unitID;
      response[respLen++] = funcCode | 0x80;
      response[respLen++] = 0x02;
    } else {
      response[respLen++] = buffer[0];
      response[respLen++] = buffer[1];
      response[respLen++] = 0x00;
      response[respLen++] = 0x00;
      uint16_t dataLen = 3 + (regCount * 2);
      response[respLen++] = (dataLen >> 8) & 0xFF;
      response[respLen++] = dataLen & 0xFF;
      response[respLen++] = unitID;
      response[respLen++] = funcCode;
      response[respLen++] = regCount * 2;

      for (int i = 0; i < regCount; i++) {
        uint16_t val = __holdingRegs[regAddr + i];
        response[respLen++] = (val >> 8) & 0xFF;
        response[respLen++] = val & 0xFF;
      }
    }
  }

  // Función 6: Write Single Register
  else if (funcCode == 0x06) {
    if (len < 12) return;
    uint16_t value = (buffer[10] << 8) | buffer[11];

    if (regAddr >= MAX_HREGS) {
      // Error
      response[respLen++] = buffer[0];
      response[respLen++] = buffer[1];
      response[respLen++] = 0x00;
      response[respLen++] = 0x00;
      response[respLen++] = 0x00;
      response[respLen++] = 0x03;
      response[respLen++] = unitID;
      response[respLen++] = funcCode | 0x80;
      response[respLen++] = 0x02;
    } else {
      __holdingRegs[regAddr] = value;

      Serial.print("Write Single Register: Addr=");
      Serial.print(regAddr);
      Serial.print(" Value=");
      Serial.println(value);

      // Echo 12 bytes
      for (int i = 0; i < 12; i++) response[respLen++] = buffer[i];
    }
  }

  // Función 16: Write Multiple Registers
  else if (funcCode == 0x10 && len >= 13) {
    uint8_t byteCount = buffer[12];

    if ((uint32_t)regAddr + (uint32_t)regCount > MAX_HREGS || len < 13 + byteCount) {
      // Error
      response[respLen++] = buffer[0];
      response[respLen++] = buffer[1];
      response[respLen++] = 0x00;
      response[respLen++] = 0x00;
      response[respLen++] = 0x00;
      response[respLen++] = 0x03;
      response[respLen++] = unitID;
      response[respLen++] = funcCode | 0x80;
      response[respLen++] = 0x02;
    } else {
      for (int i = 0; i < regCount; i++) {
        uint16_t val = (buffer[13 + i * 2] << 8) | buffer[13 + i * 2 + 1];
        __holdingRegs[regAddr + i] = val;
      }

      Serial.print("Write Multiple Registers: Addr=");
      Serial.print(regAddr);
      Serial.print(" Count=");
      Serial.println(regCount);

      // Respuesta (12 bytes echo)
      for (int i = 0; i < 12; i++) response[respLen++] = buffer[i];
    }
  }

  // Si hay respuesta, enviarla
  if (respLen > 0) {
    client.write(response, respLen);
    // no forzamos client.flush() para evitar bloqueos; W5500 manda en background
    clientLastActivity[slotIndex] = millis();
  }
}

//-----------------------------------------------------------------------------------------------

int processModbusTCP() {

  EthernetClient newClient = modbusServer.available();

  if (newClient) {
    if (  clientAlreadyRegistered (newClient) )
      {
      //Serial.println("client already present! ");
      } 
    else {
    
      int slot = findFreeClientSlot();
      if (slot >= 0) {
        clients[slot] = newClient;
        clientLastActivity[slot] = millis();
        Serial.print("Cliente nuevo en slot ");
        Serial.println(slot);
        } 
      else {
        // No hay slot libre: rechazamos (recordar: W5500 tiene 8 sockets)
        Serial.println("No hay slot libre — rechazando cliente");
        newClient.stop();
        }
      }  
    }

  // Procesar clientes activos
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (!clients[i]) continue;

    if (!clients[i].connected()) {
      // cerramos si ya desconectado
      closeClientSlot(i);
      continue;
      }

    // Si hay datos disponibles procesarlos
    if (clients[i].available()) {
      processModbus(clients[i], i);
      }

    // Timeout por inactividad
    if (clientLastActivity[i] && (millis() - clientLastActivity[i] > MODBUS_CLIENT_TIMEOUT)) {
      Serial.print("Cliente timeout en slot ");
      Serial.println(i);
      closeClientSlot(i);
      }
    }
  return(0);  
  }



//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
