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
extern line LineLeft;
extern line LineMid;
extern line LineRight;


extern unsigned int x, y;
extern const char* pathNames[4];


/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void  vexcodeInit( void );