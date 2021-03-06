#include <iostream>
#include <fstream>                        // File read/write
#include <time.h>

#include "opencv2/opencv_modules.hpp"

// ----- from contours2.cpp
// #include "opencv2/imgproc.hpp"
// #include "opencv2/highgui.hpp"
// #include <math.h>
// #include <iostream>
// -----


#if defined(HAVE_OPENCV_CUDACODEC)

#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

#include "opencv2/cudafilters.hpp"
#include "opencv2/cudaimgproc.hpp"

#include <opencv2/core.hpp>
#include <opencv2/core/opengl.hpp>
#include <opencv2/cudacodec.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/core/utility.hpp"
#include "opencv2/cudabgsegm.hpp"
#include "opencv2/video.hpp"

#include "opencv2/cudaarithm.hpp"


using namespace std;
using namespace cv;
using namespace cv::cuda;

#define FULLIMGSAVEPATH "/tmp/ramdisk/full/"
#define LOGFILE "bgsub.log"

/**
 * g++ -ggdb background_subtraction.cpp -o background_subtraction `pkg-config --cflags --libs opencv4` -I /usr/local/cuda-10.1/include/
 */


/**
 * https://www.linuxbabe.com/command-line/create-ramdisk-linux
 * sudo mkdir /tmp/ramdisk && sudo chmod 777 /tmp/ramdisk && sudo mount -t tmpfs -o size=1024m myramdisk /tmp/ramdisk && mkdir /tmp/ramdisk/full
 */

/**
 *



sudo mkdir -p /tmp/ramdisk \
sudo chmod 777 /tmp/ramdisk \
sudo mount -t tmpfs -o size=4G myramdisk /tmp/ramdisk \
mkdir /tmp/ramdisk/full \
mkdir /tmp/ramdisk/annotated \
mkdir /tmp/ramdisk/annotateddebug


sudo umount /tmp/ramdisk/




*/


/**
 * Get the filename from the path.
 * Borrowed this piece of code from here.
 * https://www.safaribooksonline.com/library/view/c-cookbook/0596007612/ch10s15.html
 *
 */
string getFileName(const string& s) {

   char sep = '/';

#ifdef _WIN32
   sep = '\\';
#endif

   size_t i = s.rfind(sep, s.length());
   if (i != string::npos) {
      return(s.substr(i+1, s.length() - i));
   }

   return(s);
}


/**
 * TODO   Pass 
 *         Input file
 *         Output dir 
 *         Logfilename
 * 
 *    Process all files or every n'th
 *
 */




/**
 * Main code
 */
int main(int argc, const char* argv[])
{
    if (argc != 2) {
        std::cout << "Please specify video source file!" << endl;
        return -1;
    }

    /*
    cout
        << "------------------------------------------------------------------------------" << endl
        << "This program will analyse a video for moving objects."                          << endl
        << "Usage:"                                                                         << endl
        << "./background_subtraction <input_video_name>"                                    << endl
        << "------------------------------------------------------------------------------" << endl
        << endl;
    */

    // TODO add as parameter to CLI
    bool showimg = true;
    bool saveimg = true;

    int minContour = 400;

    const std::string fname(argv[1]);


    cv::namedWindow("Mask", cv::WINDOW_NORMAL);
    cv::namedWindow("GPU", cv::WINDOW_OPENGL);
    cv::namedWindow("CPU", cv::WINDOW_FULLSCREEN);
//    cv::namedWindow("Mask", cv::WINDOW_NORMAL);

    cv::cuda::setGlDevice();

    cv::Mat frame;
    cv::Mat orig_frame;
    cv::Mat fgmask;

    cv::VideoCapture reader(fname);

    cv::cuda::GpuMat d_frame;
    cv::cuda::GpuMat d_fgmask;
    cv::cuda::GpuMat d_bgimg;


    cv::Ptr<cv::cudacodec::VideoReader> d_reader = cv::cudacodec::createVideoReader(fname);

    int bShadowDetection = false;
    int history = 400;
    int varThreshold = 100;

    Ptr<BackgroundSubtractor> mog2 = cuda::createBackgroundSubtractorMOG2(history, varThreshold, bShadowDetection);


    std::vector<double> cpu_times;
    std::vector<double> gpu_times;


    clock_t start, end;
    double elapsed;
    start = clock();
    int counter = 0;
    int framenum = 0;
    int hitnum = 0;

/* DIALATE */
    int erosionDilation_size = 5;// Was 5
    Mat element = cv::getStructuringElement(MORPH_RECT, Size(2*erosionDilation_size + 1,    2*erosionDilation_size+1));

    cv::cuda::GpuMat d_element(element);

    // MORPH_OPEN  MORPH_DILATE MORPH_CLOSE
    Ptr<cuda::Filter> dilateFilter = cuda::createMorphologyFilter(MORPH_CLOSE, d_fgmask.type(), element);

    Mat element2 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(40, 40));
    cv::cuda::GpuMat d_element2(element2);
    Ptr<cuda::Filter> dilateFilter2 = cuda::createMorphologyFilter(MORPH_CLOSE, d_fgmask.type(), element2);


 bool started = false;
 int height = 0;
 int width = 0;
 int maskratio=170;
 int minmask=0;

  cout << "Background subtraction opening file : " << fname << endl;
  string filename = getFileName(fname);
  //cout << filename << endl;


 for (;;)
    {
        framenum++;
        if (!d_reader->nextFrame(d_frame))
            break;

        if (!started) {
            started = true;
            d_frame.download(frame);
            width = frame.size().width;
            height = frame.size().height;
            minmask = (width*height)/maskratio;

            cout << "Width : " << width << endl;
            cout << "Height: " << height << endl;
            cout << "Maskratio: " << maskratio << endl;
            cout << "Minpixels: " << minmask << endl;

	}


        mog2->apply(d_frame, d_fgmask);
//        mog2->getBackgroundImage(d_bgimg);

//        dilateFilter->apply(d_fgmask, d_fgmask);
//        dilateFilter2->apply(d_fgmask, d_fgmask);
        if ( (framenum % 5 )) {

          continue;
	}

        //cv::String skipping = cv::format("Frame %05d",framenum);
        //std::cout << skipping << std::endl;


        int pixels =  cv::cuda::countNonZero(d_fgmask);

	if ((pixels > minmask) && (framenum > 1) ){

            // TODO dirty hack
	    double maxcontour = 0;
	    maxcontour = minContour + 1;


            //dilateFilter2->apply(d_fgmask, d_fgmask);


            counter++;
            d_frame.download(frame);
            //d_fgmask.download(fgmask);
            //d_frame.download(orig_frame);

/*
  This is a major prerformance hit !
  Finding contours in the GPU memory would really speed things up in noisy and shaky videos.

  Digging into the issue :
    https://answers.opencv.org/question/59413/cuda-based-connected-component-labeling/
    https://github.com/opencv/opencv_contrib/blob/master/modules/cudalegacy/src/graphcuts.cpp

*/

/*

            vector<vector<Point> > contours;
            vector<Vec4i> hierarchy;

            double maxcontour = 0;
            findContours( fgmask, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );



            for( int i = 0; i< contours.size(); i++ ) {

                double a=contourArea( contours[i],false);  //  Find the area of contour
                if (a > maxcontour) {
                    maxcontour = a;
		}
            }
*/
            if(saveimg && maxcontour > minContour) {

/*		   
            /// Approximate contours to polygons + get bounding rects and circles
            vector<vector<Point> > contours_poly( contours.size() );
            vector<Rect> boundRect( contours.size() );
            vector<Point2f>center( contours.size() );
            vector<float>radius( contours.size() );

            for( int i = 0; i < contours.size(); i++ ) {
                approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
                minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
            }
*/
	    hitnum++;
            char savename[100] = {0};
            sprintf(savename,"%s%s_%05d.jpg", FULLIMGSAVEPATH, filename.c_str(), framenum);
            imwrite(savename, frame );
            //std::cout << savename << "   Maxcountour in image " << maxcontour << " Maskpixels = " << pixels << std::endl;

    	}



        showimg = false;
        if(showimg && maxcontour > minContour) {

                 resize(frame, frame, Size(frame.cols/3, frame.rows/3));
                 // resize(fgmask, fgmask, Size(fgmask.cols/3, fgmask.rows/3));

		cv::imshow("CPU", frame);
                //cv::imshow("Mask", fgmask);

            if (cv::waitKey(1) > 0)
                break;
        }


      }

    }


    end = clock();
    elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

    double fps=framenum/elapsed;

    fstream f(LOGFILE, f.out | f.app);
    cv::String logText = cv::format("BGSUB processed %5d / %5d frames of size %4dx%-4d maskratio %d in %6.02f seconds %6.01f FPS. File : %s\n" , framenum, hitnum, width, height, maskratio, elapsed, fps, filename.c_str());
    f << logText;

    std::cout << logText;


    return 0;
}



#else

/**
 * Main code in case there is no CUDA support.
 *
 */
int main()
{
    std::cout << "OpenCV was built without CUDA Video decoding support\n" << std::endl;
    std::cout << "g++ -ggdb background_subtraction.cpp -o background_subtraction `pkg-config --cflags --libs opencv4` -I /usr/local/cuda-10.1/include/\n" << std::endl;

    return 0;
}

#endif
