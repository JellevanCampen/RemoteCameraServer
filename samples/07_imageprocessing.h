#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;

namespace e07_imageprocessing
{
  const char* cli_parameters =
  {
    "{help h ? usage | | show help on parameters}"
    "{@image | | image to process}"
  };

  static void ShowHistogram(Mat* image) {
    // Calculate the histograms
    std::vector<Mat> image_split;
    split(*image, image_split);

    int num_bins = 256;
    float channel_range[] { 0, 256 };
    const float* ranges[] { channel_range };

    Mat hist_b, hist_r, hist_g;
    calcHist(&image_split[0], 1, 0, Mat(), hist_b, 1, &num_bins, ranges);
    calcHist(&image_split[1], 1, 0, Mat(), hist_g, 1, &num_bins, ranges);
    calcHist(&image_split[2], 1, 0, Mat(), hist_r, 1, &num_bins, ranges);

    // Draw the histograms
    int width = 512;
    int height = 300;
    int pastel = 24;

    Mat histImage(height, width, CV_8UC3, Scalar(pastel, pastel, pastel));
    normalize(hist_b, hist_b, 0, height, NORM_MINMAX);
    normalize(hist_g, hist_g, 0, height, NORM_MINMAX);
    normalize(hist_r, hist_r, 0, height, NORM_MINMAX);

    int bin_step = cvRound(float(width) / float(num_bins));
    for (int i = 1; i < num_bins; i++) {
      line(histImage,
        Point(bin_step*(i - 1), height - cvRound(hist_b.at<float>(i - 1))),
        Point(bin_step*(i), height - cvRound(hist_b.at<float>(i))),
        Scalar(256 - pastel, pastel, pastel));
      line(histImage,
        Point(bin_step*(i - 1), height - cvRound(hist_g.at<float>(i - 1))),
        Point(bin_step*(i), height - cvRound(hist_g.at<float>(i))),
        Scalar(pastel, 256 - pastel, pastel));
      line(histImage,
        Point(bin_step*(i - 1), height - cvRound(hist_r.at<float>(i - 1))),
        Point(bin_step*(i), height - cvRound(hist_r.at<float>(i))),
        Scalar(pastel, pastel, 256 - pastel));
    }
    imshow("Histogram", histImage);
  }

  static void ApplyHistogramEqualization(Mat* image) {
    Mat image_ycrcb;
    cvtColor(*image, image_ycrcb, COLOR_BGR2YCrCb);
    std::vector<Mat> image_ycrcb_split;
    split(image_ycrcb, image_ycrcb_split);
    equalizeHist(image_ycrcb_split[0], image_ycrcb_split[0]);
    merge(image_ycrcb_split, image_ycrcb);
    Mat result;
    cvtColor(image_ycrcb, result, COLOR_YCrCb2BGR);
    imshow("Histogram Equalization", result);
  };

  static void ApplyLomographyEffect(Mat* image) { 
    // Calculate a lookup table
    const double e = std::exp(1.);
    Mat lut(1, 256, CV_8UC1);
    for (int i = 0; i < 256; i++) {
      float x = float(i) / 256.f;
      lut.at<uchar>(i) = cvRound(256.f * (1 / (1 + pow(e, -(x - 0.5) / 0.1))));
    }

    // Transform the one channel
    unsigned char channel = 2;
    std::vector<Mat> image_split;
    split(*image, image_split);
    LUT(image_split[channel], lut, image_split[channel]);
    Mat result;
    merge(image_split, result);

    // Add a vignette
    Mat vignette(image->rows, image->cols, CV_32FC3, Scalar(0.3, 0.3, 0.3));
    circle(vignette, Size(image->rows / 2, image->cols / 2), image->cols / 3, Scalar(1, 1, 1), -1);
    blur(vignette, vignette, Size(image->cols / 3, image->cols / 3));

    Mat result_float;
    result.convertTo(result_float, CV_32FC3);
    multiply(result_float, vignette, result_float);
    result_float.convertTo(result, CV_8UC3);

    imshow("Lomography", result);
  };

  static void ApplyCartoonizeEffect(Mat* image) { 
    // Create edges image
    Mat image_median;
    medianBlur(*image, image_median, 7);
    Mat image_edges;
    Canny(image_median, image_edges, 50, 150);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(2, 2));
    dilate(image_edges, image_edges, kernel);
    image_edges /= 255;
    image_edges = 1 - image_edges;
    Mat image_edges_f;
    image_edges.convertTo(image_edges_f, CV_32FC3);
    blur(image_edges_f, image_edges_f, Size(3, 3));

    // Cartoonize colors
    Mat image_bilateral;
    bilateralFilter(*image, image_bilateral, 9, 150., 150.);
    Mat result = image_bilateral / 25;
    result *= 25;

    // Merge colours and edges
    Mat image_edges_3channels;
    Mat image_edges_channels[] = { image_edges_f, image_edges_f, image_edges_f };
    merge(image_edges_channels, 3, image_edges_3channels);
    Mat result_float;
    result.convertTo(result_float, CV_32FC3);
    multiply(result_float, image_edges_3channels, result_float);
    result_float.convertTo(result, CV_8UC3);
    imshow("Cartoonize", result);
  };

  static int execute(int argc, char** argv) {
    CommandLineParser parser(argc, argv, cli_parameters);
    parser.about("Image processing v1.0.0");

    // Show help info
    if (parser.has("help")) {
      parser.printMessage();
      return 0;
    }

    // Parse parameters and show errors
    String img_file = parser.get<String>(0);
    if (!parser.check()) {
      parser.printErrors();
      return 0;
    }

    // Show the main window
    Mat img = imread(img_file);
    namedWindow("Main");
    imshow("Main", img);

    int key;
    while (key = waitKey(0)) {
      if (key == 'h') { ShowHistogram(&img); }
      if (key == 'e') { ApplyHistogramEqualization(&img); }
      if (key == 'l') { ApplyLomographyEffect(&img); }
      if (key == 'c') { ApplyCartoonizeEffect(&img); }
      if (key == 'q') { return 0; }
    }
  }
}