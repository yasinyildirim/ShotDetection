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
 **      Author: Yasin Yıldırım (yildirimyasi@gmail.com), Istanbul, Turkey
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
ShotDetector::ShotDetector(std::string filename, double threshold): sample_period(0)
{
    this->videoPath = filename;
    this->threshold = threshold;
}

ShotDetector::ShotDetector(std::string filename, double threshold, int sample_period)
{
    this->videoPath = filename;
    this->threshold = threshold;
    this->sample_period = sample_period;
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
    double result = compareHistCustom( prevHist, currHist, CV_COMP_CHISQR );

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
    double result = compareHistCustom( prevHist, currHist, CV_COMP_CHISQR );

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
 * @brief ShotDetector::shotBoundaryDetectHist: This method evaluates two adjacent frames
 * and compares RGB histograms of frames in order to detect shot boundary.
 * The difference from previous method is this method takes precomputed histogram of
 * previous frame to avoid double computation of histogram in video processing loop.
 * @param prevHist : Histogram of Previous Frame (matrix format)
 * @param currHist: Histogram of Current Frame (matrix format)
 * @return: Returns true if shot is detected, otherwise returns false
 */
bool ShotDetector::shotBoundaryDetectHist(cv::MatND &prevHist, cv::MatND& currHist){

    // calculate the Chi-Squre distance of histograms of the adjacent frames.
    double result = compareHistCustom( prevHist, currHist, CV_COMP_CHISQR );

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
 * @brief ShotDetector::prepareFrame: This method calculates RGB color histogram of input image
 * and normalizes histogram between (0, 1).
 * @param frame: input image
 * @return: Normalized histogram as a MATND multi dimentional matrix
 */
cv::MatND ShotDetector::prepareFrame(cv::Mat &frame){
    int channels[] = {0, 1, 2};
    MatND currHist;
    //histogram size (bins) for r,g,b channels equal to 32.
    int histSize[] = {32, 32, 32};
    // color ranges for rgb channels are between (0, 256)
    float color_ranges[] = { 0, 256 };
    const float* ranges[] = { color_ranges, color_ranges, color_ranges };
    calcHist(&frame, 1, channels, Mat(), currHist, 3, histSize, ranges, true, false);

    // normalize histograms
    double s = cv::sum(currHist)[0];
    currHist.convertTo(currHist, currHist.type(), 1./s, 0);
    return currHist;
}

/**
 * @brief ShotDetector::processVideo: This method process video and detect shot boundaries
 * at video with graphical interface. Results are stored in a file.
 * @param outputFileName: Results are stored in given filename
 * @param format: Format type of output file. It can be XML, YAML or TEXT (basic txt file format)
 */
void ShotDetector::processVideo(std::string outputFileName, OutputFormat format){
    VideoCapture cap(videoPath);
    stringstream ss;
    ss << outputFileName;


#ifdef _WIN32
    //create directory if not exists
    string create_dir_command("mkdir ");
#else
    //create directory if not exists
    string create_dir_command("mkdir -p ");
#endif
    create_dir_command += outputFileName;
    system(create_dir_command.c_str());

    if(format == XML){
        ss << ".xml";
    }else if(format == YAML){
        ss << ".xml";
    }else if(format == TEXT){
        ss << ".txt";
    }else{
        //should never been reached
    }

    //store the result in xml format
    FileStorage fstorage(ss.str(), FileStorage::WRITE);

    /** store the information whether shot is found at previous frame
     * in order to detect fades and dissolves
    **/
    bool shotFoundAtPrev = false;
    bool shotStartStored = false;

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
    fstorage << "{:"<< "begin_frame_number" <<(int) cap.get(CV_CAP_PROP_POS_FRAMES) << "begin_time" << miliseconds_to_DHMS( cap.get(CV_CAP_PROP_POS_MSEC) ) ;
    shotStartStored = true;

    //store the shot frame to output path
#ifdef _WIN32
    string rootShotPath(outputFileName);
    if(outputFileName.at(outputFileName.size() -1) != '\\')
        rootShotPath.append("\\");
#else
    string rootShotPath(outputFileName);
    if(outputFileName.at(outputFileName.size() -1) != '/')
        rootShotPath.append("/");
#endif

    //save the inital frame which is the start of first shot.
    string initialShotPath(rootShotPath);
    stringstream filestream;
    filestream << "frame_" << cap.get(CV_CAP_PROP_POS_FRAMES) <<".jpg";
    initialShotPath.append(filestream.str());
    imwrite(initialShotPath, prevFrame);
    int frameCounter = 0;

    while(1){
        Mat grabbedFrame, originalFrame;
        cap >> grabbedFrame;
        originalFrame = grabbedFrame.clone();
        if(grabbedFrame.empty()){
            cout<<"empty frame!" << endl;
            if(!shotFoundAtPrev)
            {
                int frame_number = (int) cap.get(CV_CAP_PROP_POS_FRAMES);
                fstorage << "end_frame_number" << frame_number<< "end_time" << miliseconds_to_DHMS( cap.get(CV_CAP_PROP_POS_MSEC) ) << "}";
                string frameStoragePath(rootShotPath);
                stringstream framestream;

                framestream << frame_number <<".jpg";
                frameStoragePath.append("frame_").append(framestream.str());
                imwrite(frameStoragePath, prevFrame);
                //shot is already saved, so clear frame counter
                frameCounter = 0;
            }
            break;
        }
        bool result = shotBoundaryDetect(prevFrame, grabbedFrame);

        if(shotFoundAtPrev && !shotStartStored)
        {
            int frame_number = (int) cap.get(CV_CAP_PROP_POS_FRAMES);
            fstorage << "{:"<< "begin_frame_number" <<frame_number << "begin_time" << miliseconds_to_DHMS( cap.get(CV_CAP_PROP_POS_MSEC) ) ;
            shotStartStored = true;

            string frameStoragePath(rootShotPath);
            stringstream framestream;

            framestream << frame_number <<".jpg";
            frameStoragePath.append("frame_").append(framestream.str());
            imwrite(frameStoragePath, grabbedFrame);
            //shot is already saved, so clear frame counter
            frameCounter = 0;
        }

        if(result){
            if(!shotFoundAtPrev)
            {
                int frame_number = (int) cap.get(CV_CAP_PROP_POS_FRAMES);
                fstorage << "end_frame_number" <<frame_number << "end_time" << miliseconds_to_DHMS( cap.get(CV_CAP_PROP_POS_MSEC) ) << "}";
                shotStartStored = false;

                string frameStoragePath(rootShotPath);
                stringstream framestream;

                framestream << frame_number <<".jpg";
                frameStoragePath.append("frame_").append(framestream.str());
                imwrite(frameStoragePath, grabbedFrame);
                //shot is already saved, so clear frame counter
                frameCounter = 0;

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

        if(this->sample_period != 0 && frameCounter == this->sample_period){
            int frame_number = (int) cap.get(CV_CAP_PROP_POS_FRAMES);
            fstorage << "frame_number" <<frame_number << "time" << miliseconds_to_DHMS( cap.get(CV_CAP_PROP_POS_MSEC) );
            string frameStoragePath(rootShotPath);
            stringstream framestream;
            framestream << frame_number <<".jpg";
            frameStoragePath.append("frame_").append(framestream.str());
            imwrite(frameStoragePath, grabbedFrame);

            //clear frame counter
            frameCounter = 0;
        }
        frameCounter++;

        prevFrame = grabbedFrame.clone();
        imshow("Video", originalFrame);

        char key = waitKey(30);
        if(key == 'q' || key == 'Q')
        {

            /* if end of shot is stored at this time, do not add closing curly bracket */
            if(shotFoundAtPrev && !shotStartStored)
            {
                int frame_number = (int) cap.get(CV_CAP_PROP_POS_FRAMES);
                fstorage << "aborted_frame_number" <<frame_number << "time" << miliseconds_to_DHMS( cap.get(CV_CAP_PROP_POS_MSEC) );
            }
            /* if end of shot is NOT stored, add closing curly bracket at the end */
            else
            {
                int frame_number = (int) cap.get(CV_CAP_PROP_POS_FRAMES);
                fstorage << "aborted_frame_number" <<frame_number << "time" << miliseconds_to_DHMS( cap.get(CV_CAP_PROP_POS_MSEC) );
                fstorage << "}" ;
            }
            break;
        }
    }
    fstorage << "]" ;
    fstorage.release();
}


/**
 * @brief ShotDetector::processVideo_NoGUI: This method process video and detect shot boundaries
 * at video without graphical interface. Results are stored in a file.
 * @param outputFileName: Results are stored in given filename
 * @param format: Format type of output file. It can be XML, YAML or TEXT (basic txt file format)
 */
void ShotDetector::processVideo_NoGUI(std::string outputFileName, OutputFormat format){
    VideoCapture cap(videoPath);
    stringstream ss;
    ss << outputFileName;

    //create directory if not exists
    string create_dir_command("mkdir -p ");
    create_dir_command += outputFileName;
    system(create_dir_command.c_str());

    if(format == XML){
        ss << ".xml";
    }else if(format == YAML){
        ss << ".xml";
    }else if(format == TEXT){
        ss << ".txt";
    }else{
        //should never been reached
    }

    //store the result in xml format
    FileStorage fstorage(ss.str(), FileStorage::WRITE);
    /** store the information whether shot is found at previous frame in order to detect fades and dissolves  **/
    bool shotFoundAtPrev = false;
    bool shotStartStored = false;

    if(!cap.isOpened()){
        cout<<"error openning video!!" << endl;
        return;
    }
    Mat prevFrame;
    MatND prevHist;
    int channels[] = {0, 1, 2};

    //histogram size (bins) for r,g,b channels equal to 32.
    int histSize[] = {32, 32, 32};
    // color ranges for rgb channels are between (0, 256)
    float color_ranges[] = { 0, 256 };
    const float* ranges[] = { color_ranges, color_ranges, color_ranges };


    cap >> prevFrame;
    prevHist = prepareFrame(prevFrame);

    fstorage << "Header" << "[" ;
    fstorage <<"{:"
            << "video_path" << videoPath
            << "fps" << (int) cap.get(CV_CAP_PROP_FPS)
            <<"frame_count" << (int) cap.get(CV_CAP_PROP_FRAME_COUNT) << "}" << "]" ;

    fstorage << "Shots" << "[" ;
    fstorage << "{:"<< "begin_frame_number" <<(int) cap.get(CV_CAP_PROP_POS_FRAMES) << "begin_time" << miliseconds_to_DHMS( cap.get(CV_CAP_PROP_POS_MSEC) ) ;
    shotStartStored = true;

    //store the shot frame to output path
#ifdef _WIN32
    string rootShotPath(outputFileName);
    if(outputFileName.at(outputFileName.size() -1) != '\\')
        rootShotPath.append("\\");
#else
    string rootShotPath(outputFileName);
    if(outputFileName.at(outputFileName.size() -1) != '/')
        rootShotPath.append("/");
#endif

    //save the inital frame which is the start of first shot.
    string initialShotPath(rootShotPath);
    stringstream filestream;
    filestream << "frame_" << cap.get(CV_CAP_PROP_POS_FRAMES) <<".jpg";
    initialShotPath.append(filestream.str());
    imwrite(initialShotPath, prevFrame);
    int frameCounter = 0;

    while(1){
        Mat grabbedFrame;
        cap >> grabbedFrame;

        if(grabbedFrame.empty()){
            cout<<"empty frame!" << endl;
            if(!shotFoundAtPrev)
            {
                int frame_number = (int) cap.get(CV_CAP_PROP_POS_FRAMES);
                fstorage << "end_frame_number" << frame_number<< "end_time" << miliseconds_to_DHMS( cap.get(CV_CAP_PROP_POS_MSEC) ) << "}";
                string frameStoragePath(rootShotPath);
                stringstream framestream;

                framestream << frame_number <<".jpg";
                frameStoragePath.append("frame_").append(framestream.str());
                imwrite(frameStoragePath, prevFrame);
                //shot is already saved, so clear frame counter
                frameCounter = 0;
            }
            break;
        }

        if(shotFoundAtPrev && !shotStartStored)
        {
            int frame_number = (int) cap.get(CV_CAP_PROP_POS_FRAMES);
            fstorage << "{:"<< "begin_frame_number" <<frame_number << "begin_time" << miliseconds_to_DHMS( cap.get(CV_CAP_PROP_POS_MSEC) ) ;
            shotStartStored = true;
            string frameStoragePath(rootShotPath);
            stringstream framestream;

            framestream << frame_number <<".jpg";
            frameStoragePath.append("frame_").append(framestream.str());
            imwrite(frameStoragePath, grabbedFrame);
            //shot is already saved, so clear frame counter
            frameCounter = 0;
        }
        MatND grabbedHist = prepareFrame(grabbedFrame);
        bool result = shotBoundaryDetectHist(prevHist, grabbedHist);
        if(result)
        {
            if(!shotFoundAtPrev)
            {
                int frame_number = (int) cap.get(CV_CAP_PROP_POS_FRAMES);
                fstorage << "end_frame_number" <<frame_number << "end_time" << miliseconds_to_DHMS( cap.get(CV_CAP_PROP_POS_MSEC) ) << "}";
                shotStartStored = false;
                string frameStoragePath(rootShotPath);
                stringstream framestream;

                framestream << frame_number <<".jpg";
                frameStoragePath.append("frame_").append(framestream.str());
                imwrite(frameStoragePath, grabbedFrame);
                //shot is already saved, so clear frame counter
                frameCounter = 0;
            }
            shotFoundAtPrev = true;
        }
        else
        {
            shotFoundAtPrev = false;
        }

        if(this->sample_period != 0 && frameCounter == this->sample_period){
            int frame_number = (int) cap.get(CV_CAP_PROP_POS_FRAMES);
            fstorage << "frame_number" <<frame_number << "time" << miliseconds_to_DHMS( cap.get(CV_CAP_PROP_POS_MSEC) );
            string frameStoragePath(rootShotPath);
            stringstream framestream;

            framestream << frame_number <<".jpg";
            frameStoragePath.append("frame_").append(framestream.str());
            imwrite(frameStoragePath, grabbedFrame);
            //clear frame counter
            frameCounter = 0;
        }
        frameCounter++;

        prevFrame = grabbedFrame.clone();
        prevHist = grabbedHist.clone();

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
    unsigned int timestamp_as_seconds = duration / 1000;
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

/**
 * @brief ShotDetector::getShotFromVideo: gets detected shot (specified frame with given frame number) from video.
 * @param frame_number: index of frame to be grabbed. Note that indexing is 0 based.
 * @return returns frame with corresponding frame number from video.
 */
cv::Mat ShotDetector::getShotFromVideo(double frame_number){
    VideoCapture cap(videoPath);
    if(!cap.isOpened()){
        cout << "error openning video!" <<endl;
        return Mat();
    }
    cap.set(CV_CAP_PROP_POS_FRAMES, frame_number);
    cv::Mat frame;
    cap >> frame;
    return frame;
}

double cv::compareHistCustom( InputArray _H1, InputArray _H2, int method )
{
    Mat H1 = _H1.getMat(), H2 = _H2.getMat();
    const Mat* arrays[] = {&H1, &H2, 0};
    Mat planes[2];
    NAryMatIterator it(arrays, planes);
    double result = 0;
    int j, len = (int)it.size;

    CV_Assert( H1.type() == H2.type() && H1.type() == CV_32F );

    double s1 = 0, s2 = 0, s11 = 0, s12 = 0, s22 = 0;

    CV_Assert( it.planes[0].isContinuous() && it.planes[1].isContinuous() );

    for( size_t i = 0; i < it.nplanes; i++, ++it )
    {
        const float* h1 = (const float*)it.planes[0].data;
        const float* h2 = (const float*)it.planes[1].data;
        len = it.planes[0].rows*it.planes[0].cols;

        if( method == CV_COMP_CHISQR )
        {
            for( j = 0; j < len; j++ )
            {
                double a = h1[j] - h2[j];
                double b = h1[j] + h2[j];
                if( fabs(b) > FLT_EPSILON )
                    result += a*a/b;
            }
        }
        else if( method == CV_COMP_CORREL )
        {
            for( j = 0; j < len; j++ )
            {
                double a = h1[j];
                double b = h2[j];

                s12 += a*b;
                s1 += a;
                s11 += a*a;
                s2 += b;
                s22 += b*b;
            }
        }
        else if( method == CV_COMP_INTERSECT )
        {
            for( j = 0; j < len; j++ )
                result += std::min(h1[j], h2[j]);
        }
        else if( method == CV_COMP_BHATTACHARYYA )
        {
            for( j = 0; j < len; j++ )
            {
                double a = h1[j];
                double b = h2[j];
                result += std::sqrt(a*b);
                s1 += a;
                s2 += b;
            }
        }
        else
            CV_Error( CV_StsBadArg, "Unknown comparison method" );
    }

    if( method == CV_COMP_CORREL )
    {
        size_t total = H1.total();
        double scale = 1./total;
        double num = s12 - s1*s2*scale;
        double denom2 = (s11 - s1*s1*scale)*(s22 - s2*s2*scale);
        result = std::abs(denom2) > DBL_EPSILON ? num/std::sqrt(denom2) : 1.;
    }
    else if( method == CV_COMP_BHATTACHARYYA )
    {
        s1 *= s2;
        s1 = fabs(s1) > FLT_EPSILON ? 1./std::sqrt(s1) : 1.;
        result = std::sqrt(std::max(1. - result*s1, 0.));
    }

    return result;
}


