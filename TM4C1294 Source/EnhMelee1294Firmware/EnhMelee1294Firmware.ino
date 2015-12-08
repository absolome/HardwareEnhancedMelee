#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <ArduinoJson.h>

#include "SSI3DMASlave.h"
#include "enhmelee.h"
#include "Flash.h"

//**********************************************************************
//*                         ASM Event Codes
//**********************************************************************
#define EVENT_GAME_START 0x37
#define EVENT_UPDATE 0x38
#define EVENT_GAME_END 0x39

int asmEvents[256];

void asmEventsInitialize() {
  asmEvents[EVENT_GAME_START] = 0xA;
  asmEvents[EVENT_UPDATE] = 0x7A;
  asmEvents[EVENT_GAME_END] = 0x1;
}

//**********************************************************************
//*               SPI Slave Communication Functions
//**********************************************************************
RfifoMessage Msg; //Keep an RfifoMessage variable as a global variable to prevent memory leak

void spiSlaveInitialize() {
  SSI3DMASlave.begin();
}

void spiReadMessage() {
  Msg = { false, 0, 0, 0, 0 };
  
  //Wait until a message is available
  while (!SSI3DMASlave.isMessageAvailable()) ethernetCheckConnection();
  
  uint32_t messageSize = SSI3DMASlave.getMessageSize();
  uint8_t* bytes = SSI3DMASlave.popMessage();
  
  Msg.eventCode = bytes[0];
  
  //Copy all bytes from receive buffer to msg
  for (int i = 1; i < messageSize; i++) Msg.data[i-1] = bytes[i];
  
  Msg.messageSize = messageSize - 1;
  
  //If message size does not match expected size, return without flagging success
  if (Msg.messageSize != asmEvents[Msg.eventCode]) return;
  
  Msg.success = true; 
}

//**********************************************************************
//*                         Event Handlers
//**********************************************************************
Game CurrentGame = { };

//The read operators will read a value and increment the index so the next read will read in the correct location
uint8_t readByte(uint8_t* a, int& idx) {
  return a[idx++];
}

uint16_t readHalf(uint8_t* a, int& idx) {
  uint16_t value = a[idx] << 8 | a[idx + 1];
  idx += 2;
  return value;
}

uint32_t readWord(uint8_t* a, int& idx) {
  uint32_t value = a[idx] << 24 | a[idx + 1] << 16 | a[idx + 2] << 8 | a[idx + 3];
  idx += 4;
  return value;
}

float readFloat(uint8_t* a, int& idx) {
  uint32_t bytes = readWord(a, idx);
  return *(float*)(&bytes);
}

void handleGameStart() {
  uint8_t* data = Msg.data;
  int idx = 0;
  
  //Reset CurrentGame variable
  CurrentGame = { };
  
  //Load stage ID
  CurrentGame.stage = readHalf(data, idx);

  for (int i = 0; i < PLAYER_COUNT; i++) {
    Player& p = CurrentGame.players[i];
    
    //Load player data
    p.controllerPort = readByte(data, idx);
    p.characterId = readByte(data, idx);
    p.playerType = readByte(data, idx);
    p.characterColor = readByte(data, idx);
  }
}

void handleUpdate() {
  uint8_t* data = Msg.data;
  int idx = 0;
  
  //Check frame count and see if any frames were skipped
  uint32_t frameCount = readWord(data, idx);
  int framesMissed = frameCount - CurrentGame.frameCounter - 1;
  CurrentGame.framesMissed += framesMissed;
  CurrentGame.frameCounter = frameCount;

  CurrentGame.randomSeed = readWord(data, idx);

  for (int i = 0; i < PLAYER_COUNT; i++) {
    Player& p = CurrentGame.players[i];

    //Change over previous frame data
    p.previousFrameData = p.currentFrameData;

    PlayerFrameData& pfd = p.currentFrameData;

    //Load player data
    pfd = { };
    pfd.internalCharacterId = readByte(data, idx);
    pfd.animation = readHalf(data, idx);
    pfd.locationX = readFloat(data, idx);
    pfd.locationY = readFloat(data, idx);

    //Controller information
    pfd.joystickX = readFloat(data, idx);
    pfd.joystickY = readFloat(data, idx);
    pfd.cstickX = readFloat(data, idx);
    pfd.cstickY = readFloat(data, idx);
    pfd.trigger = readFloat(data, idx);
    pfd.buttons = readWord(data, idx);

    //More data
    pfd.percent = readFloat(data, idx);
    pfd.shieldSize = readFloat(data, idx);
    pfd.lastMoveHitId = readByte(data, idx);
    pfd.comboCount = readByte(data, idx);
    pfd.lastHitBy = readByte(data, idx);
    pfd.stocks = readByte(data, idx);

    //Raw controller information
    pfd.physicalButtons = readHalf(data, idx);
    pfd.lTrigger = readFloat(data, idx);
    pfd.rTrigger = readFloat(data, idx);
  }
}

void handleGameEnd() {
  uint8_t* data = Msg.data;
  int idx = 0;
  
  CurrentGame.winCondition = readByte(data, idx);
}

//**********************************************************************
//*                        Ethernet
//**********************************************************************
#define DELAY_TIME 10000
#define UDP_MAX_PACKET_SIZE 100

// the media access control (ethernet hardware) address for the shield:
byte mac[] = { 0x00, 0x1A, 0xB6, 0x02, 0xF5, 0x8C };
IPAddress server(192, 168, 0, 3);
int port = 3636;
IPAddress debugServer(192, 168, 0, 3);
int udpPort = 3637;

char udpPacketBuffer[UDP_MAX_PACKET_SIZE];
long lastReconnectMs = 0;

EthernetClient client;
EthernetUDP udp;

void ethernetInitialize() {
  if (lastReconnectMs == 0 || millis() - lastReconnectMs > DELAY_TIME) {
    debugPrintln("Attempting to obtain IP address from DHCP");
    
    // start the Ethernet connection:
    if (Ethernet.begin(mac) == 0) {
      debugPrintln("Failed to configure Ethernet using DHCP");
      return;
    }
  
    debugPrint("Obtained IP address: "); debugPrintln(String(Ethernet.localIP()));
  
    udpConnect();
    ethernetConnect();
  
    lastReconnectMs = millis();
  }
}

void udpConnect() {
    int code = udp.begin(udpPort);
    if (code) {
      debugPrintln("UDP successfully started!");
      postConnectedMessage();
    } else {
      debugPrint("Failed to start UDP. Code: "); debugPrintln(String(code));
    }
}

void ethernetConnect() {
  if (lastReconnectMs == 0 || millis() - lastReconnectMs > DELAY_TIME) {
    debugPrint("Trying to connect to: "); debugPrintln(String(server));
    int code = client.connect(server, port);
    if (code) {
      debugPrintln("Connected to server!");
      postConnectedMessage();
    } else {
      debugPrint("Failed to connect to server. Code: "); debugPrintln(String(code));
    }
    
    lastReconnectMs = millis();
  }
}

int ethernetCheckConnection() {
  //Check if UDP packet has been received
  int packetSize = udp.parsePacket();
  if (packetSize) {
    handleReceiveUdp(packetSize);
  }
  
  //Checks if connection is working. If it isn't, attempt to reconnect.
  if(!client) {
    client.stop();
    ethernetInitialize();
    return -1;
  }

  //Check if we are connected to a server. If not, attempt to reconnect.
  if (!client.connected()) {
    client.stop();
    Ethernet.maintain();
    ethernetConnect();
    return -2;
  }
  
  Ethernet.maintain();
  return 1;
}

void handleReceiveUdp(int size) {
  if (size > UDP_MAX_PACKET_SIZE) return;
  
  //Get IP and port of the UDP sender
  IPAddress sourceIp = udp.remoteIP();
  int sourcePort = udp.remotePort();
  
  //Read json from udp packet
  udp.read(udpPacketBuffer, UDP_MAX_PACKET_SIZE);
  
  StaticJsonBuffer<UDP_MAX_PACKET_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(udpPacketBuffer);
  
  //Parse json for command
  int command = root["command"];
  debugPrintln("Received UDP packet with command: " + String(command));
  
  //Handle command received
  switch(command) {
    case 1:
      StaticJsonBuffer<200> jsonBuffer;
  
      JsonObject& resp = jsonBuffer.createObject();
      JsonArray& macBytes = resp.createNestedArray("mac");
      for (int i = 0; i < sizeof(mac); i++) macBytes.add(mac[i]);
    
      resp["ip"] = String(Ethernet.localIP());
      resp["port"] = String(udpPort);
      
      udp.beginPacket(sourceIp, sourcePort);
      resp.printTo(udp);
      udp.endPacket();
      
      break;
  }     
}

//**********************************************************************
//*                           JSON
//**********************************************************************
void postMatchParameters() {
  if (client.connected()) {
    //Create JSON
    StaticJsonBuffer<400> jsonBuffer;
  
    JsonObject& root = jsonBuffer.createObject();
    root["stage"] = CurrentGame.stage;
  
    JsonArray& data = root.createNestedArray("players");
    for (int i = 0; i < PLAYER_COUNT; i++) {
      Player* p = &CurrentGame.players[i];
      JsonObject& item = jsonBuffer.createObject();
      
      item["port"] = p->controllerPort + 1;
      item["character"] = p->characterId;
      item["color"] = p->characterColor;
      item["type"] = p->playerType;
  
      data.add(item);
    }
  
    root.printTo(client);
    client.println();
  }
}

void postConnectedMessage() {
  if (client.connected()) {    
    //Create JSON
    StaticJsonBuffer<200> jsonBuffer;
  
    JsonObject& root = jsonBuffer.createObject();
    JsonArray& macBytes = root.createNestedArray("mac");
    for (int i = 0; i < sizeof(mac); i++) macBytes.add(mac[i]);
  
    root.printTo(client);
    client.println();
  }
}

void postGameEndMessage() {
  if (client.connected()) {
    StaticJsonBuffer<10000> jsonBuffer;
    
    JsonObject& root = jsonBuffer.createObject();
    root["frames"] = CurrentGame.frameCounter;
    root["framesMissed"] = CurrentGame.framesMissed;
    root["winCondition"] = CurrentGame.winCondition;

    float totalActiveGameFrames = float(CurrentGame.frameCounter);
    
    JsonArray& data = root.createNestedArray("players");
    for (int i = 0; i < PLAYER_COUNT; i++) {
      //debugPrintln(String("Writing out player ") + i);
      PlayerStatistics& ps = CurrentGame.players[i].stats;
      JsonObject& item = jsonBuffer.createObject();

      item["stocksRemaining"] = CurrentGame.players[i].currentFrameData.stocks;
      
      item["apm"] = 3600 * (ps.actionCount / totalActiveGameFrames);
      
      item["averageDistanceFromCenter"] = ps.averageDistanceFromCenter;
      item["percentTimeClosestCenter"] = 100 * (ps.framesClosestCenter / totalActiveGameFrames);
      item["percentTimeAboveOthers"] = 100 * (ps.framesAboveOthers / totalActiveGameFrames);
      item["percentTimeInShield"] = 100 * (ps.framesInShield / totalActiveGameFrames);
      item["secondsWithoutDamage"] = float(ps.mostFramesWithoutDamage) / 60;

      item["rollCount"] = ps.rollCount;
      item["spotDodgeCount"] = ps.spotDodgeCount;
      item["airDodgeCount"] = ps.airDodgeCount;
      
      item["recoveryAttempts"] = ps.recoveryAttempts;
      item["successfulRecoveries"] = ps.successfulRecoveries;
      item["edgeguardChances"] = ps.edgeguardChances;
      item["edgeguardConversions"] = ps.edgeguardConversions;
      
      //Combo string stuff
      item["numberOfOpenings"] = ps.numberOfOpenings;
      item["averageDamagePerString"] = ps.averageDamagePerString;
      item["averageTimePerString"] = ps.averageTimePerString;
      item["averageHitsPerString"] = ps.averageHitsPerString;
      item["mostDamageString"] = ps.mostDamageString;
      item["mostTimeString"] = ps.mostTimeString;
      item["mostHitsString"] = ps.mostHitsString;
      
      JsonArray& stocks = item.createNestedArray("stocks");
      for (int j = 0; j < STOCK_COUNT; j++) {
        //debugPrintln(String("Writing out player ") + i + String(". Stock: ") + j);
        StockStatistics& ss = ps.stocks[j];
        
        //Only log the stock if the player actually played that stock
        if (ss.isStockUsed) {
          //debugPrintln("Stock used.");
          JsonObject& stock = jsonBuffer.createObject();
        
          uint32_t stockFrames = ss.frame;
          if (j > 0) stockFrames -= ps.stocks[j - 1].frame;
          
          stock["timeSeconds"] = float(stockFrames) / 60; 
          stock["percent"] = ss.percent;
          stock["moveLastHitBy"] = ss.lastHitBy;
          stock["lastAnimation"] = ss.lastAnimation;
          stock["openingsAllowed"] = ss.killedInOpenings;
          stock["isStockLost"] = ss.isStockLost;
        
          stocks.add(stock);
        }
      }
      
      data.add(item);
    }

    root.printTo(client);
    client.println();
  }
}

//**********************************************************************
//*                            Statistics
//**********************************************************************
int numberOfSetBits(uint16_t x) {
  //This function solves the Hamming Weight problem. Effectively it counts the number of bits in the input that are set to 1
  //This implementation is supposedly very efficient when most bits are zero. Found: https://en.wikipedia.org/wiki/Hamming_weight#Efficient_implementation
  int count;
  for (count=0; x; count++) x &= x-1;
  return count;
}

void computeStatistics() {
  //this function will only get called when frameCount >= 1
  uint32_t framesSinceStart = CurrentGame.frameCounter - 1;
  
  Player* p = CurrentGame.players;
  
  float p1CenterDistance = sqrt(pow(p[0].currentFrameData.locationX, 2) + pow(p[0].currentFrameData.locationY, 2));
  float p2CenterDistance = sqrt(pow(p[1].currentFrameData.locationX, 2) + pow(p[1].currentFrameData.locationY, 2));
  
  p[0].stats.averageDistanceFromCenter = (framesSinceStart*p[0].stats.averageDistanceFromCenter + p1CenterDistance) / (framesSinceStart + 1);
  p[1].stats.averageDistanceFromCenter = (framesSinceStart*p[1].stats.averageDistanceFromCenter + p2CenterDistance) / (framesSinceStart + 1);
  
  //Increment frame counter of person who is closest to center. If the players are even distances from the center, do not increment
  if (p1CenterDistance < p2CenterDistance) p[0].stats.framesClosestCenter++;
  else if (p2CenterDistance < p1CenterDistance) p[1].stats.framesClosestCenter++;

  //Increment frame counter of person who is highest;
  if (p[0].currentFrameData.locationY > p[1].currentFrameData.locationY) p[0].stats.framesAboveOthers++;
  else if (p[1].currentFrameData.locationY > p[0].currentFrameData.locationY) p[1].stats.framesAboveOthers++;
  
  for (int i = 0; i < PLAYER_COUNT; i++) {
    Player& cp = p[i]; //Current player
    Player& op = p[!i]; //Other player
    
    bool lostStock = cp.previousFrameData.stocks - cp.currentFrameData.stocks > 0;
    bool opntLostStock = op.previousFrameData.stocks - op.currentFrameData.stocks > 0;
    
    //Check current action states, although many of these conditions check previous frame data, it shouldn't matter for frame = 1 that there is no previous
    if (cp.currentFrameData.animation >= GUARD_START && cp.currentFrameData.animation <= GUARD_END) cp.stats.framesInShield++;
    else if ((cp.currentFrameData.animation == ROLL_FORWARD && cp.previousFrameData.animation != ROLL_FORWARD) ||
             (cp.currentFrameData.animation == ROLL_BACKWARD && cp.previousFrameData.animation != ROLL_BACKWARD)) cp.stats.rollCount++;
    else if (cp.currentFrameData.animation == SPOT_DODGE && cp.previousFrameData.animation != SPOT_DODGE) cp.stats.spotDodgeCount++;
    else if (cp.currentFrameData.animation == AIR_DODGE && cp.previousFrameData.animation != AIR_DODGE) cp.stats.airDodgeCount++;
    
    //Check if we are getting damaged
    bool tookPercent = cp.currentFrameData.percent - cp.previousFrameData.percent > 0;
    if (tookPercent) {
      cp.flags.framesWithoutDamage = 0;
    } else {
      cp.flags.framesWithoutDamage++; //Increment count of frames without taking damage

      //If frames without being hit is greater than previous, set new record
      if (cp.flags.framesWithoutDamage > cp.stats.mostFramesWithoutDamage) cp.stats.mostFramesWithoutDamage = cp.flags.framesWithoutDamage; 
    }
    
    //------------------------------- Monitor Combo Strings -----------------------------------------
    bool opntTookDamage = op.currentFrameData.percent - op.previousFrameData.percent > 0;
    bool opntDamagedState = op.currentFrameData.animation >= DAMAGE_START && op.currentFrameData.animation <= DAMAGE_END;
    bool opntGrabbedState = op.currentFrameData.animation >= CAPTURE_START && op.currentFrameData.animation <= CAPTURE_END;
    bool opntTechState = (op.currentFrameData.animation >= TECH_START && op.currentFrameData.animation <= TECH_END) ||
      op.currentFrameData.animation == TECH_MISS_UP || op.currentFrameData.animation == TECH_MISS_DOWN;

    //By looking for percent changes we can increment counter even when a player gets true combo'd
    //The damage state requirement makes it so things like fox's lasers, grab pummels, pichu damaging self, etc don't increment count
    if (opntTookDamage && (opntDamagedState || opntGrabbedState)) {
      if (cp.flags.stringCount == 0) {
        cp.flags.stringStartPercent = op.previousFrameData.percent;
        cp.flags.stringStartFrame = CurrentGame.frameCounter;
        cp.stats.numberOfOpenings++;
        //debugPrint(String("Player ") + (char)(65 + i)); debugPrintln(" got an opening!");
      }
      
      cp.flags.stringCount++; //increment number of hits
    }

    //Reset combo string counter when somebody dies or doesn't get hit for too long
    if (opntDamagedState || opntGrabbedState || opntTechState) cp.flags.stringResetCounter = 0;
    else if (cp.flags.stringCount > 0) cp.flags.stringResetCounter++;

    //Mark combo completed if opponent lost his stock or if the counter is greater than threshold frames
    if (cp.flags.stringCount > 0 && (opntLostStock || lostStock || cp.flags.stringResetCounter > COMBO_STRING_TIMEOUT)) {
      //Store records
      float percent = op.previousFrameData.percent - cp.flags.stringStartPercent;
      uint32_t frames = CurrentGame.frameCounter - cp.flags.stringStartFrame;
      uint16_t hits = cp.flags.stringCount;
      
      cp.stats.averageDamagePerString = ((cp.stats.numberOfOpenings - 1)*cp.stats.averageDamagePerString + percent) / cp.stats.numberOfOpenings;
      cp.stats.averageTimePerString = ((cp.stats.numberOfOpenings - 1)*cp.stats.averageTimePerString + frames) / cp.stats.numberOfOpenings;
      cp.stats.averageHitsPerString = ((cp.stats.numberOfOpenings - 1)*cp.stats.averageHitsPerString + hits) / cp.stats.numberOfOpenings;
      
      if (percent > cp.stats.mostDamageString) cp.stats.mostDamageString = percent;
      if (frames > cp.stats.mostTimeString) cp.stats.mostTimeString = frames;
      if (hits > cp.stats.mostHitsString) cp.stats.mostHitsString = hits;

      //debugPrint(String("Player ") + (char)(65 + i)); debugPrintln(String(" combo ended. (") + percent + String("%, ") + hits + String(" hits, ") + frames + String(" frames)"));
      
      //Reset string count
      cp.flags.stringCount = 0;
    }
    
    //------------------- Increment Action Count for APM Calculation --------------------------------
    //First count the number of buttons that go from 0 to 1
    uint16_t buttonChanges = (~cp.previousFrameData.physicalButtons & cp.currentFrameData.physicalButtons) & 0xFFF;
    cp.stats.actionCount += numberOfSetBits(buttonChanges); //Increment action count by amount of button presses
    
    //Increment action count when sticks change from one region to another. Don't increment when stick returns to deadzone
    uint8_t prevAnalogRegion = getJoystickRegion(cp.previousFrameData.joystickX, cp.previousFrameData.joystickY);
    uint8_t currentAnalogRegion = getJoystickRegion(cp.currentFrameData.joystickX, cp.currentFrameData.joystickY);
    if ((prevAnalogRegion != currentAnalogRegion) && (currentAnalogRegion != 0)) cp.stats.actionCount++;
    
    //Do the same for c-stick
    uint8_t prevCstickRegion = getJoystickRegion(cp.previousFrameData.cstickX, cp.previousFrameData.cstickY);
    uint8_t currentCstickRegion = getJoystickRegion(cp.currentFrameData.cstickX, cp.currentFrameData.cstickY);
    if ((prevCstickRegion != currentCstickRegion) && (currentCstickRegion != 0)) cp.stats.actionCount++;
    
    //Increment action on analog trigger... I'm not sure when. This needs revision
    if (cp.previousFrameData.lTrigger < 0.3 && cp.currentFrameData.lTrigger >= 0.3) cp.stats.actionCount++;
    if (cp.previousFrameData.rTrigger < 0.3 && cp.currentFrameData.rTrigger >= 0.3) cp.stats.actionCount++;
    
    //--------------------------- Recovery detection --------------------------------------------------
    bool isOffStage = checkIfOffStage(CurrentGame.stage, cp.currentFrameData.locationX, cp.currentFrameData.locationY);
    bool isInControl = cp.currentFrameData.animation >= GROUNDED_CONTROL_START && cp.currentFrameData.animation <= GROUNDED_CONTROL_END;
    bool beingDamaged = cp.currentFrameData.animation >= DAMAGE_START && cp.currentFrameData.animation <= DAMAGE_END;
    bool beingGrabbed = cp.currentFrameData.animation >= CAPTURE_START && cp.currentFrameData.animation <= CAPTURE_END;
    bool isDying = cp.currentFrameData.animation >= DYING_START && cp.currentFrameData.animation <= DYING_END;
    
    if (!cp.flags.isRecovering && !cp.flags.isHitOffStage && beingDamaged && isOffStage) {
      //If player took a hit off stage
      cp.flags.isHitOffStage = true;
      //debugPrint(String("Player ") + (char)(65 + i)); debugPrintln(String(" off stage! (") + cp.currentFrameData.locationX + String(",") + cp.currentFrameData.locationY + String(")"));
    }
    else if (!cp.flags.isRecovering && cp.flags.isHitOffStage && !beingDamaged && !isDying && isOffStage) {
      //If player exited damage state off stage
      cp.flags.isRecovering = true;
      //debugPrint(String("Player ") + (char)(65 + i)); debugPrintln(String(" recovering! (") + String(cp.currentFrameData.animation, HEX) + String(")"));
    }
    else if (!cp.flags.isLandedOnStage && (cp.flags.isRecovering || cp.flags.isHitOffStage) && isInControl && !isOffStage) {
      //If a player is in control of his character after recovering flag as landed
      cp.flags.isLandedOnStage = true;
      //debugPrint(String("Player ") + (char)(65 + i)); debugPrintln(" landed!");
    }
    else if (cp.flags.isLandedOnStage && isOffStage) {
      //If player landed but is sent back off stage, continue recovery process
      cp.flags.framesSinceLanding = 0;
      cp.flags.isLandedOnStage = false;
    }
    else if (cp.flags.isLandedOnStage && !isOffStage && !beingDamaged && !beingGrabbed) {
      //If player landed, is still on stage, is not being hit, and is not grabbed, increment frame counter
      cp.flags.framesSinceLanding++;
      
      //If frame counter while on stage passes threshold, consider it a successful recovery
      if (cp.flags.framesSinceLanding > FRAMES_LANDED_RECOVERY) {
        if (cp.flags.isRecovering) {
          cp.stats.recoveryAttempts++;
          cp.stats.successfulRecoveries++;
          op.stats.edgeguardChances++;
          //debugPrint(String("Player ") + (char)(65 + i)); debugPrintln(" recovered!");
        }
        
        resetRecoveryFlags(cp.flags);
      }
    }
    
    if ((cp.flags.isRecovering || cp.flags.isHitOffStage) && lostStock) {
      //If player dies while recovering, consider it a failed recovery
      if (cp.flags.isRecovering) {
        cp.stats.recoveryAttempts++;
        op.stats.edgeguardChances++;
        op.stats.edgeguardConversions++;
        //debugPrint(String("Player ") + (char)(65 + i)); debugPrintln(" died recovering!");
      }
      else if (cp.flags.isHitOffStage) {
        //debugPrint(String("Player ") + (char)(65 + i)); debugPrintln(" died outright!");
      }
      
      resetRecoveryFlags(cp.flags);
    }
    
    //-------------------------- Stock specific stuff -------------------------------------------------
    int prevStockIndex = STOCK_COUNT - cp.previousFrameData.stocks;
    if (prevStockIndex >= 0 && prevStockIndex < STOCK_COUNT) {
      StockStatistics& s = cp.stats.stocks[prevStockIndex];
      s.isStockUsed = true;
      s.frame = CurrentGame.frameCounter;
      s.percent = cp.currentFrameData.percent;
      s.lastHitBy = op.currentFrameData.lastMoveHitId; //This will indicate what this player was killed by
      s.lastAnimation = cp.currentFrameData.animation; //What was character doing before death
    }
    
    //Mark last stock as lost if lostStock is true
    if (lostStock && prevStockIndex >= 0 && prevStockIndex < STOCK_COUNT) {
      int16_t prevOpenings = 0;
      for (int i = prevStockIndex - 1; i >= 0; i--) prevOpenings += cp.stats.stocks[i].killedInOpenings;
      
      cp.stats.stocks[prevStockIndex].killedInOpenings = op.stats.numberOfOpenings - prevOpenings;
      cp.stats.stocks[prevStockIndex].isStockLost = true;
      
      debugPrint(String("Player ") + (char)(65 + i)); debugPrintln(String(" lost a stock. (") + cp.currentFrameData.animation + String(", ") + cp.previousFrameData.animation + String(")"));
    }
  }
}

//**********************************************************************
//*                             Setup
//**********************************************************************
void checkFlashErase() {
  pinMode(PJ_0, INPUT);
  
  if (digitalRead(PJ_0) == HIGH) {
    debugPrintln("Erasing flash...");
    eraseFlash(); 
    debugPrintln("Flash erased?");
  }
}

void setup() {
  debugPrintln("Starting initialization.");
  
  ethernetInitialize();
  asmEventsInitialize();
  spiSlaveInitialize();
  
  debugPrintln("Initialization complete.");
}

//**********************************************************************
//*                             Debug
//**********************************************************************
void debugPrintMatchParams() {
  debugPrintln(String("Stage: (") + CurrentGame.stage + String(") ") + String(stages[CurrentGame.stage]));
  for (int i = 0; i < PLAYER_COUNT; i++) {
    Player* p = &CurrentGame.players[i];
    debugPrintln(String("Player ") + (char)(65 + i));
    debugPrint("Port: "); debugPrintln(String(p->controllerPort + 1, DEC));
    debugPrintln(String("Character: (") + p->characterId + String(") ") + String(externalCharacterNames[p->characterId]));
    debugPrintln(String("Color: (") + p->characterColor + String(") ") + String(colors[p->characterColor]));
  }
}

void debugPrintGameInfo() {
  if (CurrentGame.frameCounter % 600 == 0) {
    debugPrintln(String("Frame: ") + CurrentGame.frameCounter);
    debugPrintln(String("Frames missed: ") + CurrentGame.framesMissed);
    debugPrint("Random seed: "); debugPrintln(String(CurrentGame.randomSeed, HEX));
    for (int i = 0; i < PLAYER_COUNT; i++) {
      Player* p = &CurrentGame.players[i];
      PlayerFrameData* pfd = &p->currentFrameData;
      debugPrintln(String("Player ") + (char)(65 + i));
      debugPrint("Location X: "); debugPrintln(String(pfd->locationX));
      debugPrint("Location Y: "); debugPrintln(String(pfd->locationY));
      debugPrint("Stocks: "); debugPrintln(String(pfd->stocks));
      debugPrint("Percent: "); debugPrintln(String(pfd->percent));
      debugPrint("Trigger: "); debugPrintln(String(pfd->trigger));
      debugPrint("LTrigger: "); debugPrintln(String(pfd->lTrigger));
      debugPrint("RTrigger: "); debugPrintln(String(pfd->rTrigger));
      debugPrint("PhysButtons: "); debugPrintln(String(pfd->physicalButtons, BIN));
//      debugPrint("Buttons: "); debugPrintln(pfd->buttons, BIN);
//      debugPrint("Joystick: "); debugPrint(pfd->joystickX); debugPrint(","); debugPrintln(pfd->joystickY);
//      debugPrint("Joystick: "); debugPrint(pfd->cstickX); debugPrint(","); debugPrintln(pfd->cstickY);
    }
  }
}

void debugPrintln(String s) {
   debugPrint(s + String("\r\n"));
}

void debugPrint(String s) {
   udp.beginPacket(debugServer, udpPort);
   char buffer[s.length() + 1];
   s.toCharArray(buffer, sizeof(buffer));
   udp.write(buffer);
   udp.endPacket();
}

//**********************************************************************
//*                           Main Loop
//**********************************************************************
void loop() {
  //If ethernet client not working, attempt to re-establish
  if (ethernetCheckConnection()) {
    //read a message from the read fifo - this function doesn't return until data has been read
    spiReadMessage();
    
    if (Msg.success) {
      switch (Msg.eventCode) {
        case EVENT_GAME_START:
          handleGameStart();
          debugPrintMatchParams();
          postMatchParameters();
          break;
        case EVENT_UPDATE:
          handleUpdate();
          //debugPrintGameInfo();
          computeStatistics();
          break;
        case EVENT_GAME_END:
          handleGameEnd();
          postGameEndMessage();
          break;
      }
    } else {
      debugPrintln("Failed to read message.");
    }
  }
}



