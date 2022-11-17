#include "vex.h"
#include "userAutonomous.h"

#include "string"

#include "visionConfig.h"

using namespace vex;


// Constructor method 
// Inits the vision sensor and the interal team color variable
ai::ai( const char* inputTeamColor ) {

  teamColor = inputTeamColor;

  Vision.setLedBrightness(0);
  Vision.setSignature(V_DISK);

  //aiError("yeet");
  
};



// Displays error on brain screen
void ai::aiError(const char* message) {
  Brain.Screen.newLine();
  Brain.Screen.setPenColor(vex::color::red);
  Brain.Screen.print(message);
  Brain.Screen.setPenColor(vex::color::white);
}

// Displays error on brain screen
void ai::aiDebug(const char* message) {
  Brain.Screen.newLine();
  Brain.Screen.setPenColor(vex::color::purple);
  Brain.Screen.print(message);
  Brain.Screen.setPenColor(vex::color::white);
}


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
    return true;     
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
  if (strcmp(personality[taskNum], "roller") == 0) { return changeRoller(); };
  if (strcmp(personality[taskNum], "collect") == 0) { return collectDisk(); };

  return false; // Gives an error if it can not find the specified behavior
};




// Behaviors 

// Behaviors return either true or false
// True = The task was completed correctly
// False = The task was not able to be completed for some reason

bool ai::changeRoller() {

  Brain.Screen.newLine();
  Brain.Screen.print("Starting Roller Code");
  
  return true;
};

bool ai::collectDisk() {

  Brain.Screen.newLine();
  Brain.Screen.print("Starting Collect Disk");

  wait(5, seconds);
    
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