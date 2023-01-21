using namespace vex;

extern brain Brain;

// VEXcode devices
extern drivetrain Drivetrain;
extern controller Controller1;
extern motor_group LauncherGroup;
extern motor PickerUper;
extern motor LauncherFeeder;
extern limit DiskLimitSwitch;
extern digital_out fnewmatics;
extern digital_out fnewmaticsB;


extern motor rightMotorA;
extern motor rightMotorB;
extern motor leftMotorA;
extern motor leftMotorB;

extern int motorFL;
extern int motorFR;
extern int motorBL;
extern int motorBR;

extern bool replaying;

extern int andrewDriving;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void  vexcodeInit( void );