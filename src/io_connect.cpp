#include "io_connect.h"
#include "mydht.h"
#include "esp_myset.h"

String apSSID;                  // SSID of the AP
WebServer webServer(80);        // a simple web server
int loopIteration = 0;
char MAC_ADDRESS[13];
int lengthb = 0;
int lengthl = 0;

const char* ssid = "ASK4 Wireless";
const char* password = "";
const char* mqttServer = "io.adafruit.com";
const int mqttPort = 1883;
const char* mqttUser = "Keanne";
const char* mqttPassword = "aio_ueJA80yyQZz4uKgEQe8VENFRZITU";
const char* mqttTopic_light = "Keanne/feeds/light";
const char* mqttTopic_temperature = "Keanne/feeds/temperature";
const char* mqttTopic_humidity = "Keanne/feeds/humidity";
const char* mqttTopic_heater = "Keanne/feeds/heater";
const char* mqttTopic_autotemp = "Keanne/feeds/autotemp";

const char *boiler[] = { // boilerplate: constants & pattern parts of template
  "<html><head><title>",                                                // 0
  "default title",                                                      // 1
  "</title>\n",                                                         // 2
  "<meta charset='utf-8'>",                                             // 3

  // adjacent strings in C are concatenated:
  "<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n"
  "<style>body{background:#FFF; color: #000; font-family: sans-serif;", // 4

  "font-size: 150%;}</style>\n",                                        //  5
  "</head><body>\n",                                                    //  6
  "<h2>Welcome to Thing!</h2>\n",                                       //  7
  "<!-- page payload goes here... -->\n",                               //  8
  "<!-- ...and/or here... -->\n",                                       //  9
  "\n<p><a href='/'>Home</a>&nbsp;&nbsp;&nbsp;</p>\n",                  // 10
  "</body></html>\n\n",                                                 // 11
};


WiFiClient espClient;
PubSubClient client(espClient);

//--------- WIFI -------------------------------------------

/*void wifi_connect() {
  Serial.print("Starting connecting WiFi.");
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}*/

//------------------ MQTT ----------------------------------
void mqtt_setup() {
  client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
    Serial.println("Connecting to MQTT…");
    int count = 0;
    while (!client.connected()) {        
        String clientId = "ESP32Client-";
        clientId += String(random(0xffff), HEX);
        if (client.connect(clientId.c_str(), mqttUser, mqttPassword )) {
            Serial.println("connected!!!!");
        } else {
            Serial.print("failed with state  ");
            Serial.println(client.state());
            delay(2000);
        }
        count++;
        if(count==20){
          Serial.println("Connect to mqtt fail!");
          break;
        }
    }

    mqtt_send_lamp_status();
    mqtt_send_heater_status();
    client.subscribe(mqttTopic_light);
    client.subscribe(mqttTopic_heater);
    client.subscribe(mqttTopic_autotemp);
}


void mqtt_send_temphum(){
  int temp = (int)getTemp();
  int hum = (int)getHum();
  String temp_s = String(temp);
  char tempc[3];
  temp_s.toCharArray(tempc,3);
  client.publish(mqttTopic_temperature, tempc);
  String hum_s = String(hum);
  char humc[3];
  hum_s.toCharArray(humc,3);
  client.publish(mqttTopic_humidity, humc);
}

void mqtt_send_heater_status() {   //use LED as the flag of the condition of auto mode
  int val = digitalRead(LED);
  Serial.printf("Sending LAMP status: ");
  if(val == LED_OFF) {
    Serial.println("OFF");
    client.publish(mqttTopic_heater, "OFF");
  } else {
    Serial.println("ON");
    client.publish(mqttTopic_heater, "ON");
  } 
}

void mqtt_send_lamp_status() {   
  int val = digitalRead(LED0);
  Serial.printf("Sending LAMP status: ");
  if(val == LED_OFF) {
    Serial.println("OFF");
    client.publish(mqttTopic_light, "OFF");
  } else {
    Serial.println("ON");
    client.publish(mqttTopic_light, "ON");
  } 
}

void mqtt_lamp_on(){
  client.publish(mqttTopic_light, "ON");
  Serial.println("Sending to mqtt...");
}

void mqtt_lamp_off(){
  client.publish(mqttTopic_light, "OFF");
  Serial.println("Sending to mqtt...");
}

void callback(char* topic, byte* payload, unsigned int length) {

    Serial.print("Message arrived in topic: ");
    Serial.print(topic);
    Serial.println("!!!");

    String byteRead = "";
    Serial.print("Message: ");
    for (int i = 0; i < length; i++) {
        byteRead += (char)payload[i];
    }    
    Serial.println(byteRead);

    if(strcmp(topic, mqttTopic_light)==0){
      Serial.println("light control");
      if (byteRead == "OFF"){
        Serial.println("LAMP OFF!");
        digitalWrite(LED0, LED_OFF);
      }

      if (byteRead == "ON"){
        Serial.println("LAMP ON!");
        digitalWrite(LED0, LED_ON);
      }
    }else if(strcmp(topic, mqttTopic_heater)==0){
      if (byteRead == "OFF"){
        Serial.println("Heater OFF!");
        digitalWrite(LED, LED_OFF);
      }

      if (byteRead == "ON"){
        Serial.println("Heater ON!");
        digitalWrite(LED, LED_ON);
      }
    }else if(strcmp(topic, mqttTopic_autotemp)==0){
      sscanf(byteRead.c_str(),"%d",&temp_set);
      Serial.println("temperature set change");
      Serial.print("the target is: ");
      Serial.println(temp_set);
    }

    Serial.println();
    Serial.println(" — — — — — — — — — — — -");

}

// setup and loop ///////////////////////////////////////////////////////////
void wifi_connect() {
  Serial.begin(115200);
  getMAC(MAC_ADDRESS);

  startAP();            // fire up the AP...
  startWebServer();     // ...and the web server
}

void getMAC(char *buf) { // the MAC is 6 bytes, so needs careful conversion...
  uint64_t mac = ESP.getEfuseMac(); // ...to string (high 2, low 4):
  char rev[13];
  sprintf(rev, "%04X%08X", (uint16_t) (mac >> 32), (uint32_t) mac);

  // the byte order in the ESP has to be reversed relative to normal Arduino
  for(int i=0, j=11; i<=10; i+=2, j-=2) {
    buf[i] = rev[j - 1];
    buf[i + 1] = rev[j];
  }
  buf[12] = '\0';
}

// startup utilities 
void startAP() {
  apSSID = String("Thing-");
  apSSID.concat(MAC_ADDRESS);

  if(! WiFi.mode(WIFI_AP_STA))
    Serial.println("failed to set Wifi mode");
  if(! WiFi.softAP(apSSID.c_str(), "zhang163"))
    Serial.println("failed to start soft AP");
  printIPs();
}

void printIPs() {
    Serial.print("AP SSID: ");
    Serial.print(apSSID);
    Serial.print("; IP address(es): local=");
    Serial.print(WiFi.localIP());
    Serial.print("; AP=");
    Serial.println(WiFi.softAPIP());

    WiFi.printDiag(Serial);
}
void startWebServer() {
  // register callbacks to handle different paths
  webServer.on("/", handleRoot);
  webServer.on("/hello", handleHello);
  webServer.on("/wifi", hndlWifi);          // page for choosing an AP
  webServer.on("/wifichz", hndlWifichz);    // landing page for AP form submit
  webServer.on("/status", hndlStatus);      // status check, e.g. IP address


  // 404s...
  webServer.onNotFound(handleNotFound);

  webServer.begin();
  Serial.println("HTTP server started");
}

// webserver handler callbacks ///////////////////////////////////////////////
void handleNotFound() {
  Serial.print("URI Not Found: ");
  Serial.println(webServer.uri());
  webServer.send(200, "text/plain", "URI Not Found");
}

void handleRoot() {
  Serial.println("serving page notionally at /");
  replacement_t repls[] = { // the elements to replace in the boilerplate
    {  1, apSSID.c_str() },
    {  8, "" },
    {  9, "<p>Choose a <a href=\"wifi\">wifi access point</a>.</p>" },
    { 10, "<p>Check <a href='/status'>wifi status</a>.</p>" },
  };

  String htmltext = "";
  getHtml(htmltext, boiler, repls);
  webServer.send(200, "text/html", htmltext);
}

void handleHello() {
  Serial.println("serving /hello");
  webServer.send(
    200,
    "text/plain",
    "Hello! Have you considered sending your lecturer a large gift today? :)\n"
  );
}

void hndlWifi() {
  Serial.println("serving page at /wifi");

  String form = ""; // a form for choosing an access point and entering key
  apListForm(form);
  replacement_t repls[] = { // the elements to replace in the boilerplate
    { 1, apSSID.c_str() },
    { 7, "<h2>Network configuration</h2>\n" },
    { 8, "" },
    { 9, form.c_str() },
  };
  String htmlPage = ""; // a String to hold the resultant page
  getHtml(htmlPage, boiler, repls); // GET_HTML sneakily added to Ex07

  webServer.send(200, "text/html", htmlPage);
}


void hndlWifichz() {
  Serial.println("serving page at /wifichz");

  String title = "<h2>wifi connection complete</h2>";
  String message = "<p>Check <a href='/status'>wifi status</a>.</p>";

  String ssid = "";
  String key = "";
  for(uint8_t i = 0; i < webServer.args(); i++ ) {
    if(webServer.argName(i) == "ssid")
      ssid = webServer.arg(i);
    else if(webServer.argName(i) == "key")
      key = webServer.arg(i);
  }

  if(ssid == "") {
    message = "<h2>Ooops, no SSID...?</h2>\n<p>Looks like a bug :-(</p>";
  } else {
    char ssidchars[ssid.length()+1];
    char keychars[key.length()+1];
    ssid.toCharArray(ssidchars, ssid.length()+1);
    key.toCharArray(keychars, key.length()+1);
    WiFi.begin(ssidchars, keychars);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.print("Connecting successful!\n");
    mqtt_setup();
  }

  replacement_t repls[] = { // the elements to replace in the template
    { 1, apSSID.c_str() },
    { 7, title.c_str() },
    { 8, "" },
    { 9, message.c_str() },
  };
  String htmlPage = "";     // a String to hold the resultant page
  getHtml(htmlPage, boiler, repls);

  webServer.send(200, "text/html", htmlPage);
}


void hndlStatus() {         // UI for checking connectivity etc.
  Serial.println("serving page at /status");

  String s = "";
  s += "<ul>\n";
  s += "\n<li>SSID: ";
  s += WiFi.SSID();
  s += "</li>";
  s += "\n<li>Status: ";
  switch(WiFi.status()) {
    case WL_IDLE_STATUS:
      s += "WL_IDLE_STATUS</li>"; break;
    case WL_NO_SSID_AVAIL:
      s += "WL_NO_SSID_AVAIL</li>"; break;
    case WL_SCAN_COMPLETED:
      s += "WL_SCAN_COMPLETED</li>"; break;
    case WL_CONNECTED:
      s += "WL_CONNECTED</li>"; break;
    case WL_CONNECT_FAILED:
      s += "WL_CONNECT_FAILED</li>"; break;
    case WL_CONNECTION_LOST:
      s += "WL_CONNECTION_LOST</li>"; break;
    case WL_DISCONNECTED:
      s += "WL_DISCONNECTED</li>"; break;
    default:
      s += "unknown</li>";
  }

  s += "\n<li>Local IP: ";     s += ip2str(WiFi.localIP());
  s += "</li>\n";
  s += "\n<li>Soft AP IP: ";   s += ip2str(WiFi.softAPIP());
  s += "</li>\n";
  s += "\n<li>AP SSID name: "; s += apSSID;
  s += "</li>\n";

  s += "</ul></p>";

  replacement_t repls[] = { // the elements to replace in the boilerplate
    { 1, apSSID.c_str() },
    { 7, "<h2>Status</h2>\n" },
    { 8, "" },
    { 9, s.c_str() },
  };
  String htmlPage = ""; // a String to hold the resultant page
  getHtml(htmlPage, boiler, repls); // GET_HTML sneakily added to Ex07

  webServer.send(200, "text/html", htmlPage);
}


void apListForm(String& f) { // utility to create a form for choosing AP
  const char *checked = " checked";
  int n = WiFi.scanNetworks();
  Serial.print("scan done: ");

  if(n == 0) {
    Serial.println("no networks found");
    f += "No wifi access points found :-( ";
    f += "<a href='/'>Back</a><br/><a href='/wifi'>Try again?</a></p>\n";
  } else {
    Serial.print(n); Serial.println(" networks found");
    f += "<p>Wifi access points available:</p>\n"
         "<p><form method='POST' action='wifichz'> ";
    for(int i = 0; i < n; ++i) {
      f.concat("<input type='radio' name='ssid' value='");
      f.concat(WiFi.SSID(i));
      f.concat("'");
      f.concat(checked);
      f.concat(">");
      f.concat(WiFi.SSID(i));
      f.concat(" (");
      f.concat(WiFi.RSSI(i));
      f.concat(" dBm)");
      f.concat("<br/>\n");
      checked = "";
    }
    f += "<br/>Pass key: <input type='textarea' name='key'><br/><br/> ";
    f += "<input type='submit' value='Submit'></form></p>";
  }
}
String ip2str(IPAddress address) { // utility for printing IP addresses
  return
    String(address[0]) + "." + String(address[1]) + "." +
    String(address[2]) + "." + String(address[3]);
}

void get_Html( // turn array of strings & set of replacements into a String
  String& html, const char *boiler[], int boilerLen,
  replacement_t repls[], int replsLen
) {
  for(int i = 0, j = 0; i < boilerLen; i++) {
    if(j < replsLen && repls[j].position == i)
      html.concat(repls[j++].replacement);
    else
      html.concat(boiler[i]);
  }
}

void setup_ios() {
  pinMode(LED, OUTPUT);
}

//--------- ARDUINO --------------------------------------
void connection_setup() {  
  Serial.begin(115200);
  setup_ios();  
  wifi_connect();
  //mqtt_setup(); 
}

void connection_loop() {
    client.loop();
    webServer.handleClient();
}
