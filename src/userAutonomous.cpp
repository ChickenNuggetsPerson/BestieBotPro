
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


    aiDebug("Loading Driving Config");
    
    andrewDriving = readFile("andrewDriving");
    
    Brain.Screen.newLine();

    if ( andrewDriving == 1 ) {
      Brain.Screen.print("Loaded Andrew Driving Config");
    } else {
      Brain.Screen.print("Loading Alt Driving Config");      
    }

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
       if ( launch == 1 ) { personality[2] = 1;}
      } else {
        if ( rollers == 1 ) { personality[0] = 2;  }
        if ( rollers == 1 && launch == 1) { personality[3] = 3; }
      }

    } else {
      personality[1] = 13;
      personality[2] = 12;
      personality[3] = 11;

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

  // Debug Messages
  aiDebug("Running Task: ");
  Brain.Screen.print( taskNum );
  Brain.Screen.print( "  " );
  Brain.Screen.print( personality[taskNum] );

  // Run the specified behavior
  if (personality[taskNum] == 0) { return true; }; // If 0 is inputed, then it will skip
  if (personality[taskNum] == 10) { return changeRoller( false ); };
  if (personality[taskNum] == 11) { return expand();}
  if (personality[taskNum] == 12) {return replay(pathNames[0]); }
  if (personality[taskNum] == 13) { return changeRoller( true ); };
  if (personality[taskNum] >= 0 && personality[taskNum] < 10) { return replay(pathNames[personality[taskNum]]); }
  
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


  setVel(30);
  Drivetrain.drive(vex::directionType::fwd);

  if ( longer ) {
    wait(0.45 * 2, seconds);
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
  motorFL = vel;
  motorFR = vel;
  motorBL = vel;
  motorBR = vel;
  return true;

}


// Code for replaying the autonomous recordings
// After a lot of testing this is the final* version of the replay system
// The program reads the values from the file and then apply the velocities to the motors

//    After some testing, I found that the sd card takes ~10ms to read and write a line but at 
//  random the sd card takes +100ms to write a line for whatever reason. To account for this, the 
//  system records how long it took to write the line during the recording process and during the 
//  replay process it takes into account how long it took to read and write the line and
//  then waits the correct amount of time to keep the replay in sync with the recording.

bool ai::replay( const char* pathFile) {

  motorFL = 0;
  motorFR = 0;
  motorBL = 0;
  motorBR = 0;

  // open the file 
  std::ifstream input_file(pathFile);

  Brain.Screen.setPenColor(vex::color::white);
  
  int launchVel = 0;

  double readDeltaTime = 0;
  double deltaTime;

  bool debug = true;

  double avgDelta = 0;
  double avgReadDelta = 0;
  double avgWaitTime = 0;
  double highestDelta = 0;
  double highestReadDelta = 0;
  int totalLines = 0;

  while (true) {

    double startTime = Brain.timer(vex::timeUnits::msec);

    // read a line from the file
    std::string line;
    std::getline(input_file, line);

    // check if the end of the file has been reached
    if (input_file.eof()) {
      // reset the file stream to the beginning of the file
      input_file.clear();
      input_file.seekg(0, std::ios::beg);

      motorFL = 0;
      motorFR = 0;
      motorBL = 0;
      motorBR = 0;

      int i;
      for ( i = 0; i < 20; i ++ ) { cout << "" << endl; }

      cout << "Replaying Done: " << endl;
      cout << "" << endl;
      cout << "Average Writing Delta: " << ( avgReadDelta / totalLines ) << endl;
      cout << "Average Reading Delta: " << ( avgDelta / totalLines ) << endl;
      cout << "Average Waiting Delta: " << ( avgWaitTime / totalLines ) << endl;
      cout << "" << endl;
      cout << "Highest Writing Delta: " << highestReadDelta << endl;
      cout << "Highest Reading Delta: " << highestDelta << endl;
      cout << "" << endl;

      //wait(10, seconds);
      return true;
    }

    totalLines ++;
  
    // Read the values from the line
    std::stringstream ss(line);

    unsigned int runlaunch, runlaunchfeed, runmainfeed, fr, br, fl, bl = 0;

    std::string fr_str;
    std::getline(ss, fr_str, ',');
    std::istringstream frStream(fr_str.c_str());
    frStream >> fr;
    motorFR = fr;

    std::string fl_str;
    std::getline(ss, fl_str, ',');
    std::istringstream flStream(fl_str.c_str());
    flStream >> fl;
    motorFL = fl;

    std::string br_str;
    std::getline(ss, br_str, ',');
    std::istringstream brStream(br_str.c_str());
    brStream >> br;
    motorBR = br;

    std::string bl_str;
    std::getline(ss, bl_str, ',');
    std::istringstream blStream(bl_str.c_str());
    blStream >> bl;
    motorBL = bl;

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
    

    std::string readDeltaTime_str;
    std::getline(ss, readDeltaTime_str, ',');
    std::istringstream readDeltaTimeStream(readDeltaTime_str.c_str());
    readDeltaTimeStream >> readDeltaTime;


    // Calculate launcher velocities

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

      // Display the values that were read

      Brain.Screen.clearScreen();
      Brain.Screen.setCursor(1, 5);
      Brain.Screen.print("FL: ");
      Brain.Screen.print(motorFL);
      Brain.Screen.print("  FR: ");
      Brain.Screen.print(motorFR);
      Brain.Screen.print("  BL: ");
      Brain.Screen.print(motorBL);
      Brain.Screen.print("  BR: ");
      Brain.Screen.print(motorBR);  
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
 
    }

    // Calculate the time to wait based on the read delta time and the current delta time
    double endTime = Brain.timer(vex::timeUnits::msec);
    deltaTime = endTime - startTime;

    Brain.Screen.setCursor(7, 5);
    Brain.Screen.print(readDeltaTime - deltaTime);
    
    avgDelta = avgDelta + deltaTime;
    avgReadDelta = avgReadDelta + readDeltaTime;
    avgWaitTime = avgWaitTime + ( readDeltaTime - deltaTime ) ;

    if ( deltaTime > highestDelta ) { highestDelta = deltaTime; }
    if ( readDeltaTime > highestReadDelta ) { highestReadDelta = readDeltaTime; }

    vex::task::sleep(fabs(readDeltaTime - deltaTime));

  }
  return false;
};
