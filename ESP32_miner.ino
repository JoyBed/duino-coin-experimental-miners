#include "hwcrypto/sha.h"

#define debug false

void setup() {
  Serial.begin(115200);
  if (Serial.available()) {
    Serial.println("ready"); // Send start word to miner program
  }
}
 
void loop() {
  String startStr = Serial.readStringUntil('\n');
  if (startStr == "start") {
    Serial.flush(); // Clear serial buffer
    String hash = Serial.readStringUntil('\n');
    String job = Serial.readStringUntil('\n'); // Read job
    job.toUpperCase();
    const char * c = job.c_str();
    unsigned char* job1 = hexstr_to_char(c);
    unsigned long diff = Serial.parseInt() + 1; // Read difficulty
    unsigned long StartTime = micros(); // Start time measurement
    byte shaResult[20];

    for (unsigned int iJob = 0; iJob < diff; iJob++) { // Difficulty loop
      yield(); // uncomment if ESP watchdog triggers
      String hash1 = String(hash) + String(iJob);
      const unsigned char* payload = (const unsigned char*) hash1.c_str();
      unsigned int payloadLenght = hash1.length();

      esp_sha(SHA1, payload, payloadLenght, shaResult);
      
      int compareresult = memcmp(shaResult, job1, sizeof(shaResult));

      if (debug == true) {
         Serial.print("diff: ");
         Serial.println(diff);
         Serial.print("job: ");
         Serial.println(job);
         Serial.print("job1: ");
         for(int i=0; i<sizeof(job1); i++){
         printHex(job1[i]);
         }
         Serial.println();
         Serial.print("iJob: ");
         Serial.println(iJob);
         Serial.print("hash: ");
         Serial.println(hash);
         Serial.print("hash1: ");
         Serial.println(hash1);
         Serial.print("result: ");
         for(int i=0; i<sizeof(shaResult); i++){
           printHex(shaResult[i]);
         } 
         Serial.println();
         Serial.print("Compare result: ");
         Serial.println(compareresult);
         Serial.println();
        }
              
      if (compareresult == 0) { // If result is found
        unsigned long EndTime = micros(); // End time measurement
        unsigned long ElapsedTime = EndTime - StartTime; // Calculate elapsed time
        Serial.println(String(iJob) + "," + String(ElapsedTime)); // Send result back to the program with share time
        break; // Stop the loop and wait for more work
      }
    }
  }
}


void printHex(uint8_t num) {
  char hexCar[2];
  sprintf(hexCar, "%02X", num);
  Serial.print(hexCar);
}

unsigned char* hexstr_to_char(const char* hexstr)
{
    size_t len = strlen(hexstr);
    if (len % 2 != 0)
        return NULL;
    size_t final_len = len / 2;
    unsigned char* chrs = (unsigned char*)malloc((final_len + 1) * sizeof(unsigned char));
    for (size_t i = 0, j = 0; j < final_len; i += 2, j++)
        chrs[j] = (hexstr[i] % 32 + 9) % 25 * 16 + (hexstr[i + 1] % 32 + 9) % 25;
    return chrs;
}
