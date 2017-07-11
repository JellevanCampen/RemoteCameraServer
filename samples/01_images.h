#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;

namespace e01_images
{
  static int execute(int argc, char** argv) {
    Mat image;
    image = imread("../resources/lena.bmp", CV_LOAD_IMAGE_COLOR);
    if (!image.data)
    {
      printf("No image data \n");
      return -1;
    }
    Mat edges;
    Canny(image, edges, 10, 100);

    namedWindow("Input image", WINDOW_AUTOSIZE);
    imshow("Input image", image);

    namedWindow("Edge detection", WINDOW_AUTOSIZE);
    imshow("Edge detection", edges);

    imwrite("../resources/edges.png", edges);

    Vec3b color = image.at<Vec3b>(16, 16);
    std::cout << "Color at [16,16]: " << int(color[0]) << ", " << int(color[1]) << ", " << int(color[2]) << std::endl;

    waitKey(0);
    return 0;
  }
}