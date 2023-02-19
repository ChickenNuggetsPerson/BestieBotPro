#include <string>



// This is the file that defines the ai class

class ai {
  private:

    int personality[10] = {};
    int personalitySize = 10;
    
    // Behaviors
    bool changeRoller( bool longer ); // 10
    bool expand();                    // 11


    int startPos;

    bool isWhite(float val);

    // Private Methods
    bool runTask( int taskNum );

    bool runPath( int pathNum );
    
    // Misc things 
    int teamColor = 0;
    int currentTask = -1;

    bool setVel(double lVel, double rVel = 10000);

  public:
    
    // Methods
    ai( int fallbackColor );
    void init();
    
    bool iterate();

    bool running = true;

    void aiError(const char* message);
    void aiDebug(const char* message);

    bool turnTo(int rot, double timeOut = 0);
    bool driveDist(double dist, bool dynamicSpeed = true, int speed = 50, double timeOut = 0);
    bool sideDrive(int speed, double timeOut);
    
    unsigned int readFile(const char* fileName);
    void writeFile(const char* fileName, unsigned int numToWrite);

};