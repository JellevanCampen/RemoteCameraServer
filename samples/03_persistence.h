#include <opencv2/opencv.hpp>
using namespace cv;

namespace e03_persistence
{
  static int execute(int argc, char** argv) {
    
    // Write
    FileStorage fs("../resources/test.yml", FileStorage::WRITE);

    int fps = 5;
    fs << "fps" << fps;

    Mat m1 = Mat::eye(2, 3, CV_32F);
    Mat m2 = Mat::ones(3, 2, CV_32F);
    Mat result = (m1 + 1).mul(m1 + 3);
    fs << "result" << result;
    fs.release();

    // Read
    int fps_get;
    Mat result_get;

    FileStorage fs2("../resources/test.yml", FileStorage::READ);
    fs2["fps"] >> fps_get;
    fs2["result"] >> result_get;

    std::cout << "fps: " << fps_get << "\nresult: \n" << result_get << std::endl;
    fs2.release();

    return 0;
  }
}