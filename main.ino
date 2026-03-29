#include <AlfredoCRSF.h>
#include <HardwareSerial.h>
#define BAUD_RATE 420000
// Motor 1 (Left)
int RPWM1 = 5;
int LPWM1 = 18;
int R_EN1 = 16;
int L_EN1 = 17;
// Motor 2 (Right)
int RPWM2 = 22;
int LPWM2 = 23;
int R_EN2 = 19;
int L_EN2 = 21;
AlfredoCRSF crsf;
void setup()
{
  pinMode(RPWM1, OUTPUT);
  pinMode(LPWM1, OUTPUT);
  pinMode(R_EN1, OUTPUT);
  pinMode(L_EN1, OUTPUT);
  pinMode(RPWM2, OUTPUT);
  pinMode(LPWM2, OUTPUT);
  pinMode(R_EN2, OUTPUT);
  pinMode(L_EN2, OUTPUT);
  digitalWrite(R_EN1, HIGH);
  digitalWrite(L_EN1, HIGH);
  digitalWrite(R_EN2, HIGH);
  digitalWrite(L_EN2, HIGH);
  Serial.begin(BAUD_RATE);
  crsf.begin(Serial);
}
void loop()
{
  crsf.update();
  int armSwitch = crsf.getChannel(4);
  if (!crsf.isLinkUp() || armSwitch < 1500)
  {
    stopMotors();
    return;
  }
  // Read channels
  float throttle = (crsf.getChannel(2) - 1500) / 500.0;
  float steering = (crsf.getChannel(1) - 1500) / 500.0;
  int sensitivityRaw = crsf.getChannel(3);
  // Turn sensitivity (60% → 100%)
  float turnSensitivity = (float)map(sensitivityRaw, 1000, 2000, 60, 100) / 100.0;
  // Apply sensitivity
  steering = steering * turnSensitivity;
  // Curvature drive scaling
  float speedScale = 1 - abs(throttle);
  steering = steering * (speedScale + 0.5);
  // Differential drive mixing
  float leftSpeed = throttle + steering;
  float rightSpeed = throttle - steering;
  // Limit speeds
  leftSpeed = constrain(leftSpeed, -1, 1);
  rightSpeed = constrain(rightSpeed, -1, 1);
  // Send to motors (swapped to fix left/right inversion)
  setLeftMotor(rightSpeed);
  setRightMotor(leftSpeed);
}
void setLeftMotor(float speedVal)
{
  int pwm = speedVal * 255;
  if (pwm > 0)
  {
    analogWrite(RPWM1, pwm);
    analogWrite(LPWM1, 0);
  }
  else
  {
    analogWrite(RPWM1, 0);
    analogWrite(LPWM1, abs(pwm));
  }
}
void setRightMotor(float speedVal)
{
  int pwm = speedVal * 255;
  if (pwm > 0)
  {
    analogWrite(RPWM2, pwm);
    analogWrite(LPWM2, 0);
  }
  else
  {
    analogWrite(RPWM2, 0);
    analogWrite(LPWM2, abs(pwm));
  }
}
void stopMotors()
{
  analogWrite(RPWM1, 0);
  analogWrite(LPWM1, 0);
  analogWrite(RPWM2, 0);
  analogWrite(LPWM2, 0);
}
