// Simple OSC receiver
 
import oscP5.*;
import netP5.*;

OscP5 oscP5;
boolean isMouseDown = false;
int mX = 0, mY = 0;

void setup() {
  size(400,400);
  frameRate(25);
  /* start oscP5, listening for incoming messages at port 12000 */
  oscP5 = new OscP5(this, 12000);
  
}


void draw() {
  background(0);
  fill(255);
  textSize(24);
  if (isMouseDown) {
    text("mouse down", 20, 20);
  }
  else {
    text("mouse up", 20, 20);
  }
  text("cursor at " + mX + " " + mY, 20, 40);
  ellipse(mX, mY, 10, 10);
}


/* incoming osc message are forwarded to the oscEvent method. */
void oscEvent(OscMessage theOscMessage) {

  if (theOscMessage.checkAddrPattern("/mouseDown")==true) {
    isMouseDown = true;
  }
  else if (theOscMessage.checkAddrPattern("/mouseUp")==true) {
    isMouseDown = false;
  }
  else if (theOscMessage.checkAddrPattern("/mouseMoved")==true) {
    int firstInt = theOscMessage.get(0).intValue();
    int secondInt = theOscMessage.get(1).intValue();
    mX = firstInt;
    mY = secondInt;
  }
}
