#include <iostream>
#include <string>
#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;

namespace e06_guibuttons
{
  Mat image;
  bool applyGray = false;
  bool applyBlur = false;
  bool applySobel = false;

  static void applyFilters() {
    Mat result;
    image.copyTo(result);
    if (applyGray) { cvtColor(result, result, COLOR_BGR2GRAY); }
    if (applyBlur) { blur(result, result, Size(8, 8)); }
    if (applySobel) { Sobel(result, result, CV_8U, 1, 1); }
    imshow("Lena", result);
  }

  static void grayCallback(int state, void*) {
    applyGray = true;
    applyFilters();
  }

  static void rgbCallback(int state, void*) {
    applyGray = false;
    applyFilters();
  }

  static void blurCallback(int state, void*) {
    applyBlur = bool(state);
    applyFilters();
  }

  static void sobelCallback(int state, void*) {
    applySobel= !applySobel;
    applyFilters();
  }

  // Example execution
  static int execute(int argc, char** argv) {
    image = imread("../resources/lena.bmp");
    
    namedWindow("Lena");
    createButton("Gray", grayCallback, NULL, QT_RADIOBOX, false);
    createButton("RBG", rgbCallback, NULL, QT_RADIOBOX, true);
    createButton("Blur", blurCallback, NULL, QT_CHECKBOX, false);
    createButton("Sobel", sobelCallback, NULL, QT_PUSH_BUTTON, false);

    waitKey(0);

    destroyWindow("Lena");

    return 0;
  }
}