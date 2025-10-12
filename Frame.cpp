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
	is_valid = true;

	img = img(cv::Range(img.size().height * 2 / 10, img.size().height * 6 / 10), cv::Range(0, img.size().width)).clone();

	//int height = img.size().height;
	//int width = img.size().width;
	//cv::Mat mask = cv::Mat::zeros(img.size(), CV_8UC1);
	//cv::Rect mask_shape(width / 10, height / 10, width * 8 / 10, height * 6 / 10);
	//cv::rectangle(mask, mask_shape, cv::Scalar(255), -1);
	//img.setTo(cv::Scalar(0, 0, 20), ~mask);

	cv::extractChannel(img, bw, 1);
	//std::print("Frame size: {} {} \n", img.size().width, img.size().height);
	//std::print("BW Frame size: {} {}\n", bw.size().width, bw.size().height);
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

