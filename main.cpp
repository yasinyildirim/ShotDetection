/*#******************************************************************************
 ** IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
 **
 ** By downloading, copying, installing or using the software you agree to this license (LGPL).
 ** If you do not agree to this license, do not download, install,
 ** copy or use the software.
 **
 ** See COPYING file for license information.
 **
 **  Creation - July 2015
 **      Author: Yasin Yıldırım (yildirimyasi@gmail.com), Divit Research, Istanbul, Turkey
 **
*******************************************************************************/

#include "shotdetector.h"

#define DEFAULT_THRESHOLD 0.49
#define APP_VERSION "1.0.0"
#define ENABLE_GUI false
#define DEFAULT_SAMPLE_PERIOD 0

using namespace std;
using namespace cv;

void show_help(char** );

int main(int argc, char** argv)
{
    double threshold = DEFAULT_THRESHOLD;
    bool showGUI = ENABLE_GUI;
    int sample_period = DEFAULT_SAMPLE_PERIOD;
    string videoFile, outputPath;
    if (argc < 4) { // Check the value of argc. If not enough parameters have been passed, inform user and exit.
        show_help(argv);
        exit(0);
    }
    for (int i = 1; i < argc; i++) { /* We will iterate over argv[] to get the parameters stored inside.
                                              * Note that we're starting on 1 because we don't need to know the
                                              * path of the program, which is stored in argv[0] */
        if (i + 1 != argc){ // Check that we haven't finished parsing already
            if (string(argv[i]) == "-t") {
                // We know the next argument *should* be the threshold:
                threshold = atof( argv[i + 1] );
            } else if (string(argv[i]) == "-i") {
                videoFile =  argv[i + 1];

            } else if (string(argv[i]) == "-o") {
                outputPath = argv[i + 1];
            } else if (string(argv[i]) == "-s") {
                sample_period = atoi( argv[i + 1] );
            } else if (string(argv[i]) == "-h") {
                show_help(argv);
            }
        }
        if (string(argv[i]) == "-show") {
            showGUI = true;
        }
    }
    if(outputPath.compare("") == 0 || outputPath.compare(" ") == 0 ){
        //default output filename
        outputPath = "result";
    }
    switch(showGUI){
    case true:
    {
        ShotDetector sd(videoFile, threshold, sample_period);
        sd.processVideo(outputPath, ShotDetector::XML);
        break;
    }
    case false:
    {
        cout <<"video file: " << videoFile <<endl;
        ShotDetector sd(videoFile, threshold, sample_period);
        int64 start_t =  cv::getTickCount();
        sd.processVideo_NoGUI(outputPath, ShotDetector::XML);

        int64 stop_t =  cv::getTickCount();
        int64 elapsed_t = stop_t - start_t;
        double time_elapsed = elapsed_t / cv::getTickFrequency();
        cout << "time elapsed: "<< time_elapsed <<" seconds" <<endl;
        break;
    }
    default:
    {
        //should never been reached
        cout<< "Unknkown Error" <<endl;
        break;
    }
    }

    return 0;
}

void show_help(char **argv) {
    cout<<"\nShotdetect version "<< APP_VERSION <<", Copyright (c) 2015 Yasin Yıldırım" <<endl<<endl<<
          "Usage: " <<  argv[0] << endl <<
          "-h               : show this help\n"
          "-t threshold     : threshold (Default = "<< DEFAULT_THRESHOLD << ")\n"
          "-i file          : input file path\n"
          "-o output_path   : save detected shots to output path "<<endl<<
          "-s sample_period : set the sample period of stored frames. (Default = "<< DEFAULT_SAMPLE_PERIOD <<")\n"<<
          "-show            : display the shots on GUI (Graphical Version)" <<endl;
}
