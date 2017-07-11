#include <iostream>
#include <string>
#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;

namespace e04_gui
{
  static int execute(int argc, char** argv) {
    Mat lena = imread("../resources/lena.bmp");
    Mat edges = imread("../resources/edges.png");
    
    namedWindow("Lena", CV_GUI_NORMAL);
    namedWindow("Edges", WINDOW_AUTOSIZE);
    moveWindow("Lena", 10, 10);
    moveWindow("Edges", 520, 10);
    imshow("Lena", lena);
    imshow("Edges", edges);
    resizeWindow("Lena", 512, 512);

    waitKey(0);

    destroyWindow("Lena");
    destroyWindow("Edges");

    for (int i = 0; i < 10; i++)
    {
      std::ostringstream ss;
      ss << "Photo " << i;
      namedWindow(ss.str());
      moveWindow(ss.str(), 20 * i, 20 * i);
      imshow(ss.str(), lena);
    }

    waitKey(0);

    destroyAllWindows();
    return 0;
  }
}