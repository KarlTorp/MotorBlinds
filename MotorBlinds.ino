
#include <IRremote.h>

#define RECV_PIN 2
#define BTN_UP 3
#define BTN_DOWN 4

#define LED_RED A5
#define LED_GREEN A6
#define LED_BLUE A7

#define LED 13

#define EEPROM_ADDR_1 0
#define EEPROM_ADDR_2 4

#define BTN_PRESS HIGH

#define LONG_PRESS 5000
#define INIT_POSITION 0xFFFFFFFF

unsigned long sig_up, sig_down, sig_up2, sig_down2;
unsigned long button_up_pressed_start = 0;
unsigned long button_down_pressed_start = 0;

IRrecv irrecv(RECV_PIN);
decode_results results;

void Redefine_channel_signal(char channel)
{
  while (digitalRead(BTN_UP) == BTN_PRESS || digitalRead(BTN_DOWN) == BTN_PRESS)
  {
    digitalWrite(LED, HIGH);
    delay(200);
  }
  int receive = true;
  Serial.println("Redefine_channel_signal");
  delay(500);
  while (receive)
  {
    if (irrecv.decode(&results))
    {
      if (results.value != 0xFFFFFFFF)
      {
        switch (channel) {
          case 1:
            sig_up = results.value;
            write_long_EEPROM(EEPROM_ADDR_1, sig_up);
            break;
          case 2:
            sig_down = results.value;
            write_long_EEPROM(EEPROM_ADDR_2, sig_down);
            break;
        }
        Serial.println("value changed to:");
        Serial.println(results.value, HEX);
        receive = false;
        irrecv.resume();
      }
    }
    if(digitalRead(BTN_UP) == BTN_PRESS || digitalRead(BTN_DOWN) == BTN_PRESS)
    {
      Serial.println("Abort by button press");
      receive = false;
    }
  }
  digitalWrite(LED, LOW);

}

void up_command()
{
  set_step_delay(300);
  set_target_position(0);
}

void down_command()
{
  set_step_delay(200);
  set_target_position(12900);
}

void command_init()
{
  set_step_delay(300);
  set_target_position(INIT_POSITION);
}


void check_buttons()
{
  if (digitalRead(BTN_UP) == BTN_PRESS)
  {
    if(button_up_pressed_start == 0) {
      button_up_pressed_start = millis();
    } else if(millis() > button_up_pressed_start + LONG_PRESS)  {
      button_up_pressed_start = 0;
      Redefine_channel_signal(1);
    }
  } else {
    if(button_up_pressed_start != 0) {
      button_up_pressed_start = 0;
      Serial.println("btn up");
      up_command();
    }
  }

  if (digitalRead(BTN_DOWN) == BTN_PRESS)
  {
    if(button_down_pressed_start == 0) {
      button_down_pressed_start = millis();
    } else if(millis() > button_down_pressed_start + LONG_PRESS)  {
      button_down_pressed_start = 0;
      Redefine_channel_signal(2);
    } else if(millis() > button_down_pressed_start + 2000 && digitalRead(BTN_UP) == BTN_PRESS)  {
      button_down_pressed_start = 0;
      button_up_pressed_start = 0;
      while (digitalRead(BTN_UP) == BTN_PRESS || digitalRead(BTN_DOWN) == BTN_PRESS)
      {
        digitalWrite(LED, HIGH);
        delay(100);
      }
      delay(1000);
      Serial.println("Init up");
      command_init();
      digitalWrite(LED, LOW);
    }
  } else {
    if(button_down_pressed_start != 0) {
      button_down_pressed_start = 0;
      Serial.println("btn down");
      down_command();
    }
  }
}

void check_IR()
{
  if (irrecv.decode(&results)) {
    //Serial.println(results.value, HEX);
    if (results.value == sig_up || results.value == sig_up2)
    {
      Serial.println("IR up");
      up_command();
    }
    if ( results.value == sig_down || results.value == sig_down2)
    {
      Serial.println("IR down");
      down_command();
    }
    irrecv.resume(); // Receive the next value

  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Boot");
  irrecv.enableIRIn(); // Start the receiver
  pinMode(LED, OUTPUT);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);

  //pinMode(LED_RED, OUTPUT);
  //pinMode(LED_GREEN, OUTPUT);
  //pinMode(LED_BLUE, OUTPUT); 
  //digitalWrite(LED_RED, LOW);
  //digitalWrite(LED_GREEN, LOW);
  //digitalWrite(LED_BLUE, LOW);

  init_easyDriver();

  //write_long_EEPROM(EEPROM_ADDR_1,0xFF18E7);
  //write_long_EEPROM(EEPROM_ADDR_2,0xFF4AB5);
  sig_up = read_long_EEPROM(EEPROM_ADDR_1);
  sig_down = read_long_EEPROM(EEPROM_ADDR_2);
  sig_up2 = 0xFF18E7;
  sig_down2 = 0xFF4AB5;
}

void loop() {
  check_buttons();
  check_IR();
  regulate_position();
}
