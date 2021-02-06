#include "hwcrypto/sha.h"
#include <WiFi.h>

#define LED_BUILTIN 2 // Change this if your board has built-in led on non-standard pin (NodeMCU - 16 or 2)

const char* ssid     = "SONY_BRAVIA-55xf8505"; // Change this to your WiFi SSID
const char* password = "abcdef123456"; // Change this to your WiFi password
const char* ducouser = "JoyBed"; // Change this to your Duino-Coin username

void setup() {
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
}

void loop() {
  const char * host = "51.15.127.80"; // Static server IP
  const int port = 2811;
  unsigned int acceptedShares = 0; // Shares variables
  unsigned int rejectedShares = 0;

  Serial.println("\nConnecting to Duino-Coin server...");
  // Use WiFiClient class to create TCP connection
  WiFiClient client;
  Serial.println(client.connect(host, port));

  String SERVER_VER = client.readString(); // Server sends SERVER_VERSION after connecting
  Serial.println("Connected to the server. Server version: " + String(SERVER_VER));
  blink(3); // Blink 3 times - indicate sucessfull connection with the server

  while (client.connected()) {
    Serial.println("Asking for a new job for user: " + String(ducouser));
    client.print("JOB," + String(ducouser) + ",ESP"); // Ask for new job

    String hash = client.readStringUntil(','); // Read last block hash
    String job = client.readStringUntil(','); // Read expected hash
    job.toUpperCase();
    const char * c = job.c_str();
    unsigned char* job1 = hexstr_to_char(c);
    byte shaResult[20];
    unsigned long diff =  (5000) * 100 + 1; // Low power devices use the low diff job, we don't read it as no termination character causes unnecessary network lag
    Serial.println("Job received: " + String(hash) + " " + String(job) + " " + String(diff));
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
        client.print(String(iJob) + "," + String(HashRate) + ",ESP32 Miner v1.8"); // Send result to server
        String feedback = client.readStringUntil('D'); // Receive feedback
        if (feedback.indexOf("GOOD")) {
          acceptedShares++;
          Serial.println("Accepted share #" + String(acceptedShares) + " (" + String(iJob) + ")" + " Hashrate: " + String(HashRate));
        } else {
          rejectedShares++;
          Serial.println("Rejected share #" + String(acceptedShares) + " (" + String(iJob) + ")" + " Hashrate: " + String(HashRate));
        }
        break; // Stop and ask for more work
      }
    }  
  }

  Serial.println("Not connected. Restarting ESP");
  esp_restart(); // Restart the board
}

unsigned char* hexstr_to_char(const char* hexstr) {
    size_t len = strlen(hexstr);
    if (len % 2 != 0)
        return NULL;
    size_t final_len = len / 2;
    unsigned char* chrs = (unsigned char*)malloc((final_len + 1) * sizeof(unsigned char));
    for (size_t i = 0, j = 0; j < final_len; i += 2, j++)
        chrs[j] = (hexstr[i] % 32 + 9) % 25 * 16 + (hexstr[i + 1] % 32 + 9) % 25;
    return chrs;
}

void blink(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_BUILTIN, HIGH);   // Turn off built-in led
    delay(150);
    digitalWrite(LED_BUILTIN, LOW);   // Turn on built-in led
    delay(150);
    digitalWrite(LED_BUILTIN, HIGH);   // Turn off built-in led
  }
}
