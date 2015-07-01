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
#include <cstdio>

using namespace cv;
using namespace std;

/**
 * @brief ShotDetector::ShotDetector
 * @param filename: Video filename or full path
 * @param threshold: Threshold value for shot detection.
 */
ShotDetector::ShotDetector(std::string filename, double threshold)
{
    this->videoPath = filename;
    this->threshold = threshold;
}


bool ShotDetector::shotBoundaryDetect(cv::Mat &prevFrame, cv::Mat& currntFrame, int threshold ){
    /**
     ** conversion from multi-channel to grayscale
     **
    if(prevFrame.channels() > 1){
        if(prevFrame.channels() == 3)
        cvtColor( prevFrame, prevFrame, CV_RGB2GRAY );
        else if(prevFrame.channels() == 4)
            cvtColor( prevFrame, prevFrame, CV_RGBA2GRAY );
    }
    if(currntFrame.channels() > 1){
        if(currntFrame.channels() == 3)
        cvtColor( currntFrame, currntFrame, CV_RGB2GRAY );
        else if(currntFrame.channels() == 4)
            cvtColor( currntFrame, currntFrame, CV_RGBA2GRAY );
    }
    **/

    int channels[] = {0, 1, 2};
    MatND prevHist, currHist;
    //histogram size (bins) for r,g,b channels equal to 32.
    int histSize[] = {32, 32, 32};
    // color ranges for rgb channels are between (0, 256)
    float color_ranges[] = { 0, 256 };
    const float* ranges[] = { color_ranges, color_ranges, color_ranges };
    calcHist(&prevFrame, 1, channels, Mat(), prevHist, 3, histSize, ranges, true, false);
    calcHist(&currntFrame, 1, channels, Mat(), currHist, 3, histSize, ranges, true, false);

    // normalize histograms
    double s = cv::sum(prevHist)[0];
    prevHist.convertTo(prevHist, prevHist.type(), 1./s, 0);

    s = cv::sum(currHist)[0];
    currHist.convertTo(currHist, currHist.type(), 1./s, 0);

    /**
    * OpenCV normalize() method supports up to 2 dimensions. So, this does not work on 3D histograms
    normalize( prevHist, prevHist, 0, 1, NORM_MINMAX, -1, Mat() );
    normalize(currHist, currHist, 0, 1, NORM_MINMAX, -1, Mat());
    **/

    // calculate the Chi-Squre distance of histograms of the adjacent frames.
    double result = compareHist( prevHist, currHist, CV_COMP_CHISQR );

    /** print the results for debugging and testing
    cout << "result: " << result <<"  ";
    **/

    if(result > threshold)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief ShotDetector::shotBoundaryDetect: This method evaluates two adjacent frames
 * and compares RGB histograms of frames in order to detect shot boundary.
 * @param prevFrame : Previous Frame (matrix format)
 * @param currntFrame: Current Frame (matrix format)
 * @return: Returns true if shot is detected, otherwise returns false
 */
bool ShotDetector::shotBoundaryDetect(cv::Mat &prevFrame, cv::Mat& currntFrame){

    int channels[] = {0, 1, 2};
    MatND prevHist, currHist;
    //histogram size (bins) for r,g,b channels equal to 32.
    int histSize[] = {32, 32, 32};
    // color ranges for rgb channels are between (0, 256)
    float color_ranges[] = { 0, 256 };
    const float* ranges[] = { color_ranges, color_ranges, color_ranges };
    calcHist(&prevFrame, 1, channels, Mat(), prevHist, 3, histSize, ranges, true, false);
    calcHist(&currntFrame, 1, channels, Mat(), currHist, 3, histSize, ranges, true, false);

    // normalize histograms
    double s = cv::sum(prevHist)[0];
    prevHist.convertTo(prevHist, prevHist.type(), 1./s, 0);

    s = cv::sum(currHist)[0];
    currHist.convertTo(currHist, currHist.type(), 1./s, 0);

    /**
    * OpenCV normalize() method supports up to 2 dimensions. So, this does not work on 3D histograms
    normalize( prevHist, prevHist, 0, 1, NORM_MINMAX, -1, Mat() );
    normalize(currHist, currHist, 0, 1, NORM_MINMAX, -1, Mat());
    **/

    // calculate the Chi-Squre distance of histograms of the adjacent frames.
    double result = compareHist( prevHist, currHist, CV_COMP_CHISQR );

    /** print the results for debugging and testing
    cout << "result: " << result <<"  ";
    **/

    if(result > threshold)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief ShotDetector::processVideo: This method process video and detect shot boundaries
 * at video. Results are stored in a file.
 * @param outputFileName: Results are stored in given filename
 * @param format: Format type of output file. It can be XML, YAML or TEXT (basic txt file format)
 */
void ShotDetector::processVideo(std::string outputFileName, OutputFormat format){
    VideoCapture cap(videoPath);
    stringstream ss;
    ss << outputFileName;
    if(format == XML){
        ss << ".xml";
    }else if(format == YAML){
        ss << ".xml";
    }else if(format == TEXT){
        ss << ".txt";
    }else{
        //should never been reached
    }
    outputFileName = ss.str();
    //store the result in xml format
    FileStorage fstorage(outputFileName, FileStorage::WRITE);

    /** store the information whether shot is found at previous frame
     * in order to detect fades and dissolves
    **/
    bool shotFoundAtPrev = false;

    if(!cap.isOpened()){
        cout<<"error openning video!!" << endl;
        return;
    }
    Mat prevFrame;
    cap >> prevFrame;
    fstorage << "Header" << "[" ;
    fstorage <<"{:"
            << "video_path" << videoPath
            << "fps" << (int) cap.get(CV_CAP_PROP_FPS)
            <<"frame_count" << (int) cap.get(CV_CAP_PROP_FRAME_COUNT) << "}" << "]" ;

    fstorage << "Shots" << "[" ;

    while(1){
        Mat grabbedFrame, originalFrame;
        cap >> grabbedFrame;
        originalFrame = grabbedFrame.clone();
        if(grabbedFrame.empty()){
            cout<<"empty frame!" << endl;
            break;
        }
        bool result = shotBoundaryDetect(prevFrame, grabbedFrame);
        if(result){
            if(!shotFoundAtPrev)
            {
                fstorage << "{:"<< "frame_number" <<(int) cap.get(CV_CAP_PROP_POS_FRAMES) << "time" << miliseconds_to_DHMS( cap.get(CV_CAP_PROP_POS_MSEC) ) << "}";

                while(1){
                    cout<< "Shot boundary! press 'c' or 'C' to continue" << endl;
                    char k = waitKey(0);
                    if( k == 'c' || k == 'C' ){
                        break;
                    }
                }

            }
            shotFoundAtPrev = true;
        }else{
            shotFoundAtPrev = false;
        }
        prevFrame = grabbedFrame.clone();
        imshow("Video", originalFrame);

        char key = waitKey(30);
        if(key == 'q' || key == 'Q') break;
    }
    fstorage << "]" ;
    fstorage.release();
}

void ShotDetector::processVideo_NoGUI(std::string outputFileName, OutputFormat format){
    VideoCapture cap(videoPath);
    stringstream ss;
    ss << outputFileName;
    if(format == XML){
        ss << ".xml";
    }else if(format == YAML){
        ss << ".xml";
    }else if(format == TEXT){
        ss << ".txt";
    }else{
        //should never been reached
    }
    outputFileName = ss.str();
    //store the result in xml format
    FileStorage fstorage(outputFileName, FileStorage::WRITE);
    /** store the information whether shot is found at previous frame in order to detect fades and dissolves  **/
    bool shotFoundAtPrev = false;

    if(!cap.isOpened()){
        cout<<"error openning video!!" << endl;
        return;
    }
    Mat prevFrame;
    cap >> prevFrame;
    fstorage << "Header" << "[" ;
    fstorage <<"{:"
            << "video_path" << videoPath
            << "fps" << (int) cap.get(CV_CAP_PROP_FPS)
            <<"frame_count" << (int) cap.get(CV_CAP_PROP_FRAME_COUNT) << "}" << "]" ;

    fstorage << "Shots" << "[" ;

    while(1){
        Mat grabbedFrame, originalFrame;
        cap >> grabbedFrame;
        originalFrame = grabbedFrame.clone();
        if(grabbedFrame.empty()){
            cout<<"empty frame!" << endl;
            break;
        }
        bool result = shotBoundaryDetect(prevFrame, grabbedFrame);
        if(result){
            if(!shotFoundAtPrev)
            {
                fstorage << "{:"<< "frame_number" <<(int) cap.get(CV_CAP_PROP_POS_FRAMES) << "time" << miliseconds_to_DHMS( cap.get(CV_CAP_PROP_POS_MSEC) ) << "}";
            }
            shotFoundAtPrev = true;
        }else{
            shotFoundAtPrev = false;
        }
        prevFrame = grabbedFrame.clone();
    }
    fstorage << "]" ;
    fstorage.release();
}

/**
 * @brief ShotDetector::miliseconds_to_DHMS: This method converts time interval in miliseconds format to Day-Hour-Minute-Second format
 * @param duration: duration in miliseconds
 * @return: Returns D:H:M:S format as a string
 */
std::string ShotDetector::miliseconds_to_DHMS(double duration){

    char buffer[20];
    int buffer_length;
    uint timestamp_as_seconds = duration / 1000;
    int seconds = (int) (timestamp_as_seconds % 60);
    timestamp_as_seconds /= 60;
    int minutes = (int) (timestamp_as_seconds % 60);
    timestamp_as_seconds /= 60;
    int hours = (int) (timestamp_as_seconds % 24);
    int days = (int) (timestamp_as_seconds / 24);
    if( (hours == 0) && (days == 0) ){
        buffer_length = sprintf(buffer, "%02d:%02d", minutes, seconds);
    }else if(days == 0){
        buffer_length = sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);
    }else{
        buffer_length = sprintf(buffer, "%dd:%02d:%02d:%02d", days, hours, minutes, seconds);
    }
    std::string result(buffer, buffer_length);
    return result;
}



