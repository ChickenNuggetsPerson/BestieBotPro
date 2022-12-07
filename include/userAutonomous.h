#include <string>



// This is the file that defines the ai class

class ai {
  private:
    

    int personality[10] = {};
    int personalitySize = 10;
    
    // Behaviors
    bool changeRoller();              // 1
    bool collectDisk();               // 2
    bool stationaryLaunch();          // 3

    // Private Methods
    bool runTask( int taskNum );
    
    // Misc things 
    int teamColor = 0;
    int currentTask = -1;

  public:
    
    // Methods
    ai( int fallbackColor );
    void init();

    void diskEntered();


    bool iterate();

    bool running = true;

    void aiError(const char* message);
    void aiDebug(const char* message);


    
    unsigned int readFile(const char* fileName);
    void writeFile(const char* fileName, unsigned int numToWrite);

};