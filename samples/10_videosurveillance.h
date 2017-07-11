#include <stdio.h>
#include <opencv2/core/core.hpp>

using namespace cv;

namespace e10_videosurveillance
{

  static int execute(int argc, char** argv) {

    VideoCapture video("../resources/video.avi");

    Mat frame;
    Mat frame_previous;
    Mat analysis;
    Mat analysis_1c;

    namedWindow("Video surveillance");
    namedWindow("Analysis");

    video >> frame;

    while (true) {

      frame.copyTo(frame_previous);
      video >> frame;
      imshow("Video surveillance", frame);

      // Frame differencing
      absdiff(frame, frame_previous, analysis);
      cvtColor(analysis, analysis_1c, CV_RGB2GRAY);
      threshold(analysis_1c, analysis_1c, 48, 255, THRESH_BINARY);
      
      // Morphological processing
      Mat element = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
      dilate(analysis_1c, analysis_1c, element);
      erode(analysis_1c, analysis_1c, element);

      imshow("Analysis", analysis_1c);

      if (waitKey(1000 / 7) == 'q') { break; }
    }

    

    return 0;
  }
}