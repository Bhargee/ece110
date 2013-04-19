#define WHITE 0 //find value  
#define GREY 0 //find value
#define BLACK 0//find value

//Actuation Control Ports
//output
int l_motor_power = 13;
int l_motor_dir = 12;
int r_motor_power = 11;
int r_motor_dir = 9;
//input
int l_tape_follow = A0;
int r_tape_follow = A1;
int l_grey = A2;
int r_grey = A3;
int l_strip = A4;
int r_strip = A5;
//more state control constants
//taken from Danny's code
enum robot_op {normal, cw, ccw};
robot_op state = normal;
bool val_test = false;
bool l_strip_d = false;
bool r_strip_d = false;

void setup() {
  pinMode(l_motor_power, OUTPUT);
  pinMode(l_motor_dir, OUTPUT);
  pinMode(r_motor_power, OUTPUT);
  pinMode(r_motor_dir, OUTPUT);
  
  pinMode(l_tape_follow, INPUT);
  pinMode(r_tape_follow, INPUT);
  pinMode(l_grey, INPUT);
  pinMode(r_grey, INPUT);
  pinMode(l_strip, INPUT);
  pinMode(r_strip, INPUT);
  //init serial comm
  Serial.begin(9600);
}

void loop() {
  if(val_test)
    Serial.println(analogRead(A0));
    
  //add stop condition
  if(stop())
  {
    drive(0,0);
    return;  
  }
  //returns to normal after turning
  if(is_white(l_tape_follow) && is_white(r_tape_follow) && is_black(l_grey)&& is_black(r_grey) && state != normal)
  {
    drive(0,0);
    state = normal;
    Serial.println("returned to normal state after turning");
  }
  
  //watches for directional strips
  if(is_white(l_strip))
    l_strip_d = true;
  if(is_white(r_strip))
    r_strip = true;
  
  //watches for intersection
  if(state == normal && is_grey(l_tape_follow) && is_grey(r_tape_follow))
  {
    //if both sensors read grey, check the value of the strip variables
    if(is_grey(l_grey) && is_grey(r_grey))
    {
      if(l_strip_d && r_strip_d)
      {
        l_strip_d = false;
        r_strip_d = false;
      }
      else if(l_strip_d)
      {
        state = ccw;
        l_strip_d = false;
      }
      else if(r_strip_d)
      {
        state = cw;
        r_strip_d = false;
      }
   }
   
    else if(is_grey(l_grey))
    {
      state = ccw;
    }
    else if(is_grey(r_grey))
    {
      state = cw;
    }
  } 
  
  switch(state)
  {
    case normal:
      drive(analogRead(r_tape_follow)/4, analogRead(l_tape_follow)/4);
      break;
    case cw:
      drive(255, -230);
      break;
    case ccw:
      drive(-230, 255);
      break;
  }
        
}

void drive(int l_power, int r_power)
{
  if(l_power >= 0)
    digitalWrite(l_motor_dir, HIGH);
  else
    digitalWrite(l_motor_dir, LOW);
    
  if(r_power >= 0)
    digitalWrite(r_motor_dir, HIGH);
  else
    digitalWrite(r_motor_dir, LOW);
    
    analogWrite(l_motor_power, abs(l_power));
    analogWrite(r_motor_power, abs(r_power));
}

bool stop()
{
  return (is_white(r_strip) && is_white(r_grey) && is_white(r_tape_follow));
}

bool is_grey(int sensor_port)
{
  int sensor_val = analogRead(sensor_port);
  return (sensor_val >= GREY && sensor_val < WHITE);
}

bool is_white(int sensor_port)
{
  int sensor_val = analogRead(sensor_port);
  return sensor_val >= WHITE;
}

bool is_black(int sensor_port)
{
  int sensor_val = analogRead(sensor_port);
  return (sensor_val >= BLACK && sensor_val < GREY);
}
