

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

  
  if (Brain.SDcard.isInserted()) {
    
    // read sd card and load info

    // all config files are written to the sd card through the autonSelector program

    aiDebug("Loading Auton Config: ");

    // all configs are stored as int values
    
    // reads all the files
    teamColor = readFile("team");
    startPos = readFile("startpos");
    int rollers = readFile("rollers");
    int launch = readFile("launch");
    int pickup = readFile("pickup");


    // Load configs into the personality array

    //if (startpos == 1) { personality[0] = 4; }

    if (startPos == 1) {
      if (rollers == 1) { 
        personality[1] = 4;
        personality[2] = 1; 
      }
    } else {
      if (rollers == 1) { personality[1] = 1; }
    }

    if (pickup == 1) { personality[3] = 2; }
    if (launch == 1) { personality[4] = 3; }

    


    // Code for displaying the current config on the brain screen
    
    Brain.Screen.newLine();

    if ( teamColor == 1 ) {
      Brain.Screen.print("Team: Blue");
    } else {
      Brain.Screen.print("Team: Red");
    }

    Brain.Screen.newLine();

    if ( startPos == 1 ) {
      Brain.Screen.print("Start: Right");
    } else {
      Brain.Screen.print("Start: Left");
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
  if (personality[taskNum] == 4) { return rightAdjust(); }

  return false; // Gives an error if it can not find the specified behavior
};




// Behaviors 

// Behaviors return either true or false
// True = The task was completed correctly
// False = The task was not able to be completed for some reason


// Adjust for the rollers on the right side
bool ai::rightAdjust() {

  Brain.Screen.newLine();
  Brain.Screen.print("Starting Right Adjust");


  Drivetrain.setDriveVelocity(50, percent);
  Drivetrain.setTurnVelocity(17, percent);
  
  Drivetrain.drive(vex::directionType::rev);
  
  wait(0.2, seconds);
  
  Drivetrain.stop(coast);
  
  Drivetrain.turn(vex::turnType::left);
  
  wait(0.25, seconds);

  Drivetrain.drive(vex::directionType::fwd);
  wait(1, seconds);
  Drivetrain.stop(coast);
  

  wait(1, seconds);

  Drivetrain.stop(coast);

  return true;
};

// Code that changes the roller
bool ai::changeRoller() {

  Brain.Screen.newLine();
  Brain.Screen.print("Starting Roller Code");

  PickerUper.setVelocity(-100, percent);
  
  Drivetrain.setDriveVelocity(30, percent);
  Drivetrain.drive(vex::directionType::fwd);

  wait(0.45, seconds);

  Drivetrain.stop();
  PickerUper.setVelocity(0, percent);

  return true;
  
};

bool ai::isWhite(float val) {
  if ( val > 50 ) {
    return true;
  } else {
    return false;
  }
}

bool ai::stationaryLaunch() {

  Brain.Screen.newLine();
  Brain.Screen.print("Starting Launch Code");

  if (startPos == 1) { return false; }


  // get in position

  Drivetrain.setTurnVelocity(50, percent);

  Drivetrain.driveFor(vex::directionType::rev, 1, vex::distanceUnits::in);
  Drivetrain.turn(vex::turnType::left);
  wait(0.60, seconds);
  Drivetrain.stop(coast);
  PickerUper.setVelocity(20, percent);
  Drivetrain.setDriveVelocity(70, percent);
  //wait(1, seconds);
  //Drivetrain.turnFor(-5, vex::rotationUnits::deg);
  Drivetrain.driveFor(vex::directionType::fwd, 34, vex::distanceUnits::in);
  PickerUper.setVelocity(0, percent);
  Drivetrain.turnFor(22, vex::rotationUnits::deg);


  


  float driveVel = 7;


  Drivetrain.setDriveVelocity(40, percent);
  Drivetrain.driveFor(vex::directionType::rev, 2, vex::distanceUnits::in);

  Drivetrain.setDriveVelocity(10, percent);
  Drivetrain.setTurnVelocity(9, percent);
  Drivetrain.drive(vex::directionType::rev);

  PickerUper.setVelocity(100, percent);

  bool notAligned = true;

  while (notAligned) {
    
    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(10, 5);
    Brain.Screen.print(isWhite(LineRight.reflectivity()));
    Brain.Screen.setCursor(12, 5);
    Brain.Screen.print(LineRight.reflectivity());
    
    //Brain.Screen.print(isWhite(LineMid.reflectivity()));
    Brain.Screen.setCursor(10, 15);
    Brain.Screen.print(isWhite(LineLeft.reflectivity()));
    Brain.Screen.setCursor(12, 15);
    Brain.Screen.print(LineLeft.reflectivity());
    

    if (isWhite(LineRight.reflectivity()) && isWhite(LineLeft.reflectivity())) {
      notAligned = false;
    }

    if (isWhite(LineRight.reflectivity()) && !isWhite(LineLeft.reflectivity())) {
      Drivetrain.setDriveVelocity(0, percent);
      Drivetrain.turn(turnType::right);
    }

    if (isWhite(LineLeft.reflectivity()) && !isWhite(LineRight.reflectivity())) {
      Drivetrain.setDriveVelocity(0, percent);
      Drivetrain.turn(turnType::left);
    }

    if (!isWhite(LineLeft.reflectivity()) && !isWhite(LineRight.reflectivity())) {
      Drivetrain.setDriveVelocity(driveVel, percent);
      Drivetrain.drive(vex::directionType::rev);
    }
  
    wait(0.1, seconds);
    
  }

  
  Drivetrain.setDriveVelocity(0, percent);
  PickerUper.setVelocity(0, percent);


  

  LauncherGroup.setVelocity(100, percent);
  wait(0.5, seconds);
  LauncherFeeder.setVelocity(100, percent);

  wait(7, seconds);
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

