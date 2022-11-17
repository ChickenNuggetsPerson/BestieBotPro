

#include <string>

class ai {
  private:
    
    const char* personality[2] = {"collect", "roller"};
    int personalitySize = 2;
    
    // Behaviors
    bool changeRoller();              // roller
    bool collectDisk();               // collect

    // Private Methods
    bool runTask( int taskNum );
    
    // Misc things 
    const char* teamColor = "";
    int currentTask = -1;

    void aiError(const char* message);
    void aiDebug(const char* message);




  public:
    
    // Methods
    ai( const char* inputTeamColor );
    void diskEntered();


    bool iterate();

    bool running = true;
    
    

    
};