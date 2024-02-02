# File Association 0.2.0

## Main Changes

1. Multiple service registration has been removed
2. Added different options for calling the application
3. The bug with calling an Introspection method has been fixed
4. Added remove methods for both applications' domains and types
5. .deb file now works!

and some of the code has become more readable

## Description

There is a demo version of the console application named "File Association" that proccesses and stores data about different applications and types they can use. It was developed by using Qt QDBus technologies and JSON (to store the data while the application is turned of).

## System requirements

qmake6, qdbus, g++, cmake, .deb

Use following commands in terminal to install packages above:
**sudo apt install build-essential**
**sudo apt install cmake**
**sudo apt install qmake6**
**sudo apt install dpkg**
**sudo apt install qdbus-qt5**

## Deployment instructions

1. Download file-association-0.2.0.deb
2. In terminal **cd Downloads**
3. In terminal **dpkg-deb --build file-association-0.2.0/**
4. In terminal **file_association** to start the application (and get instuctions how to use it)

***OR IF YOU WANT TO BUILD IT YOURSELF***

1. Download whole file_association directory
2. In file_association directory **mkdir build** -> **cd build** -> **cmake ../** -> **make**
3. In terminal **./file_association** to start the application (and get instuctions how to use it)

To reproduce FA_Adaptor.cpp/.h and FA_Interface.cpp/.h use following commands in terminal:
**qdbusxml2cpp fileassociation_necessaries.xml -a FA_Adaptor**
**qdbusxml2cpp fileassociation_necessaries.xml -p FA_Interface**
