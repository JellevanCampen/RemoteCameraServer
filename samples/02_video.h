#include <stdio.h>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;

namespace e02_video
{
  static int execute(int argc, char** argv) {

    const char* keys =
    {
      "{help h ? usage | | print this message}"
      "{@video | | video file (webcam is used if undefined)}"
    };

    CommandLineParser parser(argc, argv, keys);
    parser.about("Parser v1.0.0");

    if (parser.has("help"))
    {
      parser.printMessage();
      return 0;
    }

    String videoFile = parser.get<String>(0);

    if (!parser.check())
    {
      parser.printErrors();
      return -1;
    }

    VideoCapture cap;
    if (videoFile != "")
    {
      cap.open(videoFile);
    }
    else 
    {
      cap.open(0);
    }

    if (!cap.isOpened()) { return -1; }

    namedWindow("Video", WINDOW_AUTOSIZE);
    while (true)
    {
      Mat frame;
      cap >> frame;
      imshow("Video", frame);
      if (waitKey(1000/60) >= 0) { /*break;*/ } 
    }

    cap.release();

    return 0;
  }
}