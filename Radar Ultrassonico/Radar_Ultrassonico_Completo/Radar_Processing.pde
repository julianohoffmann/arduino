import processing.serial.*;

Serial myPort;
String data = "";
int iAngle = 0;
int iDistance = 0;

void setup() {
  size(1366, 700);
  smooth();
  println(Serial.list());
  myPort = new Serial(this, "COM3", 9600); // Altere COM3 para sua porta
  myPort.bufferUntil('.');
}

void draw() {
  fill(0, 25);
  noStroke();
  rect(0, 0, width, height);
  drawRadar();
  drawSweep();
  drawObject();
  drawText();
}

void serialEvent(Serial myPort) {
  data = myPort.readStringUntil('.');
  if (data != null && data.contains(",")) {
    data = trim(data.substring(0, data.length()-1));
    String[] parts = split(data, ',');
    if (parts.length == 2) {
      iAngle = int(parts[0]);
      iDistance = int(parts[1]);
    }
  }
}

void drawRadar() {
  pushMatrix();
  translate(width/2, height);
  stroke(0, 255, 0);
  noFill();
  strokeWeight(2);
  for (int r = 400; r <= 1200; r += 300) {
    arc(0, 0, r, r, PI, TWO_PI);
  }
  for (int a = 0; a <= 180; a += 30) {
    line(0, 0, 600 * cos(radians(a)), -600 * sin(radians(a)));
  }
  popMatrix();
}

void drawSweep() {
  pushMatrix();
  translate(width/2, height);
  stroke(0, 255, 0);
  strokeWeight(3);
  line(0, 0, 600 * cos(radians(iAngle)), -600 * sin(radians(iAngle)));
  popMatrix();
}

void drawObject() {
  if (iDistance > 0 && iDistance < 40) {
    pushMatrix();
    translate(width/2, height);
    stroke(255, 0, 0);
    strokeWeight(8);
    float distPix = map(iDistance, 0, 40, 0, 600);
    line(distPix * cos(radians(iAngle)), -distPix * sin(radians(iAngle)), 600 * cos(radians(iAngle)), -600 * sin(radians(iAngle)));
    popMatrix();
  }
}

void drawText() {
  fill(0);
  noStroke();
  rect(0, 650, width, 60);
  fill(0, 255, 0);
  textSize(24);
  textAlign(LEFT);
  text("Angle: " + iAngle + "°", 50, 680);
  text("Distance: " + iDistance + " cm", 300, 680);
  if (iDistance > 40) {
    text("Status: Out of Range", 600, 680);
  } else {
    text("Status: In Range", 600, 680);
  }
}
