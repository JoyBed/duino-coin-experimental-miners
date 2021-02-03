//////////////////////////////////////////////////////////
//  _____        _                    _____      _
// |  __ \      (_)                  / ____|    (_)
// | |  | |_   _ _ _ __   ___ ______| |     ___  _ _ __
// | |  | | | | | | '_ \ / _ \______| |    / _ \| | '_ \ 
// | |__| | |_| | | | | | (_) |     | |___| (_) | | | | |
// |_____/ \__,_|_|_| |_|\___/       \_____\___/|_|_| |_|
//  Code for ESP8266 boards v1.8
//  © Duino-Coin Community 2019-2020
//  Distributed under MIT License
//////////////////////////////////////////////////////////
//  https://github.com/revoxhere/duino-coin - GitHub
//  https://duinocoin.com - Official Website
//  https://discord.gg/k48Ht5y - Discord
//  https://github.com/revoxhere - @revox
//  https://github.com/JoyBed - @JoyBed
//////////////////////////////////////////////////////////
//  If you don't know what to do, visit official website
//  and navigate to Getting Started page. Happy mining!
//////////////////////////////////////////////////////////

// TIP for revox: MAKE SURE THERE ISN'T ANY OF YOUR PASSWORDS BEFORE COMMIT
const char* ssid     = "XXX"; // Change this to your WiFi SSID
const char* password = "XXX"; // Change this to your WiFi password
const char* ducouser = "XXX"; // Change this to your Duino-Coin username
// TIP for revox: MAKE SURE THERE ISN'T ANY OF YOUR PASSWORDS BEFORE COMMIT

#define LED_BUILTIN 2 // Change this if your board has built-in led on non-standard pin (NodeMCU - 16 or 2)

#include <ESP8266WiFi.h> // Include WiFi library
#include <Hash.h> // SHA1 crypto library

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // Define built-in led as output
  Serial.begin(115200); // Start serial connection
  Serial.println("\n\nDuino-Coin ESP Miner v1.8");

  Serial.println("Connecting to: " + String(ssid));
  WiFi.mode(WIFI_STA); // Setup ESP in client mode
  WiFi.begin(ssid, password); // Connect to wifi

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");
  Serial.println("Local IP address: " + WiFi.localIP().toString());
  blink(2); // Blink 2 times - indicate sucessfull connection with wifi network
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

void loop() {
  const char * host = "51.15.127.80"; // Static server IP
  const int port = 2811;
  unsigned int acceptedShares = 0; // Shares variables
  unsigned int rejectedShares = 0;

  Serial.println("\nConnecting to Duino-Coin server...");
  // Use WiFiClient class to create TCP connection
  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("Connection failed.");
    Serial.println("Retrying...");
    blink(7); // Blink 7 times indicating connection error
    ESP.reset(); // Restart the board
    ESP.restart();
  }
  client.setTimeout(5000);
  String SERVER_VER = client.readString(); // Server sends SERVER_VERSION after connecting
  Serial.println("Connected to the server. Server version: " + String(SERVER_VER));
  blink(3); // Blink 3 times - indicate sucessfull connection with the server

  while (client.connected()) {
    Serial.println("Asking for a new job for user: " + String(ducouser));
    client.print("JOB," + String(ducouser) + ",ESP"); // Ask for new job

    String hash = client.readStringUntil(','); // Read last block hash
    String job = client.readStringUntil(','); // Read expected hash
    unsigned int diff =  (1500) * 100 + 1; // Low power devices use the low diff job, we don't read it as no termination character causes unnecessary network lag
    Serial.println("Job received: " + String(hash) + " " + String(job) + " " + String(diff));
    unsigned long StartTime = micros(); // Start time measurement

    for (unsigned int iJob = 0; iJob < diff; iJob++) { // Difficulty loop
      yield(); // uncomment if ESP watchdog triggers
      String result = sha1(String(hash) + String(iJob)); // Hash previous block hash and current iJob
      if (result == job) { // If result is found
        unsigned long EndTime = micros(); // End time measurement
        unsigned long ElapsedTime = EndTime - StartTime; // Calculate elapsed time
        float ElapsedTimeSeconds = ElapsedTime / 60; // Convert to seconds
        float HashRate = ElapsedTime / 60; // Calculate hashrate
        client.print(String(iJob) + "," + String(HashRate) + ",ESP Miner v1.8"); // Send result to server

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
  blink(7); // Blink 7 times indicating connection error
  ESP.reset(); // Restart the board
  ESP.restart();
}
