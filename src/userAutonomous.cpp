
#include "vex.h"
#include "userAutonomous.h"

#include "string"

#include "visionConfig.h"

using namespace vex;
using std::cout;
using std::endl;


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
    startPos = readFile("startpos"); // 0 = left; 1 = right
    int skills = readFile("skills");
    int rollers = readFile("rollers");
    int launch = readFile("launch");
    //int pickup = readFile("pickup");
    int pickup = 0;

    // Config the personality array with the correct behaviors based on the cases


    // Special Behaviors
    //  - Rollers    - 10
    //  - Expand     - 11
    //  - Run Path 0 - 12   This is so the program can distinct between path 0 and an empty personality array 


    if ( skills == 0 ) {

      if ( rollers == 1 ) { personality[1] = 10; }

      if ( startPos == 0 ) {
       if ( launch == 1 ) { personality[2] = 2;}
      } else {
        if ( rollers == 1 ) { personality[0] = 3;  }
        if ( rollers == 1 && launch == 1) { personality[3] = 4; }
      }

    } else {
      personality[2] = 1;
    }

    // Code for displaying the current config on the brain screen
    Brain.Screen.newLine();
    if (skills != 1) {
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
      aiError("Running Skills");
      Brain.Screen.newLine();
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


// Main internal methods for iterating through the behaviors

// Called every time the robot wants to advance to the next behavior defined in the personality
bool ai::iterate() {

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

  strafeFBL = 0;
  strafeFBR = 0;
  strafeLRL = 0;
  strafeLRR = 0;

  // Debug Messages
  aiDebug("Running Task: ");
  Brain.Screen.print( taskNum );
  Brain.Screen.print( "  " );
  Brain.Screen.print( personality[taskNum] );

  // Run the specified behavior
  if (personality[taskNum] == 0) { return true; }; // If 0 is inputed, then it will skip
  if (personality[taskNum] > 0 && personality[taskNum] < 10) { return runPath(personality[taskNum]); }
  if (personality[taskNum] == 10) { return changeRoller(false);}
  
  return false;
  
};




// Behaviors 

// Behaviors return either true or false
// True = The task was completed correctly
// False = The task was not able to be completed for some reason



// Code that changes the roller
bool ai::changeRoller( bool longer ) {

  Brain.Screen.newLine();
  Brain.Screen.print("Starting Roller Code");

  if ( longer ) {
    PickerUper.setVelocity(100, percent);
  } else {
    PickerUper.setVelocity(-100, percent);
  }

  setVel(0);
  setVel(30);
  

  if ( longer ) {
    wait(0.45 * 1.5, seconds);
  } else {
    wait(0.45, seconds);
  }
  

  setVel(0);
  PickerUper.setVelocity(0, percent);

  return true;
  
};


// Extend 
bool ai::expand() {

  aiDebug("Expanding");

  fnewmatics.set(true);
  fnewmaticsB.set(true);

  wait(2, seconds);
  fnewmatics.set(false);
  fnewmaticsB.set(false);
  
  return true;
}


bool ai::setVel(int vel) {

  strafeFBL = vel;
  strafeFBR = vel;

  return true;
}

bool ai::turnTo(int rot, bool relative) {

  if (!gyroSensor.installed()) {aiError("Inertial Sensor Not Installed"); Controller1.rumble("--"); return false;}
  
  // Todo: Tune these values
  double pidP = 1.0; // Tune First
  double pidI = 1.0; // Tune Last
  double pidD = 1.0; // Tune Second


  int error;
  int prevError = 0;
  int derivative;
  int totalError = 0;

  int desiredHeading;
  if (relative) {
    desiredHeading = rot + gyroSensor.heading(vex::rotationUnits::deg);
  } else { 
    desiredHeading = rot;
  }
  

  while (true) {

    int currentHeading = gyroSensor.heading(vex::rotationUnits::deg);

    error = currentHeading - desiredHeading;
    derivative = error - prevError;

    totalError += error;


    int motorPower = ( error * pidP + derivative * pidD + totalError * pidI);

    strafeFBL = 0;
    strafeFBR = 0;

    cout << "PID OUT: " << motorPower << endl;


    prevError = error;
    vex::task::sleep(20);
  }


  return true;
};

bool ai::driveDist(double dist, int speed) {

  double pi = 3.14;
  double radius = 2;
  double circumference = 2 * pi * radius;

  double distTraveled = 0;
  double error = 0;

  leftMotorA.resetRotation();
  rightMotorA.resetRotation();

  if (dist > 0) {
    strafeFBL = speed;
    strafeFBR = speed;
  } else {
    strafeFBL = -speed;
    strafeFBR = -speed;
  }

  while ( fabs(error) > 0.5 ) {
    double left = ( leftMotorA.rotation(vex::rotationUnits::rev) / 3 ) * 5;
    double right = ( rightMotorA.rotation(vex::rotationUnits::rev) / 3 ) * 5;
    double avgRot = ( left + right ) / 2;

    distTraveled += ( circumference * avgRot );
    if (dist > 0) {
      error = dist - distTraveled;
    } else {
      error = distTraveled - dist;
    }
    
    leftMotorA.resetRotation();
    rightMotorA.resetRotation();
    wait(100, msec);
  }  

  return true;
}

bool ai::runPath( int pathNum ) {

    //const char* pathNames[5] = {
    //  "Paths/Skills/main.txt",
    //  "Paths/Left/Launch.txt",
    //  "Paths/Right/GoToRoller.txt",
    //  "Paths/Right/RollerToMid.txt",
    //  "test.txt"
    //};

  if ( pathNum == 1 ) {
    //  "Paths/Skills/main.txt",
    
    changeRoller(false);
    driveDist(2, 10);
    turnTo(45);

  }
  if ( pathNum == 2 ) {
    //  "Paths/Left/Launch.txt",

  }
  if ( pathNum == 3 ) {
    //  "Paths/Right/GoToRoller.txt",

  }
  if ( pathNum == 4 ) {
    //  "Paths/Right/RollerToMid.txt",

  }
  if ( pathNum == 5 ) {
    //  "test.txt"

  }

  return true;
};