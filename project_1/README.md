# SC3020-project-1

## Running the program
### Running on VS Code
1. Follow [this guide](https://code.visualstudio.com/docs/languages/cpp) to ensure that C/C++ support for VS Code has been set up correctly
2. Open your [.vscode/tasks.json](../.vscode/tasks.json) file and modify the "command" field to point to the correct location of the g++ compiler. Replace the existing "command" value with the full path to your g++.exe. For example:
```json
"command": "C:\\Users\\YourUsername\\mingw64\\bin\\g++.exe",
```
3. Press the play button in the top right corner of the editor
4. Choose C/C++: g++.exe build and debug active file from the list of detected compilers on your system.

### Running through CLI
1. Compile .cpp files
```console
g++ -o main main.cpp tree/*.cpp storage/*.cpp
```
2. Run the executable
```
./main
```

## Contributions
This project was built by the following:
1. [Peh Yu Ze](https://github.com/pehyuze)
2. [Lee Ding Zheng](https://github.com/leedz31)
3. [Johnathan Chow](https://github.com/john14759)
4. [Tai Chen An](https://github.com/taica00)
