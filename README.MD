# QPOP
QPOP is an remote Leauge of Legends queue joiner. Running the desktop application with the companion app on your android phone allows you to be notified when your queue has ended and join from anywhere on the local network.

## DEPENDENCIES
For compilation the following libraries are required:
* **Boost Libraries** - a dependency of websocketpp
* **websocketpp** - used to initiate and transfer data
* **wxWidgets** - to create the graphical user interface

In addition, the construction of the Solution file is dependent on **CMake**.

## COMPILATION
Navigate to the QPOP main folder then:
* Create a folder titled *build*
* Navigate into the folder and either use CMake's graphical interface or run the command: *cmake ..*
* The solution must be compiled at 32-bit. Make sure when selecting the compiler it is the 32-bit version
* Any libraries that are not installed in their default location need to be manually set in **CMake**
* Open the solution file in **Microsoft Visual Studio**
* Then compile either the Release or Debug version
* The output file will be in the *executable/* whatever version was compiled. For example, if the release version was compiled: *executable/Release*
* Before running copy the files in the *executable* folder - *accept.bmp* and *lol.txt* into the folder with the executable

## USING THE APPLICATION
The desktop application is a very simple window. If it has not sensed the League of Legends process running it will appear:
![window example](https://user-images.githubusercontent.com/13967957/33510367-6331b1ac-d6c0-11e7-9ae3-f6fdd112f66a.png)

The top number is the code to be entered into the mobile companion app. The bottom letters *connection* will turn green when a phone is connected to the application.

When the League of Legends Process is detected, the window will change to include the matching percentage between a screenshot of the current program and the a specific picture.

![process found](https://user-images.githubusercontent.com/13967957/33510365-61666d72-d6c0-11e7-81ca-037ecddc972e.PNG)

## CHANGING THE CONFIGURATION
*lol.txt* is the configuration file. It can be changed to scan other programs, or adapted if the League Client changes.
The format:
* Configuration Name
* Executable associated with the program
* Base directory
* Image to compare
* Default X resolution
* Default Y resolution
If you are using a section of a screenshot of a program change the next two lines as follows
* X offset
* Y offset
* Threshold to signal mobile app

Note: If the threshold is positive, then if the result is above the threshold, trigger. If the threshold is below, the trigger will happen when it falls below the threshold.