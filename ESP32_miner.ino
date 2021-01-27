#include "mbedtls/md.h"

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
    begining:
    Serial.flush(); // Clear serial buffer

    String hash = Serial.readStringUntil('\n');
    String job = Serial.readStringUntil('\n'); // Read job
    byte job1[20];
    unsigned int jobLenght = job.length();
    job.getBytes(job1, jobLenght);
    job.toUpperCase();
    unsigned long diff = Serial.parseInt() + 1; // Read difficulty
    unsigned long StartTime = micros(); // Start time measurement
    byte shaResult[20];

    for (unsigned int iJob = 0; iJob < diff; iJob++) { // Difficulty loop
      yield(); // uncomment if ESP watchdog triggers
      String hash1 = String(hash) + String(iJob);
      const unsigned char* payload = (const unsigned char*) hash1.c_str();
      unsigned int payloadLenght = hash1.length();

      mbedtls_md_context_t ctx;
      mbedtls_md_type_t md_type = MBEDTLS_MD_SHA1;
      mbedtls_md_init(&ctx);
      mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
      mbedtls_md_starts(&ctx);
      mbedtls_md_update(&ctx, payload, payloadLenght);
      mbedtls_md_finish(&ctx, shaResult);
      mbedtls_md_free(&ctx);
      int compareresult = memcmp(shaResult, job1, sizeof(shaResult));

      if (debug == true) {
         Serial.print("diff: ");
         Serial.println(diff);
         Serial.print("job: ");
         Serial.println(job);
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
        goto begining;
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
