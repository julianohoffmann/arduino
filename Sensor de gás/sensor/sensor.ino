int smoke = 0;
bool estadoLED = false;

void setup()
{
  pinMode(7, INPUT);
  pinMode(6, OUTPUT); // LED
}

void loop()
{
  smoke = digitalRead(7);

  if(smoke == 0)
  {
    // Sirene + LED piscando
    for(int i = 1000; i < 2000; i += 50){
      tone(5, i);

      // alterna LED (pisca)
      estadoLED = !estadoLED;
      digitalWrite(6, estadoLED);

      delay(50);
    }

    for(int i = 2000; i > 1000; i -= 50){
      tone(5, i);

      // alterna LED (pisca)
      estadoLED = !estadoLED;
      digitalWrite(6, estadoLED);

      delay(50);
    }
  }
  else
  {
    digitalWrite(6, LOW);
    noTone(5);
  }
}