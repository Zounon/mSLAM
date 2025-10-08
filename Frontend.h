#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>

#include "Frame.h"
#include "Viewer.h"

class Frontend
{
public:
	int num_features = 100;
	int min_distance /*btwn features*/ = 20; // TODO: min dist should be a fn of input img;
	//int frame_id = 0;

	cv::Ptr<cv::GFTTDetector> gftt = cv::GFTTDetector::create(num_features, 0.01, 20);

	bool addFrame(std::shared_ptr<Frame> frame);

	void trackFeatures();

	void extractNewFeatures(int min_features, int max_total_features);

	bool estimatePose();

	std::shared_ptr<Frame> getCurrFrame() const { return curr_frame_; }
	std::shared_ptr<Frame> getPrevFrame() const { return curr_frame_; }

private:
	std::shared_ptr<Frame> prev_frame_ = nullptr;
	std::shared_ptr<Frame> curr_frame_ = nullptr;
	std::shared_ptr<Viewer> viewer_ = nullptr;
};