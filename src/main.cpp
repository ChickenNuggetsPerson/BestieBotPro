/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       VEX                                                       */
/*    Created:      Thu Sep 26 2019                                           */
/*    Description:  Competition Template                                      */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Drivetrain           drivetrain    19, 20, 11, 12  
// Controller1          controller                    
// LauncherGroup        motor_group   14, 15          
// PickerUper           motor         16              
// LauncherFeeder       motor         13              
// DiskLimitSwitch      limit         B               
// fnewmatics           digital_out   A               
// LineLeft             line          G               
// LineMid              line          F               
// LineRight            line          H               
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

#include <fstream>

#include "userAutonomous.h"
#include "userFunctions.h"

using namespace vex;

ai botAi( 1 ); // Creates the autonomous ai object

// Phenumatics launch code
bool launchConfirm = false;

void pneumaticPressed( void ) {

  launchConfirm = true; // When launchConfirm = true, it will show the confirm menu on the controller 
  bool confirm = true;

  Controller1.rumble("..");
  
  while (confirm) { 
    
    // Runs a confirm menu on the controller screen so the extender does not acidentally goes off
    
    wait(0.1, seconds);

    if (Controller1.ButtonLeft.pressing()) {
      confirm = false;
      launchConfirm = false;
    }
    
    if (Controller1.ButtonRight.pressing()) {
      // Launch
      botAi.aiDebug("Launching");
      fnewmatics.set(true);
      fnewmaticsB.set(true);

      confirm = false;
      launchConfirm = false;

      wait(2, seconds);
      fnewmatics.set(false);
      fnewmaticsB.set(false);
    }

  }

}



void recalibrate() {
  if (gyroSensor.installed()) {
    Brain.Screen.newLine();
    Brain.Screen.print("Starting Calibration");
    Controller1.Screen.clearScreen();
    Controller1.Screen.setCursor(2, 1);
    Controller1.Screen.print("Calibrating...");
    Controller1.Screen.setCursor(3, 1);
    Controller1.Screen.print("Do Not Touch Robot");
    Controller1.rumble("..");
    gyroSensor.startCalibration();
    while (gyroSensor.isCalibrating()) { wait(100, msec);}
  } else {
    Brain.Screen.newLine();
    Brain.Screen.print("Inertial Sensor Not Installed");
  }
}



// A global instance of competition
competition Competition;

// define your global instances of motors and other devices here

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the V5 has been powered on and        */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/

void pre_auton(void) {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void autonomous(void) {
  // ..........................................................................
  // Insert autonomous user code here.
  // ..........................................................................

  Brain.Screen.newLine();
  Brain.Screen.print("Starting Autonomous Stuff");


  // By calling the botAi.iterate() function, we can run through the list of ai behaviors 

  bool runBotAuton = true;
  bool iterateCycle;
  replaying = true;

  while(runBotAuton) {
  
    iterateCycle = botAi.iterate(); // .iterate() returns false if the task failed

    if ( not iterateCycle ) {
      // Run error
      runBotAuton = false;
    };

    if (not botAi.running) {
      // Bot is done running
      runBotAuton = false;
    }
    
    wait(0.1, seconds);

  };

  replaying = false;
  
  // Interperet why the loop ended

  if ( iterateCycle && not botAi.running) {
    Brain.Screen.newLine();
    Brain.Screen.print("Finished autonomous without errors");
  };

  if ( not iterateCycle && botAi.running) {
    Brain.Screen.newLine();
    Brain.Screen.print("There was an error with the autonomous ");
  };
    
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void usercontrol(void) {

  // Reset everything for driver control

  strafeFBL = 0;
  strafeFBR = 0;
  strafeLRL = 0;
  strafeLRR = 0;

  replaying = false;


  LauncherFeeder.setVelocity(0, percent);
  PickerUper.setVelocity(0, percent);
  LauncherGroup.setVelocity(0, percent);

  // Start Launcher rev code
  StartLauncherControl();
  
}





// Once the match is started
int whenStarted() {

  Controller1.Screen.clearScreen();

  // Start the motors with 0% velocity
  LauncherGroup.spin(forward);
  LauncherGroup.setVelocity(0.0, percent);
  LauncherFeeder.spin(forward);
  LauncherFeeder.setVelocity(0.0, percent);
  PickerUper.spin(forward);
  PickerUper.setVelocity(0.0, percent);


  fnewmatics.set(false);
  fnewmaticsB.set(false);
  
  // ^^ Inside joke in the team 

  //Brain.Screen.print(Competition.isCompetitionSwitch());

  botAi.init(); // Inits the ai object

  // Quality of life feature... 
  // Controller will notify the user if the battery is under 30%
  if (Brain.Battery.capacity() <=30) {
    
    Controller1.Screen.clearScreen();
    wait(0.5, seconds);
    Controller1.rumble("..--..");
    Controller1.Screen.setCursor(2, 1);
    Controller1.Screen.print("Low Battery: ");
    Controller1.Screen.print(Brain.Battery.capacity());
    wait(5, seconds);

  } else {
    Controller1.Screen.newLine();
    Controller1.Screen.print("Homie");
  }
  
  botAi.aiDebug("Homie");

  double driveNextWarnTime = Brain.timer(vex::timeUnits::msec);
  double launchNextWarnTime = Brain.timer(vex::timeUnits::msec);

  // Loop that controlls the controller's display
  while (true) {
    
    wait(0.5, seconds);

    Controller1.Screen.setCursor(1, 0);
    Controller1.Screen.clearScreen();
      
    // If no menu is being displayed, show the main menu
    if (!launchConfirm) { 
      
      // Shows the battery left until 20%
      // In testing, the smart motor's max velocity decreases to 50% at 20% battery capacity

      Controller1.Screen.print("Juice Left: ");
      //                         ^ Inside joke
      //Controller1.Screen.print(Brain.Battery.capacity() - 20);
      Controller1.Screen.print(Drivetrain.isMoving());
      Controller1.Screen.newLine();
      Controller1.Screen.print("Drive Temp: ");
      Controller1.Screen.print(Drivetrain.temperature(percent));
      Controller1.Screen.newLine();
      Controller1.Screen.print("Lanch Temp: ");
      Controller1.Screen.print(LauncherGroup.temperature(vex::temperatureUnits::fahrenheit));

   
    }
    if (launchConfirm) {

      // Shows this menu when the user presses the up button

      Controller1.Screen.print("Confirm Launch");
      Controller1.Screen.newLine();
      Controller1.Screen.newLine();
      Controller1.Screen.print("< Cancel   Confirm >");

    }

    // Warn the driver when the Drivetrain temperature is above 65%
    // In testing the motors start to slow down when they get above 70%

    // The menu shows for 10 seconds and then will show again after a minute
    if (Drivetrain.temperature(percent) >= 60 && Brain.timer(vex::timeUnits::msec) > driveNextWarnTime) {
      Controller1.Screen.setCursor(1, 0);
      Controller1.Screen.clearScreen();   
      Controller1.Screen.print("WARNING");
      Controller1.Screen.setCursor(2, 0);
      Controller1.Screen.print("Drivetrain Temp High");
      Controller1.Screen.setCursor(3, 0);
      Controller1.Screen.print("Temp: ");
      Controller1.Screen.print(Drivetrain.temperature(percent));
      Controller1.Screen.print("%");   
      Controller1.rumble("....");
      driveNextWarnTime = Brain.timer(vex::timeUnits::msec) + 60000;  
      wait(5, seconds);
    }


    if (LauncherGroup.temperature(vex::temperatureUnits::celsius) >= 50 && Brain.timer(vex::timeUnits::msec) > launchNextWarnTime) {
      Controller1.Screen.setCursor(1, 0);
      Controller1.Screen.clearScreen();   
      Controller1.Screen.print("WARNING");
      Controller1.Screen.setCursor(2, 0);
      Controller1.Screen.print("Launcher Temp High");
      Controller1.Screen.setCursor(3, 0);
      Controller1.Screen.print("Temp: ");
      Controller1.Screen.print(LauncherGroup.temperature(vex::temperatureUnits::fahrenheit));
      Controller1.Screen.print("%");   
      Controller1.rumble("....");
      launchNextWarnTime = Brain.timer(vex::timeUnits::msec) + 60000;  
      wait(5, seconds);
    }
  }

  return 0;
}



//
// Main will set up the competition functions and callbacks.
//
int main() {
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);
  
  // Run the pre-autonomous function.
  pre_auton();

  // Set up User Callbacks

  Controller1.ButtonA.pressed(buttonAPressed);
  Controller1.ButtonY.pressed(buttonYPressed);
  Controller1.ButtonB.pressed(buttonBPressed);

  Controller1.ButtonL2.pressed(buttonL2Pressed);
  Controller1.ButtonL2.released(buttonL2Released);

  Controller1.ButtonR2.pressed(buttonR2Pressed);
  Controller1.ButtonR2.released(buttonR2Released);


  Controller1.ButtonR1.pressed(buttonR1Pressed);
  Controller1.ButtonL1.pressed(buttonL1Pressed);

  Controller1.ButtonR1.released(buttonR1Released);
  Controller1.ButtonL1.released(buttonL1Released);

  Controller1.ButtonUp.pressed(pneumaticPressed);

  // Calibrate Sensor and start main loop
  recalibrate();
  whenStarted();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}




