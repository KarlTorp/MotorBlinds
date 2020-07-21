#define ENABLE 12
#define MS1 11
#define MS2 10
#define MS3 9
#define RESET 8
#define SLEEP 7
#define STEP 6
#define DIR 5
/*
MS1 MS2 MS3 Microstep Resolution
Low  Low Low Full step
High  Low Low Half step
Low High  Low Quarter step
High  High  Low Eighth step
High  High  High  Sixteenth step
*/

typedef enum {
  FULL_STEP,
  HALF_STEP,
  QUARTER_STEP,
  EIGHT_STEP,
  SIXTEEN_STEP
  
} A4988_STEP_DEVIDE;



#define DELAY_MOTOR 300 // 75 ok
#define INIT_POSITION 0xFFFFFFFF

uint32_t target_position = 0;
uint32_t current_position = 0;
uint32_t current_delay = DELAY_MOTOR;
uint8_t steps_pr_cycle = 0;
bool moving = false;

void set_step_size(int steps)
{
  switch(steps) {
    case SIXTEEN_STEP:
      digitalWrite(MS1, HIGH);
      digitalWrite(MS2, HIGH);
      digitalWrite(MS3, HIGH);
      steps_pr_cycle = 64;
      break;
    case EIGHT_STEP:
      digitalWrite(MS1, HIGH);
      digitalWrite(MS2, HIGH);
      digitalWrite(MS3, LOW);
      steps_pr_cycle = 32;
      break;
    case QUARTER_STEP:
      digitalWrite(MS1, LOW);
      digitalWrite(MS2, HIGH);
      digitalWrite(MS3, LOW);
      steps_pr_cycle = 16;
      break;
    case HALF_STEP:
      digitalWrite(MS1, HIGH);
      digitalWrite(MS2, LOW);
      digitalWrite(MS3, LOW);
      steps_pr_cycle = 8;
      break;
    case FULL_STEP:
    default:
      digitalWrite(MS1, LOW);
      digitalWrite(MS2, LOW);
      digitalWrite(MS3, LOW);
      steps_pr_cycle = 4;
      break;
  }
}

void init_easyDriver()
{
  pinMode(ENABLE, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);
  pinMode(RESET, OUTPUT);
  pinMode(ENABLE, OUTPUT);
  pinMode(SLEEP, OUTPUT);
  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);

  digitalWrite(ENABLE, HIGH); // Disable 
  digitalWrite(RESET, HIGH);
  digitalWrite(SLEEP, LOW);
  digitalWrite(STEP, LOW);
  set_step_size(HALF_STEP);
}


void set_target_position(uint32_t new_position)
{
  target_position = new_position; 
}

void set_step_delay(uint32_t step_delay)
{
  current_delay = step_delay; 
}

void stepDelay()
{
  delayMicroseconds(current_delay);
}

void make_one_cycle()
{
  uint8_t i = 0;
  moving = true;
  for(i = 0; i < steps_pr_cycle; ++i) {
    digitalWrite(STEP, HIGH);
    stepDelay();
    digitalWrite(STEP, LOW);
    stepDelay();
  }
}

void movement_start_check()
{
  if(!moving){
    digitalWrite(SLEEP, HIGH);
    digitalWrite(ENABLE, LOW);
    delay(10);
  }
}

void turn_ccw() 
{
  movement_start_check();
  digitalWrite(DIR, HIGH);
  make_one_cycle();
}

void turn_cw() 
{
  movement_start_check();
  digitalWrite(DIR, LOW);
  make_one_cycle();
}

void regulate_position()
{
  if(target_position == current_position) {
    if(moving)  {
      delay(200);
      digitalWrite(ENABLE, HIGH);
      digitalWrite(SLEEP, LOW);
      moving =  false;
    }
  }else if(target_position == INIT_POSITION) {
    turn_cw();
    current_position = 0;
  }else if(target_position > current_position) {
    turn_ccw();
    current_position++;
  } else if(target_position < current_position) {
    turn_cw();
    current_position--;
  }
}
