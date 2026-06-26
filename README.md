# CSOPESY-MO1
CSOPESY Major Output: Process Scheduler and CLI

Members:
Avelino, Sophia Kylie G.
Chua, Renzo Sheldon Trevor K.
Liong, John Lawrence O.

Instructions: -
Entry class file: - 
*Add a README.txt with your name and instructions on running your program. Also, indicate the entry class file where the main function is located. An alternative can be a GitHub link.*

---
1. Delete csopesy-log.txt before starting
- csoesy-log.txt doesnt overwrite but appends. can change in reportUtilService.cpp.
2. Ctrl shift b to build, then press run


## TODO:
processes got changed to a shared ptr to change everythig accordingly. reference is SystemState.cpp, screnservice
- fix reportUtilService.cpp

- initialize: nothing should work until initialized
- screen -ls: cpu core usage
- screen -s, process-smi

### Demo notes:
- video quality must be 480p - 720p
- max size: 1GB
- show you pressing Run/Debug from IDE




--
random code notes:
int main()
{

    ConsoleManager consoleManager;
    consoleManager.setCursorPosition(0, 1);
}



void ConsoleManager::setCursorPosition (int posX, int posY) const
{
    COORD coord;
    coord.X = posX;
    coord.Y = posY;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}