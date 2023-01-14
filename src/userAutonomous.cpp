
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
    startPos = readFile("startpos"); // 0 = left; 1 = right
    int rollers = readFile("rollers");
    int launch = readFile("launch");
    //int pickup = readFile("pickup");
    int pickup = 0;

    // Config the personality array with the correct behaviors based on the cases

    if ( !skillsCheck ) {

      if ( rollers == 1 ) { personality[1] = 10; }

      if ( startPos == 0 ) {
       if ( launch == 1 ) { personality[2] = 1;}
      } else {
        if ( rollers == 1 ) { personality[0] = 2;  }
        if ( rollers == 1 && launch == 1) { personality[3] = 3; }
      }

    } else {
      personality[1] = 10;
      personality[2] = 12;
      personality[3] = 11;
    }

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

  // Debug Messages
  aiDebug("Running Task: ");
  Brain.Screen.print( taskNum );
  Brain.Screen.print( "  " );
  Brain.Screen.print( personality[taskNum] );

  // Run the specified behavior
  if (personality[taskNum] == 0) { return true; }; // If 0 is inputed, then it will skip
  if (personality[taskNum] == 10) { return changeRoller(); };
  if (personality[taskNum] == 11) { return expand();}
  if (personality[taskNum] == 12) {return replay(pathNames[0]); }
  if (personality[taskNum] >= 0 && personality[taskNum] < 10) { return replay(pathNames[personality[taskNum]]); }
  
  return false;
  
};




// Behaviors 

// Behaviors return either true or false
// True = The task was completed correctly
// False = The task was not able to be completed for some reason



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



// Code for replaying the autonomous recordings

bool ai::replay( const char* pathFile) {
  // open the file for reading
  std::ifstream input_file(pathFile);

  Brain.Screen.setPenColor(vex::color::white);
  
  int launchVel = 0;
  
  RightDriveSmart.resetPosition();
  LeftDriveSmart.resetPosition();

  //double lastLeftPos = LeftDriveSmart.position(vex::rotationUnits::rev);
  //double lastRightPos = RightDriveSmart.position(vex::rotationUnits::rev);

  double readDeltaTime = 0;
  double deltaTime;

  bool debug = true;

  while (true) {

    double startTime = Brain.timer(vex::timeUnits::msec);


    //if ( LeftDriveSmart.position(vex::rotationUnits::rev) != lastLeftPos ) { LeftDriveSmart.rotateTo(lastLeftPos, vex::rotationUnits::rev); }
    //if ( RightDriveSmart.position(vex::rotationUnits::rev) != lastRightPos ) { RightDriveSmart.rotateTo(lastRightPos, vex::rotationUnits::rev); }

    // read a line from the file
    std::string line;
    std::getline(input_file, line);

    // check if the end of the file has been reached
    if (input_file.eof()) {
      // reset the file stream to the beginning of the file
      input_file.clear();
      input_file.seekg(0, std::ios::beg);
      x = 0;
      y = 0;

      //wait(10, seconds);
      return true;
    }

  
    // parse the values from the line
    std::stringstream ss(line);

    unsigned int runlaunch, runlaunchfeed, runmainfeed = 0;
    double leftPos, rightPos = 0;

    std::string x_str;
    std::getline(ss, x_str, ',');
    std::istringstream xStream(x_str.c_str());
    xStream >> x;


    std::string y_str;
    std::getline(ss, y_str, ',');
    std::istringstream yStream(y_str.c_str());
    yStream >> y;


    std::string runlaunch_str;
    std::getline(ss, runlaunch_str, ',');
    std::istringstream runlaunchStream(runlaunch_str.c_str());
    runlaunchStream >> runlaunch;


    std::string runlaunchfeed_str;
    std::getline(ss, runlaunchfeed_str, ',');
    std::istringstream runlaunchfeedStream(runlaunchfeed_str.c_str());
    runlaunchfeedStream >> runlaunchfeed;


    std::string runmainfeed_str;
    std::getline(ss, runmainfeed_str, ',');
    std::istringstream runmainfeedStream(runmainfeed_str.c_str());
    runmainfeedStream >> runmainfeed;


    std::string leftPos_str;
    std::getline(ss, leftPos_str, ',');
    std::istringstream LeftPosStream(leftPos_str.c_str());
    LeftPosStream >> leftPos;


    std::string rightPos_str;
    std::getline(ss, rightPos_str, ',');
    std::istringstream rightPosStream(rightPos_str.c_str());
    rightPosStream >> rightPos;

    

    std::string readDeltaTime_str;
    std::getline(ss, readDeltaTime_str, ',');
    std::istringstream readDeltaTimeStream(readDeltaTime_str.c_str());
    readDeltaTimeStream >> readDeltaTime;




    if (runlaunch == 1) {
      launchVel = launchVel + 5.0;
    } else if (runlaunch == 0 ) {
      launchVel = launchVel + -5.0;
    }
    if (launchVel > 100) {
      launchVel = 100;
    }
    if (launchVel < 0.0) {
      launchVel = 0.0;
    }

    LauncherGroup.setVelocity(launchVel, percent);
    LauncherFeeder.setVelocity(runlaunchfeed * 100, percent);
    PickerUper.setVelocity(runmainfeed, percent);

    if (debug) {

      Brain.Screen.clearScreen();
      Brain.Screen.setCursor(1, 5);
      Brain.Screen.print("Left: ");
      Brain.Screen.print(x);
      Brain.Screen.setCursor(1, 17);
      Brain.Screen.print("Right: ");
      Brain.Screen.print(y);  
      Brain.Screen.setCursor(2, 5);
      Brain.Screen.print("RunLaunch: ");
      Brain.Screen.print(runlaunch); 
      Brain.Screen.setCursor(3, 5);
      Brain.Screen.print("RunLaunchFeed: ");
      Brain.Screen.print(runlaunchfeed); 
      Brain.Screen.setCursor(4, 5);
      Brain.Screen.print("RunMainFeed: ");
      Brain.Screen.print(runmainfeed);

      Brain.Screen.setCursor(5, 5);
      Brain.Screen.print("Delata: ");
      Brain.Screen.print(readDeltaTime);
      Brain.Screen.print("   ");
      Brain.Screen.print(deltaTime);
      Brain.Screen.print("   ");
      Brain.Screen.print(readDeltaTime - deltaTime);

      Brain.Screen.setCursor(6, 5);
      Brain.Screen.print("Lancher Vel: ");
      //Brain.Screen.print(LauncherVel);    
 
      Brain.Screen.setCursor(8, 5);
      Brain.Screen.print("ReadLeftPos: ");
      Brain.Screen.print(leftPos);    

      Brain.Screen.setCursor(9, 5);
      Brain.Screen.print("ActualLeftPos: ");
      Brain.Screen.print(LeftDriveSmart.position(vex::rotationUnits::rev));    

    }

    double endTime = Brain.timer(vex::timeUnits::msec);
    deltaTime = endTime - startTime;

    Brain.Screen.setCursor(7, 5);
    Brain.Screen.print(readDeltaTime - deltaTime);

    vex::task::sleep(fabs(readDeltaTime - deltaTime));

    //lastLeftPos = LeftDriveSmart.position(vex::rotationUnits::rev);
    //lastRightPos = RightDriveSmart.position(vex::rotationUnits::rev);
  }

};