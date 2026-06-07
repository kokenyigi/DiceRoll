# DiceRoll
This is a windows-based application which let's you roll dice of various numbers and dimensions. Great for a good D&amp;D game with your friends! The App is Win32 specific, due to its static libraries.

## Guide To Building & Running
The App requires Cmake & MSVC toolchain present for building and compilation. After cloning the repository, use the following cmd script guide: <br><br>
First, create a build directory:<br>
```mkdir build```<br>
```cd build```<br>
<br>
Then, run this script in order to generate the necessary build files: <br>
```cmake .. -G "Visual Studio 17 2022" -A Win32```<br>
<br>
After this, in order to fully compile the application use the following: <br>
```cmake --build .```<br>
<br>
Now, there should be an .exe executable file inside the working directory, running it will result in the application launching.

## Guide To Controls
The app uses a free Camera, this means WASD,SPACE,LSHIFT determine the 6 directions of movement, pressing right click will enable/disable the mouse. <br>In order to interact with the GUI present inside the App, you must disable the mouse first (right click)
