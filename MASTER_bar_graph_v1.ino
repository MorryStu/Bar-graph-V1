/******************************************************************
  This is an example for the Adafruit RA8875 Driver board for TFT displays
  ---------------> http://www.adafruit.com/products/1590
  The RA8875 is a TFT driver for up to 800x480 dotclock'd displays
  It is tested to work with displays in the Adafruit shop. Other displays
  may need timing adjustments and are not guanteed to work.
  Adafruit invests time and resources providing this open
  source code, please support Adafruit and open-source hardware
  by purchasing products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, check license.txt for more information.
  All text above must be included in any redistribution.
 ******************************************************************/
// Also borrowed code from here --> https://www.hackster.io/LightPro/tft-graphing-bar-charts-185436 //
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"
#define RA8875_INT 3
#define RA8875_CS 10
#define RA8875_RESET 9
Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
uint16_t tx, ty;
// Global Variables
int valueBlock[4];
int posBlock[4];
int prevPosBlock[4];
// Editable Variables
bool proDebug = 1;                          // TODO - Debugging? 0 = No, 1 = Yes
bool displayValues = true;                  // EDIT - Display Value of Blocks?

uint16_t graphColor = RA8875_RED;                 // EDIT - Prefered Colour of Graph
uint16_t blockColor = RA8875_GREEN;                // EDIT - Prefered Colour of Blocks

String graphName = "Bar Chart";             // EDIT - Title of Graph
String graphBlock[] = {"Pot1", "Pot2"};   // EDIT - Names of Blocks

int graphRange = 1024;                      // EDIT - Max possible number of block, eg Potentiometer = from 0 to 1024, so 1024
int numberOfBlocks = 4;                     // EDIT - Number of Blocks to display

// calculate values
const int numberOfMarks = (numberOfBlocks * 2); // calculates amount of lines that mark x axis
const int originX = 45; // the origin of the graph on the x axis
const int originY = 440; // the origin of the graph on the y axis
const int sizeX = 780; // the size of the chart on the x axis (lenght)
const int sizeY = 350; // the size of the chart on the y axis (height)

int boxSize = (sizeX / numberOfMarks); // calculates the size between the marks
int mark[] = {boxSize, (boxSize * 2), (boxSize * 3), (boxSize * 4), (boxSize * 5), (boxSize * 6), (boxSize * 7), (boxSize * 8)}; // calculates the position of the marks

const int minorSizeY = (originY + 10); // calculates the y origin of the marks
const int minorSizeX = (originX - 10); // calculates the x origin of the numbers

int numberSize = (sizeY / 10); // calculates the size between the numbers
int number[] = {numberSize, (numberSize * 2), (numberSize * 3), (numberSize * 4), (numberSize * 5), (numberSize * 6)}; // calculates the position of the numbers

int numberValue = (graphRange / 10); // calculates the value per number
int val[] = {graphRange, (numberValue * 5), (numberValue * 4), (numberValue * 3), (numberValue * 2), numberValue}; // calculates the value of each number


void drawGraph()
{ Serial.begin(9600);
  Serial.println("RA8875 start");

  /* Initialize the display using 'RA8875_480x80', 'RA8875_480x128', 'RA8875_480x272' or 'RA8875_800x480' */
  if (!tft.begin(RA8875_800x480))
  {
    Serial.println("RA8875 Not Found!");
    while (1);
  }

  Serial.println("Found RA8875");

  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255);
  tft.textMode();

  tft.fillScreen(RA8875_BLACK);

  
  // draw outline
  tft.drawLine(originX, originY, (originX + sizeX), originY, RA8875_WHITE);
  tft.drawLine(originX, originY, originX, (originY - sizeY), RA8875_WHITE);

  // draw lables
  for (int i = 0; i < numberOfMarks; i++)
  {
    tft.drawLine(mark[i], originY, mark[i], minorSizeY, RA8875_BLACK);
  }

  // draw lable names
  for (int i = 0; i < numberOfMarks; i += 2)
  {
    tft.setCursor((mark[i] + 6), (originY + 10));
    tft.setTextColor(RA8875_RED);
    tft.setTextSize(2);
    tft.println(graphBlock[i / 2]);
  }

  // draw numbers
  for (int i = 0; i < 6; i++)
  {
    tft.drawLine(originX, (originY - number[i]), minorSizeX, (originY - number[i]), RA8875_GREEN);
  }

  // draw number values
  for (int i = 0; i < 6; i++)
  {
    tft.textSetCursor(480, 10);
    tft.textColor(RA8875_GREEN, RA8875_BLACK);
    tft.setTextSize(1);
    //tft.textWrite(val[i]);
  }
}

void graph()
{
  // get the values of the sensors
  valueBlock[0] = analogRead(A0);
  valueBlock[1] = analogRead(A1);
  valueBlock[2] = analogRead(A2);
  valueBlock[3] = analogRead(A3);
  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255);
  tft.textMode();
  tft.textSetCursor(10, 10);
  tft.textColor(RA8875_WHITE, RA8875_BLACK);
  tft.textEnlarge(8);
  tft.textWrite("  Temp  Oil   BOOST  EGR ");
  if (proDebug)
  
  {
    Serial.println(valueBlock[0]);
    Serial.println(valueBlock[1]);
    Serial.println(valueBlock[2]);
    Serial.println(valueBlock[3]);

    //Serial.println("");
  }

  // map the sensor values to the graph size
  for (int i = 0; i < numberOfBlocks; i++)
  {
    posBlock[i] = map(valueBlock[i], 0, graphRange, originY, (originY - sizeY));
  }

  if (proDebug)
  {
    Serial.println(posBlock[0]);
    Serial.println(posBlock[1]);
    Serial.println(posBlock[2]);
    Serial.println(posBlock[3]);
    Serial.println("");
  }

  // draw the blocks - draw only if value differs
  for (int i = 0; i < numberOfBlocks; i++)
  {
    if (posBlock[i] > (prevPosBlock[i] + 2) || posBlock[i] < (prevPosBlock[i] - 2))
    {
      prevPosBlock[i] = posBlock[i];
      tft.fillRect((mark[i * 2] + 1), (originY - sizeY), (boxSize - 1), sizeY, RA8875_YELLOW);
      delay(10);
      tft.fillRect((mark[i * 2] + 1), posBlock[i], (boxSize - 1), (originY - posBlock[i]), RA8875_GREEN);
    }
  }

  // display values of sensors if mode selected
  if (displayValues)
  {
    for (int i = 0; i < numberOfMarks; i += 2)
    {
      tft.textSetCursor(10, 10);
      tft.textColor(RA8875_RED, RA8875_WHITE);
      tft.setTextSize(2);
      //tft.textWrite(valueBlock[i / 2]);
    }
  }
}

void setup()
{
  
  {
    Serial.begin(9600);
    while (!Serial) {};
  }
  {
    delay(500);

    drawGraph(); // draws the graph
  }
}
void loop()
{
  graph(); // graphs the values on the graph
  delay(500);
}
