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

#ifndef SHOTDETECTOR_H
#define SHOTDETECTOR_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

class ShotDetector
{
public:
    enum OutputFormat {XML, YAML, TEXT};
    ShotDetector(std::string filename, double threshold);
    void processVideo(std::string outputFileName, OutputFormat format);
    void processVideo_NoGUI(std::string outputFileName, OutputFormat format);
    std::string videoPath;
    bool shotBoundaryDetect(cv::Mat &prevFrame, cv::Mat& currntFrame, int threshold );
     bool shotBoundaryDetect(cv::Mat &prevFrame, cv::Mat& currntFrame );
private:
     std::string miliseconds_to_DHMS(double duration);
    cv::Mat currentFrame;
    double threshold;
};

#endif // SHOTDETECTOR_H
