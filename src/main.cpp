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
// PickerUper           motor         6               
// LauncherFeeder       motor         13              
// DiskLimitSwitch      limit         A               
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"


#include "userAutonomous.h"
#include "userFunctions.h"


using namespace vex;


float visionTurnPercent = 0;
float visionTurnDirection = 0;


ai botAi("blue");


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
  


  // All activities that occur before the competition starts
  // Example: clearing encoders, setting servo positions, ...
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
  
  //botAi.collectDisk();

  bool runBotAuton = true;
  bool iterateCycle;

  while(runBotAuton) {
  
     iterateCycle = botAi.iterate();

    if ( not iterateCycle ) {
      // Run error
      runBotAuton = false;
    };

    if (not botAi.running) {
      // Bot is done running
      runBotAuton = false;
    }
    
    wait(0.5, seconds);

  };

  if ( iterateCycle && not botAi.running) {
    Brain.Screen.newLine();
    Brain.Screen.print("Finished autonomous without errors");
  };

  if ( not iterateCycle && botAi.running) {
    Brain.Screen.newLine();
    Brain.Screen.print(" There was an error with the autonomous ");
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
  // Start Launcher rev code

  StartLauncherControl();
  
}





// Once the match is started
int whenStarted() {

  LauncherGroup.spin(forward);
  LauncherGroup.setVelocity(0.0, percent);
  LauncherFeeder.spin(forward);
  LauncherFeeder.setVelocity(0.0, percent);
  PickerUper.spin(forward);
  PickerUper.setVelocity(0.0, percent);
  Controller1.Screen.print("Homie Natasha");
  Brain.Screen.print("Homie");
  while (true) {
    wait(1.0, seconds);
    Controller1.Screen.setCursor(1, 0);
    Controller1.Screen.clearScreen();
    Controller1.Screen.print("Batt Left");
    Controller1.Screen.setCursor(1, 12);
    Controller1.Screen.print(Brain.Battery.capacity() - 20);
    //Controller1.Screen.newLine();
    //Controller1.Screen.print(LauncherGroup.temperature(vex::temperatureUnits::celsius));
    //Controller1.Screen.newLine();
    //Controller1.Screen.print(LauncherGroup.velocity(vex::rpm));  
  
  }


  return 0;
}



void diskPressed() {
  botAi.diskEntered();
};

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



  // Autonomous Callbacks

  DiskLimitSwitch.pressed(diskPressed);


  whenStarted();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}




