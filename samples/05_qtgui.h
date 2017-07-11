#include <iostream>
#include <string>
#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;

namespace e05_qtgui
{
  int blurAmount = 8;

  // Callback for trackbar
  static void onChange(int pos, void* userInput) {
    if (pos <= 0) { return; }
    Mat imageBlurred;
    blur(*(Mat*)userInput, imageBlurred, Size(pos, pos));
    imshow("Lena", imageBlurred);
  }

  // Callback for mouse click
  static void onMouse(int event, int x, int y, int, void* userInput) {
    if (event != EVENT_LBUTTONDOWN) { return; }
    circle(*(Mat*)userInput, Point(x, y), 10, Scalar(0, 255, 0), 3);
    onChange(blurAmount, userInput);
  }

  // Example execution
  static int execute(int argc, char** argv) {
    Mat lena = imread("../resources/lena.bmp");
    
    namedWindow("Lena");
    createTrackbar("Blur", "Lena", &blurAmount, 64, onChange, &lena);
    setMouseCallback("Lena", onMouse, &lena);

    onChange(blurAmount, &lena);

    waitKey(0);

    destroyWindow("Lena");

    return 0;
  }
}