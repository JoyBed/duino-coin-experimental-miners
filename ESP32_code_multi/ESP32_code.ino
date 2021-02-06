#include "hwcrypto/sha.h"
#include <WiFi.h>

#define LED_BUILTIN 2 // Change this if your board has built-in led on non-standard pin (NodeMCU - 16 or 2)

TaskHandle_t Task1;
TaskHandle_t Task2;

const char* ssid     = "SONY_BRAVIA-55xf8505"; // Change this to your WiFi SSID
const char* password = "abcdef123456"; // Change this to your WiFi password
const char* ducouser = "JoyBed"; // Change this to your Duino-Coin username

const char * host = "51.15.127.80"; // Static server IP
const int port = 2811;

//Task1code
void Task1code( void * pvParameters ){
  unsigned int acceptedShares1 = 0; // Shares variables
  unsigned int rejectedShares1 = 0;

  Serial.println("\nCORE1 Connecting to Duino-Coin server..."); 
  // Use WiFiClient class to create TCP connection
  WiFiClient client1;
  Serial.println(client1.connect(host, port));
  Serial.println();

  String SERVER_VER = client1.readString(); // Server sends SERVER_VERSION after connecting
  Serial.println("CORE1 Connected to the server. Server version: " + String(SERVER_VER));
  Serial.println();
  digitalWrite(LED_BUILTIN, HIGH);   // Turn off built-in led
  delay(150);
  digitalWrite(LED_BUILTIN, LOW);   // Turn on built-in led
  delay(150);
  digitalWrite(LED_BUILTIN, HIGH);   // Turn off built-in led

  while (client1.connected()) {
    Serial.println("CORE1 Asking for a new job for user: " + String(ducouser));
    Serial.println();
    client1.print("JOB," + String(ducouser) + ",ESP"); // Ask for new job

    String hash1 = client1.readStringUntil(','); // Read last block hash
    String job1 = client1.readStringUntil(','); // Read expected hash
    job1.toUpperCase();
    const char * c = job1.c_str();

    size_t len = strlen(c);
    size_t final_len = len / 2;
    unsigned char* job11 = (unsigned char*)malloc((final_len + 1) * sizeof(unsigned char));
    for (size_t i = 0, j = 0; j < final_len; i += 2, j++)
        job11[j] = (c[i] % 32 + 9) % 25 * 16 + (c[i + 1] % 32 + 9) % 25;
    
    byte shaResult1[20];
    unsigned long diff1 =  (5000) * 100 + 1; // Low power devices use the low diff job, we don't read it as no termination character causes unnecessary network lag
    Serial.println("CORE1 Job received: " + String(hash1) + " " + String(job1) + " " + String(diff1));
    Serial.println();
    unsigned long StartTime1 = micros(); // Start time measurement

    for (unsigned long iJob1 = 0; iJob1 < diff1; iJob1++) { // Difficulty loop
      yield(); // uncomment if ESP watchdog triggers
      String hash11 = String(hash1) + String(iJob1);
      const unsigned char* payload1 = (const unsigned char*) hash11.c_str();
      unsigned int payloadLenght1 = hash11.length();

      esp_sha(SHA1, payload1, payloadLenght1, shaResult1);
      
      int compareresult1 = memcmp(shaResult1, job11, sizeof(shaResult1));
      
      if (compareresult1 == 0) { // If result is found
        unsigned long EndTime1 = micros(); // End time measurement
        unsigned long ElapsedTime1 = EndTime1 - StartTime1; // Calculate elapsed time
        float ElapsedTimeSeconds1 = ElapsedTime1 / 1000; // Convert to seconds
        float HashRate1 = ElapsedTime1 / iJob; // Calculate hashrate
        client1.print(String(iJob1) + "," + String(HashRate1) + ",ESP32 CORE1 Miner v1.8"); // Send result to server
        String feedback1 = client1.readStringUntil('D'); // Receive feedback
        if (feedback1.indexOf("GOOD")) {
          acceptedShares1++;
          Serial.println("CORE1 Accepted share #" + String(acceptedShares1) + " (" + String(iJob1) + ")" + " Hashrate: " + String(HashRate1) + "Free RAM: " + String(ESP.getFreeHeap()));
          Serial.println();
        } else {
          rejectedShares1++;
          Serial.println("CORE1 Rejected share #" + String(acceptedShares1) + " (" + String(iJob1) + ")" + " Hashrate: " + String(HashRate1));
          Serial.println();
        }
        break; // Stop and ask for more work
      }
    }  
  }

  Serial.println("CORE1 Not connected. Restarting ESP");
  Serial.println();
  esp_restart(); // Restart the board
}

//Task2code
void Task2code( void * pvParameters ){
  unsigned int acceptedShares = 0; // Shares variables
  unsigned int rejectedShares = 0;

  Serial.println("\nCORE2 Connecting to Duino-Coin server...");
  Serial.println();
  // Use WiFiClient class to create TCP connection
  WiFiClient client;
  Serial.println(client.connect(host, port));
  Serial.println();

  String SERVER_VER = client.readString(); // Server sends SERVER_VERSION after connecting
  Serial.println("CORE2 Connected to the server. Server version: " + String(SERVER_VER));
  Serial.println();
  digitalWrite(LED_BUILTIN, HIGH);   // Turn off built-in led
  delay(150);
  digitalWrite(LED_BUILTIN, LOW);   // Turn on built-in led
  delay(150);
  digitalWrite(LED_BUILTIN, HIGH);   // Turn off built-in led
  while (client.connected()) {
    Serial.println("CORE2 Asking for a new job for user: " + String(ducouser));
    Serial.println();
    client.print("JOB," + String(ducouser) + ",ESP"); // Ask for new job

    String hash = client.readStringUntil(','); // Read last block hash
    String job = client.readStringUntil(','); // Read expected hash
    job.toUpperCase();
    const char * c = job.c_str();

    size_t len = strlen(c);
    size_t final_len = len / 2;
    unsigned char* job1 = (unsigned char*)malloc((final_len + 1) * sizeof(unsigned char));
    for (size_t i = 0, j = 0; j < final_len; i += 2, j++)
        job1[j] = (c[i] % 32 + 9) % 25 * 16 + (c[i + 1] % 32 + 9) % 25;
    
    byte shaResult[20];
    unsigned long diff =  (5000) * 100 + 1; // Low power devices use the low diff job, we don't read it as no termination character causes unnecessary network lag
    Serial.println("CORE2 Job received: " + String(hash) + " " + String(job) + " " + String(diff));
    Serial.println();
    unsigned long StartTime = micros(); // Start time measurement

    for (unsigned long iJob = 0; iJob < diff; iJob++) { // Difficulty loop
      yield(); // uncomment if ESP watchdog triggers
      String hash1 = String(hash) + String(iJob);
      const unsigned char* payload = (const unsigned char*) hash1.c_str();
      unsigned int payloadLenght = hash1.length();

      esp_sha(SHA1, payload, payloadLenght, shaResult);
      
      int compareresult = memcmp(shaResult, job1, sizeof(shaResult));
      
      if (compareresult == 0) { // If result is found
        unsigned long EndTime = micros(); // End time measurement
        unsigned long ElapsedTime = EndTime - StartTime; // Calculate elapsed time
        float ElapsedTimeSeconds = ElapsedTime / 1000; // Convert to seconds
        float HashRate = ElapsedTime / iJob; // Calculate hashrate
        client.print(String(iJob) + "," + String(HashRate) + ",ESP32 CORE2 Miner v1.8"); // Send result to server
        String feedback = client.readStringUntil('D'); // Receive feedback
        if (feedback.indexOf("GOOD")) {
          acceptedShares++;
          Serial.println("CORE2 Accepted share #" + String(acceptedShares) + " (" + String(iJob) + ")" + " Hashrate: " + String(HashRate));
          Serial.println();
        } else {
          rejectedShares++;
          Serial.println("CORE2 Rejected share #" + String(acceptedShares) + " (" + String(iJob) + ")" + " Hashrate: " + String(HashRate));
          Serial.println();
        }
        break; // Stop and ask for more work
      }
    }  
  }

  Serial.println("CORE2 Not connected. Restarting ESP");
  Serial.println();
  esp_restart(); // Restart the board
}

void setup() {
  disableCore0WDT();
  disableCore1WDT();
  Serial.begin(115200); // Start serial connection
  Serial.println("\n\nDuino-Coin ESP32 Miner v1.8");
  Serial.println("Connecting to: " + String(ssid));
  WiFi.mode(WIFI_STA); // Setup ESP in client mode
  WiFi.begin(ssid, password); // Connect to wifi

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  Serial.println("Local IP address: " + WiFi.localIP().toString());
  Serial.println();
  xTaskCreatePinnedToCore(Task1code,"Task1",10000,NULL,1,&Task1,0);  //create a task with priority 1 and executed on core 0            
  delay(500); 
  xTaskCreatePinnedToCore(Task2code,"Task2",10000,NULL,2,&Task2,1);  //create a task with priority 1 and executed on core 1
  delay(500);
}

void loop() {}
