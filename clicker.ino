#include <hidboot.h>
#include <usbhub.h>
#include <Mouse.h>

#include <LiquidCrystal.h>
//LCD pin to Arduino
const int pin_RS = 8; 
const int pin_EN = 9; 
const int pin_d4 = 4; 
const int pin_d5 = 5;
const int pin_d6 = 6; 
const int pin_d7 = 7; 
const int pin_BL = 10; 
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

USB     Usb;
USBHub     Hub(&Usb);
HIDBoot<USB_HID_PROTOCOL_MOUSE>    HidMouse(&Usb);


byte bf[3];

int inputInt = {0};
float inputFloat = 0.0;
char inputCsvString[12];
int x;
int y;
bool middleClicked {false};
int shoot{0};
int cycleNum{0};


unsigned long firstClicked{0};

void MoveMouseToXY(long x, long y) {
  long max = max(abs(x), abs(y));
  int count = (int) (max / 127);
  signed char stepX = x / (count + 1);
  signed char stepY = y / (count + 1);
  for (int i = 0; i < count; i++) {
    Mouse.move(stepX, stepY);
  }
  signed char resX = x - (stepX * count);
  signed char resY = y - (stepY * count);
  if (resX != 0 || resY != 0) {
    Mouse.move(resX, resY);
  }
}

class MouseRptParser : public MouseReportParser
{
  protected:
    void OnMouseMove  (MOUSEINFO *mi);
    void OnLeftButtonUp (MOUSEINFO *mi);
    void OnLeftButtonDown (MOUSEINFO *mi);
    void OnRightButtonUp  (MOUSEINFO *mi);
    void OnRightButtonDown  (MOUSEINFO *mi);
    void OnMiddleButtonUp (MOUSEINFO *mi);
    void OnMiddleButtonDown (MOUSEINFO *mi);

};

MouseRptParser Prs;

void MouseRptParser::OnMouseMove(MOUSEINFO *mi)
{
  //Serial.print("dx=");
  //Serial.print(mi->dX, DEC);
  //Serial.print(" dy=");
  //Serial.println(mi->dY, DEC);
  MoveMouseToXY(mi->dX, mi->dY);
};
void MouseRptParser::OnLeftButtonUp (MOUSEINFO *mi)
{
  Mouse.release(MOUSE_LEFT);
  middleClicked = true;
  //Serial.println("L Butt Up");
};
void MouseRptParser::OnLeftButtonDown (MOUSEINFO *mi)
{
  Mouse.press(MOUSE_LEFT);
  middleClicked = false;
  //Serial.println("L Butt Dn");
};
void MouseRptParser::OnRightButtonUp  (MOUSEINFO *mi)
{
  Mouse.release(MOUSE_RIGHT);
  //Serial.println("R Butt Up");
};
void MouseRptParser::OnRightButtonDown  (MOUSEINFO *mi)
{
  Mouse.press(MOUSE_RIGHT);
  //Serial.println("R Butt Dn");
};
void MouseRptParser::OnMiddleButtonUp (MOUSEINFO *mi)
{

  Mouse.release(MOUSE_MIDDLE);
  
  //Serial.println("M Butt Up");
};
void MouseRptParser::OnMiddleButtonDown (MOUSEINFO *mi)
{
  Mouse.press(MOUSE_MIDDLE);
  
  //Serial.println("M Butt Dn");
};

void setup()
{
  //Serial.begin( 115200 );
  //Serial.println("Start");

  if (Usb.Init() == -1)
    //Serial.println("OSC did not start.");

  delay( 200 );
  Mouse.begin();
  HidMouse.SetReportParser(0, &Prs);
}



void loop()
{
  int period = random(55, 300);
  Usb.Task();
  if (middleClicked) {
    // do what you want on your first action
    firstClicked = millis();
    cycleNum = 1;
  }
  if (cycleNum > 0) {
      if(millis() > firstClicked + period){
        firstClicked = millis();
        Mouse.click();
        //Serial.println(period);
    }
  }
}
