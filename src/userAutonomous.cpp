
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
  cout << "ERROR: " << message << endl;
}

// Displays debug message on brain screen
void ai::aiDebug(const char* message) {
  
  if (Brain.Screen.row() == 12) { Brain.Screen.clearScreen(); Brain.Screen.setCursor(0, 1);}

  Brain.Screen.newLine();
  Brain.Screen.setPenColor(vex::color::purple);
  Brain.Screen.print(message);
  Brain.Screen.setPenColor(vex::color::white);
  cout << "DEBUG: " << message << endl;
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
  cout << "Changing Roller" << endl;

  if ( longer ) {
    PickerUper.setVelocity(100, percent);
  } else {
    PickerUper.setVelocity(-100, percent);
  }

  setVel(0);
  setVel(20);
  
  wait(0.45, seconds);
  
  setVel(0);

  if ( longer ) {
    wait(0.225, seconds);
  }

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


bool ai::setVel(double lVel, double rVel) {

  if ( rVel == 10000) {
    strafeFBL = lVel;
    strafeFBR = lVel;
  } else {
    strafeFBL = lVel;
    strafeFBR = rVel;
  }
  
  return true;
}

bool ai::turnTo(int rot, double timeOut) {

  // Trash code, just use the Drivetrain.turnToHeading()

  if (!gyroSensor.installed()) {aiError("Inertial Sensor Not Installed"); Controller1.rumble("--"); return false;}
  
  // Todo: Tune these values
  double pidP = 1; // Tune First
  double pidI = 0; // Tune Last
  double pidD = 0; // Tune Second

  int error;
  int prevError = 0;
  int derivative;
  int totalError = 0;

  gyroSensor.resetRotation();
  gyroSensor.resetHeading();

  int desiredHeading = rot;

  //desiredHeading = desiredHeading & 360;

  double endTime;
  if ( timeOut == 0) {
    endTime = 999999999999999999;
  } else {
    endTime = Brain.timer(vex::timeUnits::msec) + ( timeOut * 1000 );
  }

  int precision = 1;

  int motorPower = 0 + precision;

  while (true && Brain.timer(vex::timeUnits::msec) < endTime) {

    int currentHeading = gyroSensor.angle(vex::rotationUnits::deg);

    error = currentHeading - desiredHeading;
    derivative = error - prevError;

    totalError += error;

    motorPower = ( error * pidP + derivative * pidD + totalError * pidI);

    strafeFBL = -motorPower;
    strafeFBR = motorPower;
    
    
    cout << "PID OUT: " << motorPower << endl;
    //cout << "Heading: " << gyroSensor.heading(vex::rotationUnits::deg) << endl;
    cout << "Angle: " << gyroSensor.angle(vex::rotationUnits::deg) << endl;


    prevError = error;
    vex::task::sleep(20);
  }


  return true;
};

bool ai::driveDist(double dist, bool dynamicSpeed, int speed, double timeOut) {

  cout << "Starting Drive Dist" << endl;

  double startHeading = Drivetrain.heading(degrees);

  double wantedDistance = dist;

  if ( dist > 0 ) {
    wantedDistance = dist - 1;
  } else {
    wantedDistance = dist + 1;
  } 

  double pi = 3.14;
  double radius = 2;
  double circumference = 2 * pi * radius;



  double distTraveled = 0;

  leftMotorA.resetRotation();
  rightMotorA.resetRotation();
  leftMotorB.resetRotation();
  rightMotorB.resetRotation();

  double endTime;
  if ( timeOut == 0) {
    endTime = 999999999999999999;
  } else {
    endTime = Brain.timer(vex::timeUnits::msec) + ( timeOut * 1000 );
  }


  // Dynamic Speed Stuff

  double maxSpeed = 90;
  double minSpeed = 15;
  
  double accelerationDist = 8;
  double minSpeedDist = 0;

  double dynSpeed = speed;


  if ( dynamicSpeed ) {
  } else if (wantedDistance > 0) {
    setVel(speed);
  } else {
    setVel(-speed);
  }

  while ( fabs(wantedDistance) > distTraveled && Brain.timer(vex::timeUnits::msec) < endTime ) {
    // Todo: do gear math
    double Fleft = leftMotorA.rotation(vex::rotationUnits::rev);
    double Fright = rightMotorA.rotation(vex::rotationUnits::rev);

    double Bleft = leftMotorB.rotation(vex::rotationUnits::rev);
    double Bright = rightMotorB.rotation(vex::rotationUnits::rev);

    double avgRot = ( Fleft + Fright + Bleft + Bright) / 4;

    distTraveled += fabs( circumference * avgRot );

    


    if ( dynamicSpeed ) {
      double remainingDist = fabs(distTraveled - fabs(wantedDistance));

      double percent = 0;

      if ( fabs(distTraveled) < accelerationDist ) {
        // Initial acceleraction
        percent = fabs( distTraveled / accelerationDist );
        if ( percent < 0.2 ) { percent = 0.2;}
        dynSpeed = percent * maxSpeed;      
      } else {
        // Handle slowing down
        percent = ( remainingDist - minSpeedDist ) / ( accelerationDist - minSpeedDist );
        dynSpeed = percent * maxSpeed;

        if (dynSpeed > maxSpeed) { dynSpeed = maxSpeed; }
        if (dynSpeed < minSpeed) { dynSpeed = minSpeed; }
      }
    
    
      //cout << " " << endl;
      //cout << remainingDist << endl;
      //cout << percent << endl;
      //cout << dynSpeed << endl;
    } else {
      //cout << " " << endl;
      //cout << "Dist Traveled: " << distTraveled << endl;
      //cout << "Debug: " << endl;
      //cout << Fleft << " " << Fright << endl;
      //cout << Bleft << " " << Bright << endl;
    }


    
    //if ( fabs(dynSpeed) < 20 ) { offSet = 0;}
    
    if ( dist < 0 ) {
      setVel(-dynSpeed); 
    } else { 
      setVel(dynSpeed); 
    }
    
    
    leftMotorA.resetRotation();
    rightMotorA.resetRotation();
    leftMotorB.resetRotation();
    rightMotorB.resetRotation();
    wait(10, msec);
  }  

  setVel(0);

  cout << "Drive Dist Done" << endl;

  cout << "   Drive Dist Error: " << Drivetrain.heading(degrees) - startHeading << endl;

  Drivetrain.turnToHeading(startHeading, degrees);

  return true;
}

bool ai::sideDrive(int speed, double timeOut) {

  strafeLRL = speed;
  wait(timeOut, seconds);
  strafeLRL = 0;

  return true;
};


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
  
    changeRoller(true);

    driveDist(-4);
    Drivetrain.turnToHeading(150, degrees);

    PickerUper.setVelocity(100, percent);
    driveDist(17);

    Drivetrain.turnToHeading(90, degrees);
    PickerUper.setVelocity(0, percent);

    driveDist(9);

    changeRoller(true);

    driveDist(-6);

    Drivetrain.turnToHeading(0, degrees);

    driveDist(-55);

    LauncherGroup.spin(fwd, 9, vex::voltageUnits::volt);

    Drivetrain.turnToHeading(5, degrees);

    wait(2, seconds);

    LauncherFeeder.setVelocity(50, percent);

    wait(3, seconds);

    LauncherGroup.spin(fwd, 0, vex::voltageUnits::volt);
    Drivetrain.turnToHeading(0, degrees);
    LauncherFeeder.setVelocity(0, percent);

    driveDist(27);

    Drivetrain.turnToHeading(-86, degrees);

    // Pickup 3 long dist 

    
    //PickerUper.setVelocity(100, percent);

    driveDist(70);

    /*
    LauncherGroup.spin(fwd, 9, vex::voltageUnits::volt);
    Drivetrain.turnToHeading(130, degrees);

    wait(2, seconds);

    LauncherFeeder.setVelocity(30, percent);

    wait(4, seconds);

    LauncherGroup.spin(fwd, 0, vex::voltageUnits::volt);
    LauncherFeeder.setVelocity(0, percent);

  */

    driveDist(10, false);
    driveDist(-10, false);

    PickerUper.setVelocity(100, percent);

    Drivetrain.turnToHeading(-135, degrees);

    driveDist(70, false);

    PickerUper.setVelocity(0, percent);

    Drivetrain.turnToHeading(-90, degrees);

    driveDist(8);

    changeRoller(true);

    driveDist(-4);

    Drivetrain.turnToHeading(125, degrees);

    PickerUper.setVelocity(100, percent);

    driveDist(13);

    PickerUper.setVelocity(0, percent);

    Drivetrain.turnToHeading(180, degrees);

    driveDist(16);

    changeRoller(true);

    driveDist(-20);

    Drivetrain.turnToHeading(45, degrees);

    driveDist(20);

    expand();

    driveDist(-20);

  }
  if ( pathNum == 2 ) {
    //  "Paths/Left/Launch.txt",

    Drivetrain.setTurnVelocity(100, percent);

    driveDist(-6);
    //sideDrive(50, 0.75);
    
    Drivetrain.turnToHeading(-90, degrees);

    driveDist(15, false, 60);

    Drivetrain.turnToHeading(-139, degrees);
    PickerUper.setVelocity(90, percent);
    
    driveDist(20, false, 80);
  
    //Drivetrain.turnToHeading(-45, degrees);
    LauncherGroup.spin(fwd, 11, vex::voltageUnits::volt);
    
    //driveDist(-8, false, 40);
    Drivetrain.turnToHeading(-26, degrees);

    driveDist(-6);

    LauncherFeeder.setVelocity(15, percent);
    
    wait(10, seconds);
    
    LauncherGroup.setVelocity(0, percent);
    LauncherFeeder.setVelocity(0, percent);
    PickerUper.setVelocity(0, percent);

  }
  if ( pathNum == 3 ) {
    //  "Paths/Right/GoToRoller.txt",

    //driveDist(4, false, 30);
    Drivetrain.setHeading(-90, degrees);
    driveDist(20, false, 50);
    Drivetrain.turnToHeading(0, degrees);
    driveDist(5, false, 30);
    
  }
  if ( pathNum == 4 ) {
    //  "Paths/Right/RollerToMid.txt",

    
    LauncherGroup.spin(fwd, 10, vex::voltageUnits::volt);

    driveDist(-3, false, 30);
    Drivetrain.turnToHeading(130, degrees);
    PickerUper.setVelocity(100, percent);
    driveDist(37, false, 70);
    
    Drivetrain.turnToHeading(30, degrees);
    PickerUper.setVelocity(0, percent);

    driveDist(-10, false, 50);

    LauncherFeeder.setVelocity(20, percent);

  }
  if ( pathNum == 5 ) {
    //  "test.txt"

  }

  return true;
};