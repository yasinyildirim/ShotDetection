/*#******************************************************************************
 ** IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
 **
 ** By downloading, copying, installing or using the software you agree to this license.
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

namespace cv {
double compareHistCustom( InputArray _H1, InputArray _H2, int method );
}
class ShotDetector
{
public:
    enum OutputFormat {XML, YAML, TEXT};
    ShotDetector(std::string filename, double threshold);
    ShotDetector(std::string filename, double threshold, int sample_period);
    void processVideo(std::string outputFileName, OutputFormat format);
    void processVideo_NoGUI(std::string outputFileName, OutputFormat format);
    std::string videoPath;
    static bool shotBoundaryDetect(cv::Mat &prevFrame, cv::Mat& currntFrame, int threshold );
    bool shotBoundaryDetect(cv::Mat &prevFrame, cv::Mat& currntFrame );
    bool shotBoundaryDetectHist(cv::MatND &prevHist, cv::MatND& currntFrame );
    cv::MatND prepareFrame(cv::Mat &frame);
    cv::Mat getShotFromVideo(double frame_number);
private:
    std::string miliseconds_to_DHMS(double duration);
    cv::Mat currentFrame;
    double threshold;
    int sample_period;

};

#endif // SHOTDETECTOR_H
