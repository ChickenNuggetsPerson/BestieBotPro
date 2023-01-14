#include <string>



// This is the file that defines the ai class

class ai {
  private:
    

    int personality[10] = {};
    int personalitySize = 10;
    
    // Behaviors
    bool changeRoller();              // 10
    bool expand();                    // 11


    int startPos;

    bool isWhite(float val);

    // Private Methods
    bool runTask( int taskNum );
    
    // Misc things 
    int teamColor = 0;
    int currentTask = -1;



    const char* pathNames[4] = {
      "Paths/Skills/main.txt",
      "Paths/Left/Launch.txt",
      "Paths/Right/GoToRoller.txt",
      "Paths/Right/RollerToMid.txt"
    };

  public:
    
    // Methods
    ai( int fallbackColor );
    void init();

    bool skillsCheck = false;
    
    bool iterate();

    bool running = true;

    void aiError(const char* message);
    void aiDebug(const char* message);

    bool replay(const char* pathFile);  
    
    unsigned int readFile(const char* fileName);
    void writeFile(const char* fileName, unsigned int numToWrite);

};