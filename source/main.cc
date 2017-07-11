#include <iostream>
#include <sstream>
#include "opencv2\opencv.hpp"
#include "utility.h"

using namespace cv;

int main(int argc, char** argv)
{
	avt::ServerSettingsParser ssp(argc, argv);
	if (!ssp.IsParseSuccessful()) { return -1; }

	Mat image;
	VideoCapture video;
	video.open(ssp.GetVideoURL());
	if (!video.isOpened()) {
		avt::LogMessage("Failed to open the specified video stream.", avt::LogMessageType::ERROR);
		return -1;
	}

	namedWindow("Video stream", WINDOW_NORMAL);
	resizeWindow("Video stream", ssp.GetVideoWidth(), ssp.GetVideoHeight());
	unsigned long long int frame_index = 0;
	while (true) {
		Mat frame;
		video >> frame;
		imshow("Video stream", frame);
		unsigned short key = waitKey(1000 / ssp.GetVideoFPS());
		if (key == 'q') { break; }	// q = quit
		if (key == 'c') {			// c = capture frame
			std::string filename(ssp.GetOutputFileName());

			// Replace #fi8 tags by the framerate as an 8 digit number
			std::stringstream fi8_ss;
			fi8_ss << std::setfill('0') << std::setw(8) << frame_index;
			std::string fi8_string = fi8_ss.str();
			avt::ReplaceAllOccurences(filename, "#fi8", fi8_string);

			// Replace #fi16 tags by the framerate as a 16 digit number
			std::stringstream fi16_ss;
			fi16_ss << std::setfill('0') << std::setw(16) << frame_index;
			std::string fi16_string = fi16_ss.str();
			avt::ReplaceAllOccurences(filename, "#fi16", fi16_string);

			std::stringstream full_filename_ss;
			full_filename_ss << ssp.GetOutputDirectory() << "/" << filename << "." << ssp.GetOutputFileExtension();
			std::string full_filename = full_filename_ss.str();
			imwrite(full_filename, frame);
		}
		frame_index++;
	}
		
	return 0;
}