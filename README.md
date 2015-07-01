# ShotDetection
Open source software that detects shot boundaries in video.

##1. Introduction

Shotdetector is a free software (LGPL) which detects shot boundaries from a video.
The result can be produced in various formats. Produced file contains frame numbers and durations of detected shots
in addition to general information of analyzed video.
Currently, XML, YAML and TEXT (simple .txt file) formats are supported.

##2. Dependencies

 - OpenCV 2.3 or greater
 - g++ (gcc)

##3. Usage

          -h               : Show help
          -s threshold     : Set threshold value. This value specifies sensitivity of detecting shots.
          -i file          : Sets input video file
          -o output_path   : save detected shots to output path 'output_path'
          -show            : display the shots on GUI (Graphical Version)
Example: 
./ShotDetection -i test.mp4 -o outputs -show

