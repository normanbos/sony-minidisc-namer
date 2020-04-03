// modified for use with Sony RM-D27M
// based on https://hackaday.io/project/165504-sony-minidisc-namer by smartroad

#include <IRremote.h>
#include "defines.h"

IRsend irsend;

char inputChars[64];
byte charCount = 0;

byte mode = 0; // Capticals = 0, Lowercase = 1
byte targetMode = 0;
boolean commandMode = true; // True = control playback | False = name entry mode

int totalCount = 0;
byte blockCount = 0;

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  helpText();
}

void loop()
{

  if (commandMode)
  {
    if (charCount > 0)
    {
      Serial.println(F("\n\r\u001b[34;1mCONFIRM NAME INPUT\u001b[0m"));
      Serial.println(F("Y - Yes or N - NO"));
    }
    else
    {
      Serial.print(F("\n\r\n\rEnter Command (H for help): "));
    }
  }

  while (!Serial.available())
  {
  }

  if (commandMode)
  {

    char serialChar = Serial.read();
    if (charCount == 0)
    {
      if (serialChar == 'b')
      {
        Serial.println(F("Next Track"));
        sendButton(AMSNEXT);
      }
      else if (serialChar == 'z')
      {
        Serial.println(F("Previous Track"));
        sendButton(AMSPREV);
      }
      else if (serialChar == 'x')
      {
        Serial.println(F("Play"));
        sendButton(PLAY);
      }
      else if (serialChar == 'c')
      {
        Serial.println(F("Pause"));
        sendButton(PAUSE);
      }
      else if (serialChar == 'v')
      {
        Serial.println(F("Stop"));
        sendButton(STOP);
      }
      else if (serialChar == 'l')
      {
        Serial.println(F("Eject"));
        sendButton(EJECT);
      }
      else if (serialChar == 's')
      {
        Serial.println(F("Starting Record"));
        sendButton(RECORD);
      }
      else if (serialChar == 'S')
      {
        Serial.println(F("Music Sync Recording"));
        sendButton(MUSICSYNC);
      }
      else if (serialChar == 'd')
      {
        Serial.println(F("Display Mode"));
        sendButton(DISPLAY);
      }
      else if (serialChar == 'D')
      {
        Serial.println(F("Scrolling Display"));
        sendButton(SCROLL);
      }
      else if (serialChar == 'r')
      {
        Serial.println(F("Repeat Mode"));
        sendButton(REPEATMODE);
      }
      else if (serialChar == 'R')
      {
        Serial.println(F("Section Repeat (A<>B)"));
        sendButton(ATOB);
      }
      else if (serialChar == '1')
      {
        Serial.print(F("1"));
        sendButton(NUMBER1);
      }
      else if (serialChar == '2')
      {
        Serial.print(F("2"));
        sendButton(NUMBER2);
      }
      else if (serialChar == '3')
      {
        Serial.print(F("3"));
        sendButton(NUMBER3);
      }
      else if (serialChar == '4')
      {
        Serial.print(F("4"));
        sendButton(NUMBER4);
      }
      else if (serialChar == '5')
      {
        Serial.print(F("5"));
        sendButton(NUMBER5);
      }
      else if (serialChar == '6')
      {
        Serial.print(F("6"));
        sendButton(NUMBER6);
      }
      else if (serialChar == '7')
      {
        Serial.print(F("7"));
        sendButton(NUMBER7);
      }
      else if (serialChar == '8')
      {
        Serial.print(F("8"));
        sendButton(NUMBER8);
      }
      else if (serialChar == '9')
      {
        Serial.print(F("9"));
        sendButton(NUMBER9);
      }
      else if (serialChar == '0')
      {
        Serial.print(F("0"));
        sendButton(NUMBER10);
      }
      else if (serialChar == '=' || serialChar == '+')
      {
        Serial.print(F(">10"));
        sendButton(LARGER25);
      }
      else if (serialChar == '#')
      {
        Serial.print(F("CLEARING MEMUSAGE"));
        totalCount = 0;
        blockCount = 0;
        memUsed();
      }
      else if (serialChar == '`')
      { // change to naming mode
        Serial.println(F("Name Mode"));
        commandMode = false;
        memUsed();
        Serial.println(F("\n\rType name and press enter to send:\u001b[37;1m"));
      }
      else if (serialChar == 'H' || serialChar == 'h')
      { // stop
        Serial.println(F("Help\r\n\r\n"));
        helpText();
      }
    }
    else
    {
      if (serialChar == 'Y' || serialChar == 'y')
      { // confirm name
        totalCount += charCount;
        blockCount += (charCount + 6) / 7;
        memUsed();
        charCount = 0;
        sendButton(NAME);
      }
      else if (serialChar == 'N' || serialChar == 'n')
      { // cancel name
        memUsed();
        charCount = 0;
        sendButton(STOP);
      }
    }
  }
  else
  {
    inputChars[charCount] = Serial.read();
    if (inputChars[charCount] == 10 || inputChars[charCount] == 13)
    {
      Serial.print(F("\u001b[0m"));
      if (nameCheck())
        sendName();
      else
        charCount = 0;
      commandMode = true;
    }
    else if (inputChars[charCount] == 127)
    {
      Serial.print(inputChars[charCount]);
      charCount--;
    }
    else
    {
      Serial.print(inputChars[charCount]);
      charCount++;
    }
  }
}

void memUsed()
{
  float countPercent = (float(totalCount) / 1785) * 100;
  float blockPercent = (float(blockCount) / 255) * 100;
  Serial.print(F("\n\rTotal characters used: \u001b[1m"));
  if (countPercent >= 75 && countPercent < 90)
    Serial.print(F("\u001b[33m"));
  else if (countPercent >= 90)
    Serial.print(F("\u001b[31m"));
  else
    Serial.print(F("\u001b[32m"));
  Serial.print(countPercent);
  Serial.print(F("%\u001b[0m\u001b[0m\n\rTotal blocks used: \u001b[1m"));
  if (blockPercent >= 75 && blockPercent < 90)
    Serial.print(F("\u001b[33m"));
  else if (blockPercent >= 90)
    Serial.print(F("\u001b[31m"));
  else
    Serial.print(F("\u001b[32m"));
  Serial.print(blockPercent);
  Serial.print(F("%\u001b[0m\n\r"));
}

void sendName()
{
  Serial.println(F("\n\r\n\rActivating Name Edit on Player - \u001b[31mPress any key when player ready\u001b[0m"));
  sendButton(NAME);
  mode = 0;
  targetMode = 0;
  // delay(2000);
  while (!Serial.available())
  {
  }

  char purge = Serial.read(); // dump the character

  Serial.print(F("\n\rSending: \u001b[32;1m"));

  for (byte c = 0; c < charCount; c++)
  {
    if (inputChars[c] >= 65 && inputChars[c] <= 90)
    { // Check if char is capital and switch to correct mode
      targetMode = 0;
    }
    else if (inputChars[c] >= 97 && inputChars[c] <= 122)
    { // Check if char is lower and switch to correct mode
      targetMode = 1;
    }

    while (mode != targetMode)
    {
      sendButton(CHAR);
      delay(1000);
      mode++;
      if (mode > 1)
        mode = 0;
    }

    if (inputChars[c] == 'A')
    {
      Serial.print(inputChars[c]);
      sendCmd(CONTINUE, 1);
    }
    else if (inputChars[c] == 'B')
    {
      Serial.print(inputChars[c]);
      sendCmd(SHUFFLE, 1);
    }
    else if (inputChars[c] == 'C')
    {
      Serial.print(inputChars[c]);
      sendCmd(PROGRAM, 1);
    }
    else if (inputChars[c] == 'D')
    {
      Serial.print(inputChars[c]);
      sendCmd(KEY_D, 1);
    }
    else if (inputChars[c] == 'E')
    {
      Serial.print(inputChars[c]);
      sendCmd(KEY_E, 1);
    }
    else if (inputChars[c] == 'F')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER1, 1);
    }
    else if (inputChars[c] == 'G')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER2, 1);
    }
    else if (inputChars[c] == 'H')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER3, 1);
    }
    else if (inputChars[c] == 'I')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER4, 1);
    }
    else if (inputChars[c] == 'J')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER5, 1);
    }
    else if (inputChars[c] == 'K')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER6, 1);
    }
    else if (inputChars[c] == 'L')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER7, 1);
    }
    else if (inputChars[c] == 'M')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER8, 1);
    }
    else if (inputChars[c] == 'N')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER9, 1);
    }
    else if (inputChars[c] == 'O')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER10, 1);
    }
    else if (inputChars[c] == 'P')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER11, 1);
    }
    else if (inputChars[c] == 'Q')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER12, 1);
    }
    else if (inputChars[c] == 'R')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER13, 1);
    }
    else if (inputChars[c] == 'S')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER14, 1);
    }
    else if (inputChars[c] == 'T')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER15, 1);
    }
    else if (inputChars[c] == 'U')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER16, 1);
    }
    else if (inputChars[c] == 'V')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER17, 1);
    }
    else if (inputChars[c] == 'W')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER18, 1);
    }
    else if (inputChars[c] == 'X')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER19, 1);
    }
    else if (inputChars[c] == 'Y')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER20, 1);
    }
    else if (inputChars[c] == 'Z')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER21, 1);
    }

    else if (inputChars[c] == 'a')
    {
      Serial.print(inputChars[c]);
      sendCmd(CONTINUE, 1);
    }
    else if (inputChars[c] == 'b')
    {
      Serial.print(inputChars[c]);
      sendCmd(SHUFFLE, 1);
    }
    else if (inputChars[c] == 'c')
    {
      Serial.print(inputChars[c]);
      sendCmd(PROGRAM, 1);
    }
    else if (inputChars[c] == 'd')
    {
      Serial.print(inputChars[c]);
      sendCmd(KEY_D, 1);
    }
    else if (inputChars[c] == 'e')
    {
      Serial.print(inputChars[c]);
      sendCmd(KEY_E, 1);
    }
    else if (inputChars[c] == 'f')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER1, 1);
    }
    else if (inputChars[c] == 'g')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER2, 1);
    }
    else if (inputChars[c] == 'h')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER3, 1);
    }
    else if (inputChars[c] == 'i')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER4, 1);
    }
    else if (inputChars[c] == 'j')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER5, 1);
    }
    else if (inputChars[c] == 'k')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER6, 1);
    }
    else if (inputChars[c] == 'l')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER7, 1);
    }
    else if (inputChars[c] == 'm')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER8, 1);
    }
    else if (inputChars[c] == 'n')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER9, 1);
    }
    else if (inputChars[c] == 'o')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER10, 1);
    }
    else if (inputChars[c] == 'p')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER11, 1);
    }
    else if (inputChars[c] == 'q')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER12, 1);
    }
    else if (inputChars[c] == 'r')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER13, 1);
    }
    else if (inputChars[c] == 's')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER14, 1);
    }
    else if (inputChars[c] == 't')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER15, 1);
    }
    else if (inputChars[c] == 'u')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER16, 1);
    }
    else if (inputChars[c] == 'v')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER17, 1);
    }
    else if (inputChars[c] == 'w')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER18, 1);
    }
    else if (inputChars[c] == 'x')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER19, 1);
    }
    else if (inputChars[c] == 'y')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER20, 1);
    }
    else if (inputChars[c] == 'z')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER21, 1);
    }

    else if (inputChars[c] == 39 || inputChars[c] == 96)
    { // ' and `
      Serial.print(inputChars[c]);
      sendCmd(NUMBER25, 1);
    }
    else if (inputChars[c] == 45 || inputChars[c] == 95)
    { // - or _
      Serial.print(inputChars[c]);
      sendCmd(NUMBER22, 1);
    }
    else if (inputChars[c] == 47 || inputChars[c] == 92)
    { // slash or backslash
      Serial.print(inputChars[c]);
      sendCmd(LARGER25, 1);
    }
    else if (inputChars[c] == 44)
    { // , not supported
      Serial.print(inputChars[c]);
      sendCmd(NUMBER1, 1);
    }
    else if (inputChars[c] == '.')
    {
      Serial.print(inputChars[c]);
      sendCmd(NUMBER24, 1);
    }
    else if (inputChars[c] == '(')
    {
      Serial.print(inputChars[c]);
      sendCmd(ASPACE, 1);
    }
    else if (inputChars[c] == ')')
    {
      Serial.print(inputChars[c]);
      sendCmd(MSCAN, 1);
    }
    else if (inputChars[c] == ':')
    { // : not supported
      Serial.print(inputChars[c]);
      sendCmd(NUMBER1, 8);
    }
    else if (inputChars[c] == '!')
    {
      Serial.print(inputChars[c]);
      sendCmd(ATOB, 1);
    }
    else if (inputChars[c] == '?')
    {
      Serial.print(inputChars[c]);
      sendCmd(REPEATMODE, 1);
    }

    else if (inputChars[c] == '1')
    {
      Serial.print(inputChars[c]);
      sendNumCmd(NUMBER1, 1);
    }
    else if (inputChars[c] == '2')
    {
      Serial.print(inputChars[c]);
      sendNumCmd(NUMBER2, 1);
    }
    else if (inputChars[c] == '3')
    {
      Serial.print(inputChars[c]);
      sendNumCmd(NUMBER3, 1);
    }
    else if (inputChars[c] == '4')
    {
      Serial.print(inputChars[c]);
      sendNumCmd(NUMBER4, 1);
    }
    else if (inputChars[c] == '5')
    {
      Serial.print(inputChars[c]);
      sendNumCmd(NUMBER5, 1);
    }
    else if (inputChars[c] == '6')
    {
      Serial.print(inputChars[c]);
      sendNumCmd(NUMBER6, 1);
    }
    else if (inputChars[c] == '7')
    {
      Serial.print(inputChars[c]);
      sendNumCmd(NUMBER7, 1);
    }
    else if (inputChars[c] == '8')
    {
      Serial.print(inputChars[c]);
      sendNumCmd(NUMBER8, 1);
    }
    else if (inputChars[c] == '9')
    {
      Serial.print(inputChars[c]);
      sendNumCmd(NUMBER9, 1);
    }
    else if (inputChars[c] == '0')
    {
      Serial.print(inputChars[c]);
      sendNumCmd(NUMBER10, 1);
    }

    else if (inputChars[c] == ' ')
    { // space
      Serial.print(inputChars[c]);
      sendCmd(NUMBER23, 1);
    }
  }

  Serial.print(F("\u001b[0m\n\r\n\rTotal characters needed: "));
  Serial.print(charCount);
  Serial.print(F("\n\rTotal blocks needed: "));
  Serial.print((charCount + 6) / 7);
  Serial.print(F("\n\r"));
}

void sendCmd(int command, byte repeats)
{
  for (int i = 0; i <= repeats - 1; i++)
  {
    sendButton(command);
    delay(100);
  }
}

void sendButton(int bts)
{
  digitalWrite(9, LOW);
  for (int i = 0; i < 3; i++)
  {
    irsend.sendSony(bts, 12);
    delay(25);
  }
  digitalWrite(9, HIGH);
}

void sendNumCmd(int command, byte repeats)
{
  for (int i = 0; i <= repeats - 1; i++)
  {
    sendButton(NUM);
    delay(1000);
    sendButton(command);
    delay(100);
    sendButton(NUM);
    delay(1000);
  }
}

void helpText()
{
  Serial.print(F("\u001b[1m\u001b[7mMinidisc Namer\u001b[0m\n\r\
\n\r\
\u001b[1mCommands:\u001b[0m\n\r\
\n\r\
\u001b[32mGeneral:\n\r\
\u001b[0mz    Previous Track\n\r\
x    Play\n\r\
c    Pause\n\r\
v    Stop\n\r\
b    Next Track\n\r\
l    Eject\n\r\
\n\r\
\u001b[31mRecording:\n\r\
\u001b[0ms    Record\n\r\
S    Music Sync Recording\n\r\
i    Input Selection\n\r\
q    Recording Mode\n\r\
\n\r\
\u001b[33mNumbers:\n\r\
\u001b[0m0-9  Number Entry\n\r\
=/+  >10 Entry\n\r\
\n\r\
\u001b[34;1mDisplay:\n\r\
\u001b[0md    Display\n\r\
D    Scroll Display\n\r\
\n\r\
\u001b[36mPlaymodes/Repeating:\n\r\
\u001b[0mp    Play Mode (Normal/Shuffle/Program)\n\r\
r    Repeat\n\r\
R    A <-> B\n\r\
\n\r\
\u001b[32;1mNaming Disc/Tracks:\n\r\
\u001b[0m`    Name Entry Mode\n\r\
#    Reset Memory Usage Stats\n\r\
\n\r\
\u001b[37;1mh    This help list\u001b[0m\n\r\n\r"));
}

boolean nameCheck()
{
  Serial.print(F("\n\r\n\r\n\rSend this to player (Y/N):\n\r\u001b[37;1m"));
  for (byte c = 0; c < charCount; c++)
  {
    Serial.print(inputChars[c]);
  }
  Serial.print(F("\n\r\u001b[0m"));
  while (!Serial.available())
  {
  }
  char serialChar = Serial.read();
  if (serialChar == 'Y' || serialChar == 'y')
    return 1;
  else
    return 0;
}
