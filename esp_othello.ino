#include <ESP8266WiFi.h>
#include <string.h>
#include "othello.hpp"
////////////////////// // WiFi Definitions //
//////////////////////
// this esp's ap credentials
const char AP_NAME[] = "othello";
const char WiFiAPPSK[] = "othello123";
// other wifi credentials
//const char *ssid = "";
//const char *password = "";

/////////////////////
// Pin Definitions //
/////////////////////
const int LED_PIN = D4; // ESP's onboard, green LED
const int ANALOG_PIN = A0; // The only analog pin on the ESP
const int DIGITAL_PIN = D3; // Digital pin to be read


int wifiStatus;
IPAddress ip(1,1,1,1);                // this node's ap ip
IPAddress gateway(1,1,1,1);           // this node's ap default router
IPAddress subnet(255,255,255,0);      // subnet mask, this node's ap subnet addres: 1.1.1.1, broadcast: 1.1.1.255
WiFiServer server(80); 

Othello game1(8, Serial);
OthelloServer game1srv(game1);

// HTML BUFFERS
struct othello_page {
    int html_len;
    char html[8192];
};

void init_othello_page(struct othello_page *p) {
    strcpy(p->html, 
"HTTP/1.1 200 OK\r\n\
Content-Type: text/html\r\n\r\n\
<!DOCTYPE HTML>\r\n<html>\r\
<head> <title>ESP8266 Othello</title> </head>\
<body> <h1>ESP8266 Othello</h1>");
    p->html_len = strlen(p->html);
}

void end_othello_page(struct othello_page *p) {
    char *htmlp = p->html + p->html_len;
    strcpy(htmlp,
"</body>\n\
</html>\n");
    p->html_len = strlen(p->html);
}

void add_t1(struct othello_page *p) {
    char t1p1[] = "<table border=\"1\"> <tr><th>Black Score: </th><th>White Score: </th><th>Current Player: </th></tr>";
    char nb[4]; itoa(game1.numBlack, nb, 10);
    char nw[4]; itoa(game1.numWhite, nw, 10);
    char *htmlp = p->html + p->html_len;

    strcpy(htmlp, t1p1);
    htmlp += strlen(t1p1);
    strcpy(htmlp, "<tr><td>");
    htmlp += 8;
    strcpy(htmlp, nb);
    htmlp += strlen(nb);
    strcpy(htmlp, "</td><td>");
    htmlp += 9;
    strcpy(htmlp, nw);
    htmlp += strlen(nw);
    strcpy(htmlp, "</td><td>");
    htmlp += 9;
    strcpy(htmlp, (game1.current == Othello::BLACK ? "Black" : "White"));
    htmlp += 5;
    strcpy(htmlp, "</td></tr>\n</table>\n");
    htmlp += 20;

    p->html_len = strlen(p->html);

    if ((p->html + p->html_len) != htmlp) {
        Serial.println("T1 ERROR");
        Serial.println(p->html);
    }
}

void add_t2(struct othello_page *p) {
    char *htmlp = p->html + p->html_len;
    char ib[4];
    char jb[4];

    strcpy(htmlp, "<table border=\"1\">");
    htmlp += 18;
    for (int i = 0; i < 8; i++) {
        itoa(i, ib, 10);
        strcpy(htmlp, "<tr>");
        htmlp += 4;
        for (int j = 0; j < 8; j++) {
            itoa(j, jb, 10);
            strcpy(htmlp, "<td><form name=\"");
            htmlp += 16;
            strcpy(htmlp, ib);
            htmlp++;
            strcpy(htmlp, jb);
            strcpy(htmlp, "\" action=\"/maketurn");
            htmlp += 19;
            strcpy(htmlp, ib);
            htmlp++;
            strcpy(htmlp, jb);
            htmlp++;
            strcpy(htmlp, "\"><button type=submit\" style=\"background-color: ");
            htmlp += 48;
            if (game1.grid[i][j] == Othello::BLACK) {
                strcpy(htmlp, "#555555;\"> b ");
            }
            else if (game1.grid[i][j] == Othello::WHITE) {
                strcpy(htmlp, "#FFFFFF;\"> w ");
            }
            else {
                strcpy(htmlp, "#e7e7e7;\"> _ ");
            }
            htmlp += 13;
            strcpy(htmlp, "</button></form></td>");
            htmlp += 21;
        }
        strcpy(htmlp, "</tr>\n");
        htmlp += 6;
    }
    strcpy(htmlp, "</table>");
    htmlp += 8;

    p->html_len = strlen(p->html);

    if ((p->html + p->html_len) != htmlp) {
        Serial.println("T2 ERROR");
        Serial.println(p->html);
    }
}

struct othello_page page;

void setup() 
{
  initHardware();
  setupWiFi();
  server.begin();
  init_othello_page(&page);
}

void loop() 
{
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val = -1; // We'll use 'val' to keep track of both the
                // request type (read/set) and value if set.
  int ri = -1;
  char br, bc;
  if ((ri = req.indexOf("/othello")) != -1) {
    val = 1; // Will print othello board
  } else if ((ri = req.indexOf("/maketurn")) != -1) {
    val = 2; // Will print othello board
    br = req[ri+9];
    bc = req[ri+10];
  } else if ((ri = req.indexOf("/reset")) != -1) {
    val = 3; // Will print othello board
  }

  client.flush();

  if (val == 1 || val == 2) {
        if (val == 2) {
            // TODO: Validate this
            game1.makeTurn((int)br - 48, (int) bc - 48);
        }

        init_othello_page(&page);
        add_t1(&page);
        add_t2(&page);
        end_othello_page(&page);
  }
  else if (val == 3) {
        game1.reset();
        init_othello_page(&page);
        add_t1(&page);
        add_t2(&page);
        end_othello_page(&page);
  }

  // Send the response to the client
  client.print(page.html);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}

void setupWiFi()
{
  Serial.print("This device's MAC address is: ");
  Serial.println(WiFi.macAddress());

  //WiFi.mode(WIFI_AP_STA);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(ip, gateway, subnet);
  WiFi.softAP(AP_NAME, WiFiAPPSK, 6, 0);
  Serial.print("This AP's IP address is: ");
  Serial.println(WiFi.softAPIP());  

    /*
  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int linenum = 10, attempt = 1;
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      if (attempt % linenum == 0) {
          Serial.println(".");
      }
      else {
          Serial.print(".");
      }
      attempt++;
  }
  wifiStatus = WiFi.status();
  if(wifiStatus == WL_CONNECTED){
      Serial.print("\nConnected - Your IP address is: ");
      Serial.println(WiFi.localIP());  
  }
    */
}
void initHardware()
{
  Serial.begin(115200);
  Serial.println("\n\n");
  pinMode(DIGITAL_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT); 
  digitalWrite(LED_PIN, HIGH);//on Lolin ESP8266 v3 dev boards, the led is active low
  delay(1000);
  // Don't need to set ANALOG_PIN as input, 
  // that's all it can be.
}
