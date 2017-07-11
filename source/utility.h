#pragma once 
#ifndef SERVER_UTILITY_H_
#define SERVER_UTILITY_H_

#include "opencv2\opencv.hpp"
#include <iostream>
#include <string>

namespace avt {

class ServerSettingsParser {
public:
	// Constructor to parse CLI arguments to settings
	ServerSettingsParser(int argc, char** argv) 
		: parse_successful_(false)
		, video_url_("")
		, video_fps_(60) {

		// Configure the OpenCV commandline parser
		const char* cli_about = {
			"AVT - Remote Camera Server v0.0.1\n"
			"----------------\n"
			"Preview a video stream and capture frames to file.\n"
			"- Press 'c' to capture a frame\n"
			"- Press 'q' to quit the capture\n"
		};

		const char* cli_parameters = {
			"{help ? usage | | show help on cli parameters}"
			"{@video | | URL to the video stream}"
			"{fps | 60 | Framerate of the video stream expressed in frames per second}"
			"{w | 800 | Width of the video preview in pixels}"
			"{h | 450 | Height of the video preview in pixels}"
			"{out-dir | out | Output directory for captured frames}"
			"{out-file | capture#fi16 | Output file name}"
			"{out-ext | jpg | Output file extension}"
		};
		cv::CommandLineParser parser(argc, argv, cli_parameters);

		// Show help is requested
		parser.about(cli_about);
		if (parser.has("help")) {
			parser.printMessage();
			return;
		}

		// Parse settings
		video_url_ = parser.get<std::string>(0);
		video_fps_ = (unsigned short int)parser.get<int>("fps");
		video_width_ = (unsigned short int)parser.get<int>("w");
		video_height_ = (unsigned short int)parser.get<int>("h");
		output_dir_ = parser.get<std::string>("out-dir");
		output_file_ = parser.get<std::string>("out-file");
		output_ext_ = parser.get<std::string>("out-ext");

		// Check for errors in parsing
		if (!parser.check()) {
			parser.printErrors();
			return;
		}
		parse_successful_ = true;
	}

	bool IsParseSuccessful() const { return parse_successful_; }
	const std::string& GetVideoURL() const { return video_url_; }
	const unsigned short int& GetVideoFPS() const { return video_fps_; }
	const unsigned short int& GetVideoWidth() const { return video_width_; }
	const unsigned short int& GetVideoHeight() const { return video_height_; }
	const std::string& GetOutputDirectory() const { return output_dir_; }
	const std::string& GetOutputFileName() const { return output_file_; }
	const std::string& GetOutputFileExtension() const { return output_ext_; }
private:
	bool parse_successful_;
	std::string video_url_;
	unsigned short int video_fps_;
	unsigned short int video_width_;
	unsigned short int video_height_;
	std::string output_dir_;
	std::string output_file_;
	std::string output_ext_;
};

enum class LogMessageType {
	STATUS, 
	WARNING, 
	ERROR
};

static void LogMessage(std::string message, LogMessageType type) {
	switch(type) {
	case LogMessageType::STATUS: std::cout << "[STATUS] " << message << std::endl; break;
	case LogMessageType::WARNING: std::cout << "[WARNING] " << message << std::endl; break;
	case LogMessageType::ERROR: std::cout << "[ERROR] " << message << std::endl; break;
	}
}

static void ReplaceAllOccurences(std::string& s, const std::string& match_string, const std::string& replacement_string) {
	size_t pos = 0;
	pos = s.find(match_string, pos);
	while (pos != std::string::npos) {
		s = s.replace(pos, match_string.length(), replacement_string);
		pos = s.find(match_string, pos);
	}
}

} // namespace avt

#endif SERVER_UTILITY_H_