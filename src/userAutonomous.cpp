#include "vex.h"
#include "userAutonomous.h"

#include "string"

#include "visionConfig.h"

using namespace vex;



// Constructor method 
ai::ai( int fallbackColor ) {}; 
// ^ I had problems with this not running when the object was being constructed
// so I moved all of its code to the bellow function

// Inits sensors and reads all config files
void ai::init() {

  Vision.setLedBrightness(0);
  Vision.setSignature(V_DISK);

  Optical.setLight(vex::ledState::on);
  Optical.setLightPower(100);
  Optical.gestureDisable();

  
  if (Brain.SDcard.isInserted()) {
    
    // read sd card and load info

    // all config files are written to the sd card through the autonSelector program

    aiDebug("Loading Auton Config: ");

    // all configs are stored as int values
    
    // reads all the files
    teamColor = readFile("team");
    int rollers = readFile("rollers");
    int launch = readFile("launch");
    int pickup = readFile("pickup");


    // Load configs into the personality array

    if (rollers == 1) { personality[0] = 1; }
    if (pickup == 1) { personality[1] = 2; }
    if (launch == 1) { personality[2] = 3; }



    // Code for displaying the current config on the brain screen
    
    Brain.Screen.newLine();

    if ( teamColor == 1 ) {
      Brain.Screen.print("Team: Blue");
    } else {
      Brain.Screen.print("Team: Red");
    }

    Brain.Screen.newLine();

    if ( rollers == 1 ) {
      Brain.Screen.print("Rollers: True");
    } else {
      Brain.Screen.print("Rollers: False");
    }

    Brain.Screen.newLine();

    if ( launch == 1 ) {
      Brain.Screen.print("Launch: True");
    } else {
      Brain.Screen.print("Launch: False");
    }

    Brain.Screen.newLine();

    if ( pickup == 1 ) {
      Brain.Screen.print("Pickup: True");
    } else {
      Brain.Screen.print("Pickup: False");
    }

  } else {
    
    // if the sd card is not inserted, the robot will do nothing during the autonomous section
    // The personality array is set to { 0,0,0,0....} by default

    teamColor = 0;
    aiError("Could not read SDCard... Running No Autonomous");
  }

};


// Functions for reading and writing files to the sd card
// All values are stored as int because it seemed the most simple to do
unsigned int ai::readFile(const char* fileName) {
  unsigned int ival = 0;
  unsigned char readBuff[4];
  Brain.SDcard.loadfile(fileName, readBuff, 4);
	ival = *(unsigned int*)(readBuff);
  return ival;
};

void ai::writeFile(const char* fileName, unsigned int numToWrite) {
  unsigned int ival = numToWrite;
  unsigned char writeBuff[4];
  memcpy(writeBuff, (char*)&ival,sizeof(unsigned int));
  Brain.SDcard.savefile(fileName, writeBuff, 4);
};



// Displays error on brain screen
void ai::aiError(const char* message) {
  
  if (Brain.Screen.row() == 12) { Brain.Screen.clearScreen(); Brain.Screen.setCursor(0, 1);}

  Brain.Screen.newLine();
  Brain.Screen.setPenColor(vex::color::red);
  Brain.Screen.print(message);
  Brain.Screen.setPenColor(vex::color::white);
  
}

// Displays debug message on brain screen
void ai::aiDebug(const char* message) {
  
  if (Brain.Screen.row() == 12) { Brain.Screen.clearScreen(); Brain.Screen.setCursor(0, 1);}

  Brain.Screen.newLine();
  Brain.Screen.setPenColor(vex::color::purple);
  Brain.Screen.print(message);
  Brain.Screen.setPenColor(vex::color::white);

}


// Used to confirming a disk entered the intake
// I have yet to implement this yet
void ai::diskEntered() {
  aiDebug("Limit switch was pressed");

};




// Main internal methods for iterating through the behaviors

// Called every time the robot wants to advance to the next behavior defined in the personality
bool ai::iterate() {

  //aiDebug("Iterating");

  currentTask = currentTask + 1; 

  if (currentTask >= personalitySize) {

    running = false; 
    return true;     // Done running through tasks

  } else {
    return runTask(currentTask); // Runs the next task - It will return false if the task fails
  };

};

// Runs the behavior at the coresponding personality index
bool ai::runTask( int taskNum ) {

  // Debug Messages
  aiDebug("Running Task: ");
  Brain.Screen.print( taskNum );
  Brain.Screen.print( "  " );
  Brain.Screen.print( personality[taskNum] );

  // Run the specified behavior
  if (personality[taskNum] == 0) { return true; }; // If 0 is inputed, then it will skip
  if (personality[taskNum] == 1) { return changeRoller(); };
  if (personality[taskNum] == 2) { return collectDisk(); };
  if (personality[taskNum] == 3) { return stationaryLaunch(); }

  return false; // Gives an error if it can not find the specified behavior
};




// Behaviors 

// Behaviors return either true or false
// True = The task was completed correctly
// False = The task was not able to be completed for some reason


// Uses the color sensor to determine when to stop spinning the roller
bool ai::changeRoller() {

  Brain.Screen.newLine();
  Brain.Screen.print("Starting Roller Code");

  PickerUper.setVelocity(-100, percent);
  //Drivetrain.driveFor(vex::directionType::fwd, 1, vex::distanceUnits::in);
  Drivetrain.setDriveVelocity(10, percent);
  Drivetrain.drive(vex::directionType::fwd);
  
  bool sawColor = false;
  bool sawOppositeColor = false;


  int colorSeen = 0;
  bool changeRollers = true;
  
  while (changeRollers) {

    if (Optical.hue() > 80) { colorSeen = 1;} else { colorSeen = -1;};

    if (colorSeen != teamColor && Optical.isNearObject()) { sawOppositeColor = true; }
    if (colorSeen == teamColor && Optical.isNearObject()) { sawColor = true;}

    if (colorSeen == teamColor && Optical.isNearObject() && sawColor && sawOppositeColor) {
      PickerUper.setVelocity(0, percent);
      changeRollers = false;
    }

    //Drivetrain.driveFor(vex::directionType::fwd, 2, vex::distanceUnits::cm);
    // PickerUper.rotateFor(-10, vex::rotationUnits::rev);

    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(2, 2);
    Brain.Screen.print(Optical.hue());
    Brain.Screen.setCursor(5, 2);
    Brain.Screen.print(sawColor);
    Brain.Screen.setCursor(8, 2);
    Brain.Screen.print(sawOppositeColor);    
    wait(0.3, seconds);
  }

  Drivetrain.stop();

  
  return true;
};


bool ai::stationaryLaunch() {

  Brain.Screen.newLine();
  Brain.Screen.print("Starting Launch Code");

  LauncherGroup.setVelocity(100, percent);
  wait(0.5, seconds);
  LauncherFeeder.setVelocity(100, percent);

  wait(5, seconds);
  LauncherGroup.setVelocity(0, percent);
  LauncherFeeder.setVelocity(0, percent);
  
  return true;
};



// Uses the vision sensor on the front of the robot to pickup a disk
// Works like 60% of the time
// If there is a clump of disks together, then it can not differentiate them
bool ai::collectDisk() {

  Brain.Screen.newLine();
  Brain.Screen.print("Starting Collect Disk");
  
  return true;
  
  int screen_middle_x = 316 / 2;
  bool collected = false;
  bool aligned = false;



  while (not collected) {
    Vision.takeSnapshot(V_DISK);
    if (Vision.objectCount > 0) {

      Brain.Screen.newLine();
      Brain.Screen.print(Vision.largestObject.centerY);

      Drivetrain.setDriveVelocity(10, percent);
      Drivetrain.setTurnVelocity(10, percent);
      Drivetrain.drive(forward);

      if (Vision.largestObject.centerX < screen_middle_x - 5) {
        // on the left
        Drivetrain.turn(turnType::left);
        aligned = false;
      } else if (Vision.largestObject.centerX > screen_middle_x + 5) {
        // on the right
        Drivetrain.turn(turnType::right);
        aligned = false;
      } else {
        // is in the center
        
        aligned = true;
        Drivetrain.stop(coast);
        Drivetrain.driveFor(forward, 4, inches);

      }
    } else {

      if (aligned) {
        PickerUper.setVelocity(100, percent);
        Drivetrain.setDriveVelocity(50, percent);
        Drivetrain.driveFor(forward, 10, inches);

        Drivetrain.setDriveVelocity(100, percent);
        Drivetrain.driveFor(forward, 10, inches);

        LauncherGroup.setVelocity(100, percent);
        LauncherFeeder.setVelocity(100, percent);

        wait(20, seconds);

        LauncherFeeder.setVelocity(0, percent);
        LauncherGroup.setVelocity(0, percent);
        PickerUper.setVelocity(0, percent);
        
        aligned = false;

      } else {
        // saw nothing
        Drivetrain.stop();
      }
      
    }
  }
  

  // check if disk was collected

  return true;
};