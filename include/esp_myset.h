#ifndef IOT_SET
#define IOT_SET

#include <Arduino.h>
#include <string>

#define ALEN(a) ((int) (sizeof(a) / sizeof(a[0]))) 

extern const char *templ[];

typedef struct { int position; const char *replacement; } rep_type;

void get_Html(String& html, const char *[], int, rep_type [], int);
#define getHtml(strout, templ, repls) get_Html(strout, templ, ALEN(templ), repls, ALEN(repls));


// MAC address 
extern char MAC_ADDRESS[];
void getMAC(char *);

#include <WiFi.h>
#include <WebServer.h>

extern String apSSID;           // SSID of the AP
extern WebServer webServer;     // a simple web server

void startAP();
void printIPs();
void startWebServer();
void ErrorHandle();
void HomePage();
void HelloPage();

void WiFiSelect();
void WiFiConnect();
void WiFiStatus();
void WiFiList(String& f);
String ip_address(IPAddress address);


#endif
