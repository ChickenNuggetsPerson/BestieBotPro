/*vex-vision-config:begin*/
#include "vex.h"
vex::vision::signature V_DISK = vex::vision::signature (1, -327, 227, -50, -4161, -3403, -3782, 3.9, 0);
vex::vision::signature V_GOAL_BLUE = vex::vision::signature (2, 0, 0, 0, 0, 0, 0, 3, 0);
vex::vision::signature V_GOAL_RED = vex::vision::signature (3, 0, 0, 0, 0, 0, 0, 3, 0);
vex::vision::signature SIG_4 = vex::vision::signature (4, 0, 0, 0, 0, 0, 0, 2.5, 0);
vex::vision::signature SIG_5 = vex::vision::signature (5, 0, 0, 0, 0, 0, 0, 2.5, 0);
vex::vision::signature SIG_6 = vex::vision::signature (6, 0, 0, 0, 0, 0, 0, 2.5, 0);
vex::vision::signature SIG_7 = vex::vision::signature (7, 0, 0, 0, 0, 0, 0, 2.5, 0);
vex::vision Vision = vex::vision (vex::PORT8, 33, V_DISK, V_GOAL_BLUE, V_GOAL_RED, SIG_4, SIG_5, SIG_6, SIG_7);
/*vex-vision-config:end*/