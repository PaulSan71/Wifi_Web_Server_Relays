/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com
*********/

#include <Arduino.h>
#include <FastLED.h>
#include <Wire.h>
#include <WiFi.h>

FASTLED_USING_NAMESPACE

#define DATA_PIN 2
#define LED_TYPE WS2811
#define COLOR_ORDER RGB
#define NUM_LEDS 100

CRGB leds[NUM_LEDS];

// Replace with your network credentials
const char *ssid = "MySpectrumWiFi68-2G";
const char *password = "orangewhale940";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String redState = "off";
String redGState = "off";
String blueState = "off";
String blueGState = "off";
String greenState = "off";
String greenGState = "off";
String noneState = "off";
String pinkState = "off";
String pinkGState = "off";
String goldState = "off";
String goldGState = "off";
String redGreenState = "off";
String redGreenGState = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

String PatternName[] = {
    "Red",
    "Blue",
    "Green",
    "None"};

enum n {Red, RedG, Blue, BlueG, Green, GreenG, Pink, PinkG, 
        Gold, GoldG, RedGreen, RedGreenG, None};

#define BRIGHTNESS 96
#define FRAMES_PER_SECOND 240

void nextPattern();
void red();
void redG();
void blue();
void blueG();
void green();
void greenG();
void pink();
void pinkG();
void gold();
void goldG();
void redGreen();
void redGreenG();

void addGlitter(fract8 chanceOfGlitter);
void none();

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns =   {
                                red,
                                redG,
                                blue,
                                blueG,
                                green,
                                greenG,
                                pink,
                                pinkG,
                                gold,
                                goldG,
                                redGreen,
                                redGreenG,
                                none
                                };

n gCurrentPatternNumber = None; // Index number of which pattern is current

void setup()
{
    Serial.begin(115200);

    // tell FastLED about the LED strip configuration
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

    // set master brightness control
    FastLED.setBrightness(BRIGHTNESS);

    // Connect to Wi-Fi network with SSID and password
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();
}

void loop()
{
    // Call the current pattern function once, updating the 'leds' array
    gPatterns[gCurrentPatternNumber]();

    // send the 'leds' array out to the actual LED strip
    FastLED.show();

    // insert a delay to keep the framerate modest
    FastLED.delay(1000/FRAMES_PER_SECOND);

    WiFiClient client = server.available(); // Listen for incoming clients

    if (client)
    { // If a new client connects,
        currentTime = millis();
        previousTime = currentTime;
        Serial.println("New Client."); // print a message out in the serial port
        String currentLine = "";       // make a String to hold incoming data from the client
        while (client.connected() && currentTime - previousTime <= timeoutTime)
        { // loop while the client's connected
            currentTime = millis();
            if (client.available())
            {                           // if there's bytes to read from the client,
                char c = client.read(); // read a byte, then
                Serial.write(c);        // print it out the serial monitor
                header += c;
                if (c == '\n')
                { // if the byte is a newline character
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0)
                    {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();

                        // turns the GPIOs on and off
                        if (header.indexOf("GET /Red/on") >= 0)
                        {
                            Serial.println("Red on");
                            redState = "on";
                            gCurrentPatternNumber = Red;
                        }
                        else if (header.indexOf("GET /Red/off") >= 0)
                        {
                            Serial.println("Red off");
                            redState = "off";
                            gCurrentPatternNumber = None;
                        }
                        if (header.indexOf("GET /RedG/on") >= 0)
                        {
                            Serial.println("Red with glitter on");
                            redGState = "on";
                            gCurrentPatternNumber = RedG;
                        }
                        else if (header.indexOf("GET /RedG/off") >= 0)
                        {
                            Serial.println("Red with glitter off");
                            redGState = "off";
                            gCurrentPatternNumber = None;
                        }
                        if (header.indexOf("GET /Blue/on") >= 0)
                        {
                            Serial.println("Blue on");
                            blueState = "on";
                            gCurrentPatternNumber = Blue;
                        }
                        else if (header.indexOf("GET /Blue/off") >= 0)
                        {
                            Serial.println("Blue off");
                            blueState = "off";
                            gCurrentPatternNumber = None;
                        }
                        if (header.indexOf("GET /BlueG/on") >= 0)
                        {
                            Serial.println("Blue with Glitter on");
                            blueGState = "on";
                            gCurrentPatternNumber = BlueG;
                        }
                        else if (header.indexOf("GET /BlueG/off") >= 0)
                        {
                            Serial.println("Blue with Glitter off");
                            blueGState = "off";
                            gCurrentPatternNumber = None;
                        }
                        if (header.indexOf("GET /Green/on") >= 0)
                        {
                            Serial.println("Green on");
                            greenState = "on";
                            gCurrentPatternNumber = Green;
                        }
                        else if (header.indexOf("GET /Green/off") >= 0)
                        {
                            Serial.println("Green off");
                            greenState = "off";
                            gCurrentPatternNumber = None;
                        }
                        if (header.indexOf("GET /GreenG/on") >= 0)
                        {
                            Serial.println("Green with Glitter on");
                            greenGState = "on";
                            gCurrentPatternNumber = GreenG;
                        }
                        else if (header.indexOf("GET /GreenG/off") >= 0)
                        {
                            Serial.println("Green with Glitter off");
                            greenGState = "off";
                            gCurrentPatternNumber = None;
                        }
                        if (header.indexOf("GET /Pink/on") >= 0)
                        {
                            Serial.println("Pink on");
                            pinkState = "on";
                            gCurrentPatternNumber = Pink;
                        }
                        else if (header.indexOf("GET /Pink/off") >= 0)
                        {
                            Serial.println("Pink off");
                            pinkState = "off";
                            gCurrentPatternNumber = None;
                        }
                        if (header.indexOf("GET /PinkG/on") >= 0)
                        {
                            Serial.println("Pink with glitter on");
                            pinkGState = "on";
                            gCurrentPatternNumber = PinkG;
                        }
                        else if (header.indexOf("GET /PinkG/off") >= 0)
                        {
                            Serial.println("Pink with glitter off");
                            pinkGState = "off";
                            gCurrentPatternNumber = None;
                        }
                        if (header.indexOf("GET /Gold/on") >= 0)
                        {
                            Serial.println("Gold on");
                            goldState = "on";
                            gCurrentPatternNumber = Gold;
                        }
                        else if (header.indexOf("GET /Gold/off") >= 0)
                        {
                            Serial.println("Gold off");
                            goldState = "off";
                            gCurrentPatternNumber = None;
                        }
                        if (header.indexOf("GET /GoldG/on") >= 0)
                        {
                            Serial.println("Gold with Glitter on");
                            goldGState = "on";
                            gCurrentPatternNumber = GoldG;
                        }
                        else if (header.indexOf("GET /GoldG/off") >= 0)
                        {
                            Serial.println("Gold with Glitter off");
                            goldGState = "off";
                            gCurrentPatternNumber = None;
                        }
                        // if (header.indexOf("GET /Gold/on") >= 0)
                        // {
                        //     Serial.println("Gold on");
                        //     goldState = "on";
                        //     gCurrentPatternNumber = Gold;
                        // }
                        // else if (header.indexOf("GET /Gold/off") >= 0)
                        // {
                        //     Serial.println("Gold off");
                        //     goldState = "off";
                        //     gCurrentPatternNumber = None;
                        // }
                        if (header.indexOf("GET /RedGreen/off") >= 0)
                        {
                            Serial.println("Red/Green off");
                            redGreenState = "off";
                            gCurrentPatternNumber = None;
                        }
                        if (header.indexOf("GET /RedGreen/on") >= 0)
                        {
                            Serial.println("Red/Green on");
                            redGreenState = "on";
                            gCurrentPatternNumber = RedGreen;
                        }
                        else if (header.indexOf("GET /RedGreenG/off") >= 0)
                        {
                            Serial.println("Red/Green with Glitter off");
                            redGreenGState = "off";
                            gCurrentPatternNumber = None;
                        }
                        if (header.indexOf("GET /RedGreenG/on") >= 0)
                        {
                            Serial.println("Red/Green with Glitter on");
                            redGreenGState = "on";
                            gCurrentPatternNumber = RedGreenG;
                        }
                        // else if (header.indexOf("GET /RedGreenG/off") >= 0)
                        // {
                        //     Serial.println("Red/Green with Glitter off");
                        //     redGreenGState = "off";
                        //     gCurrentPatternNumber = None;
                        // }
                        else if (header.indexOf("GET /None/on") >= 0)
                        {
                            Serial.println("None on");
                            noneState = "on";
                            gCurrentPatternNumber = None;
                        }
                        else if (header.indexOf("GET /None/off") >= 0)
                        {
                            Serial.println("None off");
                            noneState = "off";
                            gCurrentPatternNumber = None;
                        }

                        // Display the HTML web page
                        client.println("<!DOCTYPE html><html>");
                        client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                        client.println("<link rel=\"icon\" href=\"data:,\">");
                        // CSS to style the on/off buttons
                        // Feel free to change the background-color and font-size attributes to fit your preferences
                        client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                        client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 8px 40px;");
                        client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                        client.println(".button2 {background-color: #555555;}</style></head>");

                        // Web Page Heading
                        client.println("<body><h1>ESP32 Web Server</h1>");

                        // If the redState is off, it displays the ON button
                        if (redState == "off")
                        {
                            client.println("<p><a href=\"/Red/on\"><button class=\"button\">Red</button></a></p>");
                        }
                        else
                        {
                            client.println("<p><a href=\"/Red/off\"><button class=\"button button2\">Red</button></a></p>");
                        }

                        if (redGState == "off")
                        {
                            client.println("<p><a href=\"/RedG/on\"><button class=\"button\">Red with Glitter</button></a></p>");
                        }
                        else
                        {
                            client.println("<p><a href=\"/RedG/off\"><button class=\"button button2\">Red with Glitter</button></a></p>");
                        }

                        // If the blueState is off, it displays the ON button
                        if (blueState == "off")
                        {
                            client.println("<p><a href=\"/Blue/on\"><button class=\"button\">Blue</button></a></p>");
                        }
                        else
                        {
                            client.println("<p><a href=\"/Blue/off\"><button class=\"button button2\">Blue</button></a></p>");
                        }

                        if (blueGState == "off")
                        {
                            client.println("<p><a href=\"/BlueG/on\"><button class=\"button\">Blue with Glitter</button></a></p>");
                        }
                        else
                        {
                            client.println("<p><a href=\"/BlueG/off\"><button class=\"button button2\">Blue with Glitter</button></a></p>");
                        }

                        // If the greenState is off, it displays the ON button
                        if (greenState == "off")
                        {
                            client.println("<p><a href=\"/Green/on\"><button class=\"button\">Green</button></a></p>");
                        }
                        else
                        {
                            client.println("<p><a href=\"/Green/off\"><button class=\"button button2\">Green</button></a></p>");
                        }

                        if (greenGState == "off")
                        {
                            client.println("<p><a href=\"/GreenG/on\"><button class=\"button\">Green with Glitter</button></a></p>");
                        }
                        else
                        {
                            client.println("<p><a href=\"/GreenG/off\"><button class=\"button button2\">Green with Glitter</button></a></p>");
                        }

                        if (pinkState == "off")
                        {
                            client.println("<p><a href=\"/Pink/on\"><button class=\"button\">Pink</button></a></p>");
                        }
                        else
                        {
                            client.println("<p><a href=\"/Pink/off\"><button class=\"button button2\">Pink</button></a></p>");
                        }

                        if (pinkGState == "off")
                        {
                            client.println("<p><a href=\"/PinkG/on\"><button class=\"button\">Pink with Glitter</button></a></p>");
                        }
                        else
                        {
                            client.println("<p><a href=\"/PinkG/off\"><button class=\"button button2\">Pink with Glitter</button></a></p>");
                        }

                        if (goldState == "off")
                        {
                            client.println("<p><a href=\"/Gold/on\"><button class=\"button\">Gold</button></a></p>");
                        }
                        else
                        {
                            client.println("<p><a href=\"/Gold/off\"><button class=\"button button2\">Gold</button></a></p>");
                        }

                        if (goldGState == "off")
                        {
                            client.println("<p><a href=\"/GoldG/on\"><button class=\"button\">Gold with Glitter</button></a></p>");
                        }
                        else
                        {
                            client.println("<p><a href=\"/GoldG/off\"><button class=\"button button2\">Gold with Glitter</button></a></p>");
                        }

                        if (redGreenState == "off")
                        {
                            client.println("<p><a href=\"/RedGreen/on\"><button class=\"button\">Red/Green</button></a></p>");
                        }
                        else
                        {
                            client.println("<p><a href=\"/RedGreen/off\"><button class=\"button button2\">Red/Green</button></a></p>");
                        }

                        if (redGreenGState == "off")
                        {
                            client.println("<p><a href=\"/RedGreenG/on\"><button class=\"button\">Red/Green with Glitter</button></a></p>");
                        }
                        else
                        {
                            client.println("<p><a href=\"/RedGreenG/off\"><button class=\"button button2\">Red/Green with Glitter</button></a></p>");
                        }

                        // If the noneState is off, it displays the ON button
                        if (noneState == "off")
                        {
                            client.println("<p><a href=\"/None/on\"><button class=\"button\">No Lights</button></a></p>");
                        }
                        else
                        {
                            client.println("<p><a href=\"/None/off\"><button class=\"button button2\">No Lights</button></a></p>");
                        }
                        client.println("</body></html>");

                        // The HTTP response ends with another blank line
                        client.println();
                        // Break out of the while loop
                        break;
                    }
                    else
                    { // if you got a newline, then clear currentLine
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                {                     // if you got anything else but a carriage return character,
                    currentLine += c; // add it to the end of the currentLine
                }
            }
        }
        // Clear the header variable
        header = "";
        // Close the connection
        client.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
    }
}

void red()
{
    for (int i = 0; i < NUM_LEDS; i += 3)
        leds[i] = CRGB::Red;
    for (int i = 1; i < NUM_LEDS; i += 3)
        leds[i] = CRGB::DarkRed;
    for (int i = 2; i < NUM_LEDS; i += 3)
        leds[i] = CRGB::Maroon;
}

void redG()
{
    red();
    addGlitter(80);
}

void blue()
{
    for (int i = 0; i < NUM_LEDS; i += 2)
        leds[i] = CRGB::DeepSkyBlue;
    for (int i = 1; i < NUM_LEDS; i += 2)
        leds[i] = CRGB::DarkBlue;
        
    // FastLED.show();
}

void blueG()
{
    blue();
    addGlitter(80);
}

void green()
{
    for (int i = 0; i < NUM_LEDS; i += 5)
        leds[i] = CRGB::DarkGreen;
    for (int i = 1; i < NUM_LEDS; i += 5)
        leds[i] = CRGB::Chartreuse;
    for (int i = 2; i < NUM_LEDS; i += 5)
        leds[i] = CRGB::ForestGreen;
    for (int i = 3; i < NUM_LEDS; i += 5)
        leds[i] = CRGB::Green;
    for (int i = 4; i < NUM_LEDS; i += 5)
        leds[i] = CRGB::LawnGreen;
}

void greenG()
{
    green();
    addGlitter(80);
}

void addGlitter(fract8 chanceOfGlitter)
{
    if (random8() < chanceOfGlitter)
    {
        leds[random16(NUM_LEDS)] += CRGB::White;
    }
}

void none()
{
    for (int i = 0; i < NUM_LEDS; i++)
        leds[i] = CRGB::Black;
}

void pink()
{
    for (int i = 0; i < NUM_LEDS; i += 2)
        leds[i] = CRGB::DeepPink;
    for (int i = 1; i < NUM_LEDS; i += 2)
        leds[i] = CRGB::MediumVioletRed;
 
    // FastLED.show();
}

void pinkG()
{
    pink();
    addGlitter(80);
}

void gold()
{
    for (int i = 0; i < NUM_LEDS; i += 3)
        leds[i] = CRGB::Gold;
    for (int i = 1; i < NUM_LEDS; i += 3)
        leds[i] = CRGB::Yellow;
     for (int i = 2; i < NUM_LEDS; i += 3)
        leds[i] = CRGB::Orange;

    // FastLED.show();
}

void goldG()
{
    gold();
    addGlitter(80);
}

void redGreen()
{
    for (int i = 0; i < NUM_LEDS; i += 6)
        leds[i] = CRGB::Red;
    for (int i = 1; i < NUM_LEDS; i += 6)
        leds[i] = CRGB::DarkRed;
    for (int i = 2; i < NUM_LEDS; i += 6)
        leds[i] = CRGB::Maroon;
    for (int i = 3; i < NUM_LEDS; i += 6)
        leds[i] = CRGB::DarkGreen;
    for (int i = 4; i < NUM_LEDS; i += 6)
        leds[i] = CRGB::Chartreuse;
    for (int i = 5; i < NUM_LEDS; i += 6)
        leds[i] = CRGB::ForestGreen;
}

void redGreenG()
{
    redGreen();
    addGlitter(80);
}
