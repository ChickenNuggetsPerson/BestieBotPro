#include "vex.h"
#include "userFunctions.h"

using namespace vex;


// Variables
float LauncherVel;
int RunLauncher = 0;
int runLauncherFeeder = 0;
int runMainFeeder = 0;


// User Functions

// Rev up and down for the disk launcher
void StartLauncherControl() {
  //Brain.Screen.print("DEBUG: Starting Rev Code");
  
  double maxVel = 100;

  while (true) {
    wait(0.05, seconds);

    if (LauncherGroup.temperature(vex::temperatureUnits::celsius) > 45) {
      maxVel = 50;
    } else { maxVel = 100; };

    if (RunLauncher == 1) {
      LauncherVel = LauncherVel + 5.0;
    } else if (RunLauncher == 0 ) {
      LauncherVel = LauncherVel + -5.0;
    }
    if (LauncherVel > maxVel) {
      LauncherVel = maxVel;
    }
    if (LauncherVel < 0.0) {
      LauncherVel = 0.0;
    }
    
    if (!Controller1.ButtonDown.pressing()) {
      LauncherGroup.setVelocity(LauncherVel, percent);
      //LauncherGroupMotorA.setVelocity(LauncherVel, percent);
      //LauncherGroupMotorB.setVelocity(LauncherVel - 20, percent);
      
    } else {
      LauncherGroup.setVelocity(-50, percent);
    }


    if (runLauncherFeeder == 0) {
      LauncherFeeder.setVelocity(0, percent);
    }
    if (runLauncherFeeder == 1) {
      LauncherFeeder.setVelocity(100, percent);
    }
    if (runLauncherFeeder == -1) {
      LauncherFeeder.setVelocity(-100, percent);
    }

    if (runMainFeeder == 0) { PickerUper.setVelocity(0, percent); }
    if (runMainFeeder == 1) { PickerUper.setVelocity(100, percent); }
    if (runMainFeeder == -1) { PickerUper.setVelocity(-100, percent); }

  }
}

// Controller Button Functions
void buttonAPressed() {
  // Toggle Pickerupper
  if (PickerUper.velocity(percent) > 50.0) {
    runMainFeeder = 0;
  } else {
    runMainFeeder = 1;
  }
}

void buttonYPressed() {
  //Drivetrain.driveFor(vex::directionType::fwd, 3, vex::distanceUnits::in);
}

void buttonBPressed() {
  // Launcher Belt Control
  
  if (runLauncherFeeder == 0) {
    runLauncherFeeder = 1;
  } else {
    runLauncherFeeder = 0;
  }
}

void buttonL2Pressed() {
  runLauncherFeeder = -1;
}

void buttonL2Released() {
  runLauncherFeeder = 0;
}

void buttonR2Pressed() {
  RunLauncher = 1;
}

void buttonR2Released() {
  RunLauncher = 0;
  runLauncherFeeder = 0;
}

void buttonR1Pressed() {
  runMainFeeder = 1;
}

void buttonL1Pressed() {
  runMainFeeder = -1;
}

void buttonR1Released() {
  runMainFeeder = 0;
}

void buttonL1Released() {
  runMainFeeder = 0;
}