#include <sstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;

namespace e08_aoi
{
  const char* cli_parameters =
  {
    "{help h ? usage | | show help on parameters}"
    "{@image | | image to process}"
    "{@lightpattern | | image of light pattern in input image}"
    "{lightmethod | 1 | method for background light removal (0 = difference, 1 = division)}"
    "{segmentmethod | 0 | method for segmentation (0 = connected components, 1 = connected components with statistics, 2 = find contours)}"
  };

  static Mat RemoveLightPattern(const Mat& image, const Mat& image_lightpattern, int method) {
    Mat result;
    switch (method) {
    case 0: // Difference
      result = image_lightpattern - image;
      break;
    case 1: // Division
      Mat image_f, image_lightpattern_f;
      image.convertTo(image_f, CV_32FC3);
      image_lightpattern.convertTo(image_lightpattern_f, CV_32FC3);
      result = Scalar(1.f, 1.f, 1.f) - (image_f / image_lightpattern_f);
      result = max(result, 0.0f);
      result = min(result, 1.0f);
      result *= 255.f;
      result.convertTo(result, CV_8UC3);
      break;
    }
    return result;
  }

  static Mat CalculateLightPattern(const Mat& image) {
    Mat lightpattern;
    blur(image, lightpattern, Size(image.cols / 3, image.cols / 3));
    return lightpattern;
  }

  static Mat ConnectedComponents(const Mat& image) {
    Mat labels;
    int num_objects = connectedComponents(image, labels);
    
    Mat result = Mat::zeros(image.rows, image.cols, CV_8UC3);
    RNG rng(0xFFFFFFFF);
    for (int i = 1; i < num_objects; i++) {
      Mat mask = (labels == i);
      result.setTo(Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), mask);
    }
    return result;
  }

  static Mat ConnectedComponentsWithStatistics(const Mat& image) {
    Mat labels, statistics, centroids;
    int num_objects = connectedComponentsWithStats(image, labels, statistics, centroids);
    
    Mat result = Mat::zeros(image.rows, image.cols, CV_8UC3);
    RNG rng(0xFFFFFFFF);
    for (int i = 1; i < num_objects; i++) {
      if (statistics.at<int>(i, CC_STAT_AREA) < 10) { continue; }

      Mat mask = (labels == i);
      result.setTo(Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), mask);
      
      std::stringstream ss;
      ss << "area: " << statistics.at<int>(i, CC_STAT_AREA);
      putText(result, ss.str(), Point(centroids.at<double>(i, 0), centroids.at<double>(i, 1)), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 255, 255));
    }
    return result;
  }

  static Mat FindContours(const Mat& image) {
    std::vector<std::vector<Point>> contours;
    findContours(image, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    Mat result = Mat::zeros(image.rows, image.cols, CV_8UC3);
    RNG rng(0xFFFFFFFF);
    for (int i = 0; i < contours.size(); i++) {
      drawContours(result, contours, i, Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)));
    }
    return result;
  }

  static std::vector<std::vector<float>> ExtractFeatures(const Mat& image, std::vector<int>* pos_x = NULL, std::vector<int>* pos_y = NULL) {
    
    std::vector<std::vector<float>> features;
    std::vector<std::vector<Point>> contours;

    std::vector<Vec4i> hierarchy;
    findContours(image, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
    if (contours.size() == 0) { return features; }

    RNG rng(0xFFFFFFFF);
    for (int i = 0; i < contours.size(); i++) {
      Mat mask = Mat::zeros(image.rows, image.cols, CV_8UC1);
      drawContours(mask, contours, i, Scalar(1), FILLED, LINE_8, hierarchy, 1);
      Scalar area_s = sum(mask);
      float area = area_s[0];

      if (area > 500) {
        RotatedRect rect = minAreaRect(contours[i]);
        float ratio = rect.size.width / rect.size.height;
        ratio = (ratio < 1.0f) ? 1.0f / ratio : ratio;

        std::vector<float> feature_vector;
        feature_vector.push_back(area);
        feature_vector.push_back(ratio);
        features.push_back(feature_vector);
        if (pos_x != NULL) { pos_x->push_back(rect.center.x); }
        if (pos_y != NULL) { pos_y->push_back(rect.center.y); } 
      }
    }

    return features;
  }

  static int execute(int argc, char** argv) {
    CommandLineParser parser(argc, argv, cli_parameters);
    parser.about("AOI v1.0.0");

    // Show help info
    if (parser.has("help")) {
      parser.printMessage();
      return 0;
    }

    // Parse parameters and show errors
    String image_file = parser.get<String>(0);
    String image_lightpattern_file = parser.get<String>(1);
    int method_light = parser.get<int>("lightmethod");
    int method_segment= parser.get<int>("segmentmethod");

    if (!parser.check()) {
      parser.printErrors();
      return 0;
    }

    // Load images
    Mat image = imread(image_file);
    if (image.data == NULL) {
      std::cout << "Error loading image " << image_file << std::endl;
      return -1;
    }

    // Perform AOI
    // -- 1. Remove noise
    medianBlur(image, image, 3);
    imshow("No noise", image);

    // -- 2. Remove light pattern
    Mat image_lightpattern;
    if (image_lightpattern_file == "") { 
      image_lightpattern = CalculateLightPattern(image);
    } else {
      image_lightpattern = imread(image_lightpattern_file);
    }
    Mat image_nolightpattern = RemoveLightPattern(image, image_lightpattern, method_light);
    imshow("No light pattern", image_nolightpattern);

    // -- 3. Threshold the image
    Mat image_thresholded;
    Mat image_thresholded_1c;
    threshold(image_nolightpattern, image_thresholded, 30.f, 255.f, THRESH_BINARY);
    cvtColor(image_thresholded, image_thresholded_1c, CV_RGB2GRAY);
    imshow("Thresholded", image_thresholded);

#ifdef NULL
    // -- 4. Segmentation
    Mat image_segmented;
    switch (method_segment) {
    case 0:
      image_segmented = ConnectedComponents(image_thresholded_1c); break;
    case 1:
      image_segmented = ConnectedComponentsWithStatistics(image_thresholded_1c); break;
    case 2:
      image_segmented = FindContours(image_thresholded_1c); break;
    }
    imshow("Segmented", image_segmented);
#endif

    // -- 4. Feature extraction
    std::vector<std::vector<float>> features;
    std::vector<int> pos_x;
    std::vector<int> pos_y;
    features = ExtractFeatures(image_thresholded_1c, &pos_x, &pos_y);

    waitKey(0);
  }
}