#include <stdio.h>
#include <opencv2/core/core.hpp>
using namespace cv;

namespace e09_facedetection
{

  static int execute(int argc, char** argv) {
    
    // Read and preprocess image
    Mat image = imread("../resources/face.jpeg");
    Mat image_1c;
    cvtColor(image, image_1c, CV_RGB2GRAY);
    equalizeHist(image_1c, image_1c);

    // Detect faces
    std::vector<Rect> faces;
    CascadeClassifier face;
    face.load("../resources/haarcascade_frontalface_default.xml");
    face.detectMultiScale(image_1c, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

    // Draw detected faces
    for (int i = 0; i < faces.size(); i++) {
      Rect faceRect(faces[i].x, faces[i].y, faces[i].width, faces[i].height);
      rectangle(image, faceRect, Scalar(0, 255, 0), 2);
    }

    // Detect eyes
    std::vector<Rect> eyes;
    CascadeClassifier eye;
    eye.load("../resources/haarcascade_eye.xml");
    eye.detectMultiScale(image_1c, eyes, 1.1, 10, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

    // Draw detected eyes
    for (int i = 0; i < eyes.size(); i++) {
      Rect eyeRect(eyes[i].x, eyes[i].y, eyes[i].width, eyes[i].height);
      rectangle(image, eyeRect, Scalar(0, 0, 255), 2);
    }

    imshow("Face detection", image);

    waitKey(0);

    return 0;
  }
}