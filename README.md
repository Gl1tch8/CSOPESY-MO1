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
/class activities - put the class activities related to the MO1 here
format: <Week#><Surname> <Topic>

## TODO:
- make ascii for "CSOPESY" 
- make input loop
- for each cpp file, make a header file and put them in /include
    - then, replace the .cpp to .hpp for EACH file (For example, check ClearCommand.cpp)

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