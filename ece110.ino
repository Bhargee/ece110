#define WHITE 1000 //find value  
#define GREY 700 //find value
#define BLACK 200 

//Actuation Control Ports
//output
int l_motor_power = 11;
int l_motor_dir = 9;
int r_motor_power = 13;
int r_motor_dir = 12;
//input
int l_tape_follow = A0;
int r_tape_follow = A1;
int l_grey = A2;
int r_grey = A3;
int l_strip = A4;
int r_strip = A5;
//more state control constants
enum robot_op {normal, cw, ccw};
robot_op state = normal;
bool val_test = true;
bool l_strip_d = false;
bool r_strip_d = false;

void setup() {
  Serial.println("starting setup");
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
  Serial.print("left tape following: ");
  Serial.println(analogRead(l_tape_follow));
  Serial.print("right tape following: ");
  Serial.println(analogRead(r_tape_follow));
  Serial.print("left grey: ");
  Serial.println(analogRead(l_grey));
  Serial.print("right grey: ");
  Serial.println(analogRead(r_grey));
  Serial.print("left strip");
  Serial.println(analogRead(l_strip));
  Serial.print("right strip");
  Serial.println(analogRead(r_strip)); 
      
  if(val_test)
    return;    
  
  //add stop condition
  if(stop())
  {
    drive(0,0);
    Serial.println("stopped");
    return;  
  }
  //test for if sensors see white
  if(state != normal && is_white(l_tape_follow) && is_white(r_tape_follow))
  {
     digitalWrite(3, HIGH); 
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
    while(1) {
      Serial.println("intersection detected");
      drive(0,0);
      delay(100);
    }
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
  int l_sensor_val;
  int r_sensor_val;  
  switch(state)
  {
    case normal:
      /*if(is_white(l_tape_follow) && is_white(r_tape_follow))
      {
        drive(255,255);
      }
      else if(is_white(l_tape_follow) && is_black(r_tape_follow))
      {
        drive(-60, 255); 
      } 
      else if(is_black(l_tape_follow) && is_white(r_tape_follow))
      {
        drive(255,-60);
      }
      else
        drive(0,0);*/
      l_sensor_val = analogRead(l_tape_follow);
      r_sensor_val = analogRead(r_tape_follow);
      drive(map(r_sensor_val, 0, 255, 0, 1023)/2,map(l_sensor_val, 0, 255, 0 ,1023)/2);
      Serial.println("in normal");
      break;
    case cw:
      drive(150, -130);
      /*
      drive(150, -150);
      delay(900);
      */
      Serial.println("in cw");
      break;
    case ccw:
      drive(-130, 150);
      /*
      drive(-150, 150);
      delay(900);
      */
      Serial.println("in ccw");
      break;
  }
  delay(20);
        
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
//tweak with numbers
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
  return (sensor_val <= BLACK);
}
