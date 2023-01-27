#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 Brain screen
brain  Brain;

// VEXcode device constructors
motor leftMotorA = motor(PORT19, ratio36_1, false);
motor leftMotorB = motor(PORT20, ratio36_1, false);
motor_group LeftDriveSmart = motor_group(leftMotorA, leftMotorB);
motor rightMotorA = motor(PORT11, ratio36_1, true);
motor rightMotorB = motor(PORT12, ratio36_1, true);
motor_group RightDriveSmart = motor_group(rightMotorA, rightMotorB);
drivetrain Drivetrain = drivetrain(LeftDriveSmart, RightDriveSmart, 319.19, 254, 254, mm, 1);
controller Controller1 = controller(primary);
motor LauncherGroupMotorA = motor(PORT14, ratio6_1, true);
motor LauncherGroupMotorB = motor(PORT15, ratio6_1, false);
motor_group LauncherGroup = motor_group(LauncherGroupMotorA, LauncherGroupMotorB);
motor PickerUper = motor(PORT16, ratio18_1, false);
motor LauncherFeeder = motor(PORT13, ratio36_1, false);
limit DiskLimitSwitch = limit(Brain.ThreeWirePort.B);
digital_out fnewmatics = digital_out(Brain.ThreeWirePort.G);
digital_out fnewmaticsB = digital_out(Brain.ThreeWirePort.H);




int motorFL = 0;
int motorFR = 0;
int motorBL = 0;
int motorBR = 0;

int strafeFBL = 0;
int strafeFBR = 0;
int strafeLRL = 0;
int strafeLRR = 0;



bool replaying = false;


// VEXcode generated functions
// define variable for remote controller enable/disable
bool RemoteControlCodeEnabled = true;
// define variables used for controlling motors based on controller inputs
bool DrivetrainLNeedsToBeStopped_Controller1 = true;
bool DrivetrainRNeedsToBeStopped_Controller1 = true;

// define a task that will handle monitoring inputs from Controller1
int rc_auto_loop_function_Controller1() {
  // process the controller input every 20 milliseconds
  // update the motors based on the input values





  leftMotorA.spin(fwd);
  leftMotorB.spin(fwd);
  rightMotorA.spin(fwd);
  rightMotorB.spin(fwd);

  leftMotorA.setVelocity(0, percent);
  leftMotorB.setVelocity(0, percent);
  rightMotorA.setVelocity(0, percent);
  rightMotorB.setVelocity(0, percent);

  int deadzone = 10;

  while(true) {
    if(RemoteControlCodeEnabled && !replaying) {
      // calculate the drivetrain motor velocities from the controller joystick axies
      // left = Axis3 + Axis1
      // right = Axis3 - Axis1

      strafeFBL = Controller1.Axis3.position();
      strafeFBR = Controller1.Axis2.position();

      strafeLRL = - Controller1.Axis4.position();
      strafeLRR = Controller1.Axis1.position();


      if (strafeFBL < deadzone && strafeFBL > -deadzone) { strafeFBL = 0; }
      if (strafeFBR < deadzone && strafeFBR > -deadzone) { strafeFBR = 0; }
      if (strafeLRL < deadzone && strafeLRL > -deadzone) { strafeLRL = 0; }
      if (strafeLRR < deadzone && strafeLRR > -deadzone) { strafeLRR = 0; }

    }


    
    motorFL = strafeFBL - strafeLRL;
    motorFR = strafeFBR + strafeLRL;

    motorBL = strafeFBL + strafeLRL;
    motorBR = strafeFBR - strafeLRL;
    

    int changePercent = 15;

    if (motorBL > changePercent || motorBL < - changePercent) {if (motorBL > 0) {motorBL = motorBL - changePercent;} else {motorBL = motorBL + changePercent;}}
    if (motorBR > changePercent || motorBR < - changePercent) {if (motorBR > 0) {motorBR = motorBR - changePercent;} else {motorBR = motorBR + changePercent;}}


    leftMotorA.setVelocity(motorFL, percent);
    leftMotorB.setVelocity(motorBL, percent);
    rightMotorA.setVelocity(motorFR, percent);
    rightMotorB.setVelocity(motorBR, percent);

    // wait before repeating the process
    wait(20, msec);
  }
  return 0;
}
/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void vexcodeInit( void ) {
  task rc_auto_loop_task_Controller1(rc_auto_loop_function_Controller1);
}