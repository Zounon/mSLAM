#include "Frontend.h"

void Frontend::trackFeatures(cv::Mat& prev_bw, cv::Mat& curr_bw,
	std::vector<cv::Point2f>& prev_keypts, std::vector<cv::Point2f>& curr_keypts)
{
	if (prev_keypts.size() < 10) {
		std::cerr << "Too few keypts to track" << std::endl;
		return;
	}

	std::vector<uchar> status;
	std::vector<float> err;

	cv::calcOpticalFlowPyrLK(prev_bw, curr_bw, prev_keypts, curr_keypts,
		status, err,
		cv::Size(11, 11), 3,
		cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, 0.01),
		0, 1e-4);

	std::vector<cv::Point2f> prev_keypts_filtered, curr_keypts_filtered;
	for (size_t i = 0; i < status.size(); ++i) {
		if (status[i] == 1 && err[i] < 12.0f) {
			prev_keypts_filtered.push_back(prev_keypts[i]);
			curr_keypts_filtered.push_back(curr_keypts[i]);
		}
	}
	prev_keypts = std::move(prev_keypts_filtered);
	curr_keypts = std::move(curr_keypts_filtered);
}

bool Frontend::estimatePose(const std::vector<cv::Point2f>& prev_keypts, const std::vector<cv::Point2f>& curr_keypts)
{
	// TODO: Implementation needed
	return false;
}