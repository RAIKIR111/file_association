# File Association 0.3.0

## Main Changes

1. Multiple service registration has been removed
2. The bug with calling an Introspection method has been fixed
3. Added remove methods for both applications' domains and types
4. .deb file now works!
5. Added configuration file
6. Added different options for calling the application

and some of the code has become more readable

## Description

There is a demo version of the console application named "File Association" that proccesses and stores data about different applications and types they can use. It was developed by using Qt QDBus technologies and JSON (to store the data while the application is turned of).

## System requirements

qmake6, qdbus, g++, cmake, .deb

Use following commands in terminal to install packages above:
1. **sudo apt install build-essential**
2. **sudo apt install cmake**
3. **sudo apt install qmake6**
4. **sudo apt install dpkg**
5. **sudo apt install qdbus-qt5**

## Deployment instructions

1. Download file-association-0.3.0.deb
2. In terminal **cd Downloads**
3. In terminal **sudo apt install ./file-association-0.3.0.deb**
4. In terminal **file_association** to start the application (and get instuctions how to use it)

***OR IF YOU WANT TO BUILD IT YOURSELF***

1. Download whole file_association directory
2. In file_association directory **mkdir build** -> **cd build** -> **cmake ../** -> **make**
3. In terminal **./file_association** to start the application (and get instuctions how to use it)

If you want to reproduce FA_Adaptor.cpp/.h and FA_Interface.cpp/.h use following commands in terminal:
1. **qdbusxml2cpp fileassociation_necessaries.xml -a FA_Adaptor**
2. **qdbusxml2cpp fileassociation_necessaries.xml -p FA_Interface**
