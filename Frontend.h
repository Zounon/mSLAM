#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include <iostream>

class Frontend
{
public:
	int num_features = 100;
	int min_distance /*btwn features*/ = 20; // TODO: min dist should be a fn of input img;
	//int frame_id = 0;

	cv::Ptr<cv::GFTTDetector> gftt = cv::GFTTDetector::create(num_features, 0.01, 20);

	void trackFeatures(cv::Mat& prev_bw, cv::Mat& curr_bw,
		std::vector<cv::Point2f>& prev_keypts, std::vector<cv::Point2f>& curr_keypts);

	bool estimatePose(const std::vector<cv::Point2f>& prev_keypts, const std::vector<cv::Point2f>& curr_keypts);
};