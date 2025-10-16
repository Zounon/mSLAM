#include "Frame.h"
#include <print>
#include <mutex>

#include <sophus/se3.hpp>
#include <sophus/so3.hpp>

Frame::Frame(cv::VideoCapture& cap)
{
	if (!cap.read(img) || img.empty()) {
		std::cerr << "Failed to load img from cap device" << std::endl;
		return;
	};

	img = img(cv::Range(img.size().height * 2 / 10, img.size().height * 6 / 10), cv::Range(0, img.size().width)).clone();
	cv::extractChannel(img, bw, 1);
	is_valid = true;
}


Sophus::SE3<double> Frame::getPose()
{
	std::lock_guard<std::mutex> lock(mutex);
	return pose;
}

void Frame::setPose(Sophus::SE3<double> new_pose)
{
	std::lock_guard<std::mutex> lock(mutex);
	pose = new_pose;
}

