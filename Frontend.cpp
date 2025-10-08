#include "Frontend.h"
#include "Frame.h"

#include <print>



bool Frontend::addFrame(std::shared_ptr<Frame> frame)
{
	if (!frame->is_valid) {
		std::cerr << "Invalid frame passed to Frontend::addFrame" << std::endl;
		return false;
	}

	prev_frame_ = curr_frame_;
	curr_frame_ = frame;

	return true;
}

void Frontend::trackFeatures()
{
	if (prev_frame_->keypts.size() < 10) {
		std::cerr << "Too few keypts to track" << std::endl;
		return;
	}

	//if (curr_frame_->keypts.empty()) {
	//	curr_frame_->keypts = prev_frame_->keypts;
	//}

	std::vector<uchar> status;
	std::vector<float> err;
	//std::vector<cv:Point2f> back_tracked;




	cv::calcOpticalFlowPyrLK(
		prev_frame_->bw, curr_frame_->bw,
		prev_frame_->keypts, curr_frame_->keypts,
		status, err,
		cv::Size(11, 11), 3,
		cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, 0.01),
		0, 1e-4);
		//cv::OPTFLOW_USE_INITIAL_FLOW);

	std::vector<cv::Point2f> prev_keypts_filtered, curr_keypts_filtered;
	for (size_t i = 0; i < status.size(); ++i) {
		//float fb_err = cv::norm(prev_frame_->keypts[i] - back_tracked[i]);
		if (status[i] == 1 && err[i] < 12.0f) {
			prev_keypts_filtered.push_back(prev_frame_->keypts[i]);
			curr_keypts_filtered.push_back(curr_frame_->keypts[i]);
		}
	}
	prev_frame_->keypts = std::move(prev_keypts_filtered);
	curr_frame_->keypts = std::move(curr_keypts_filtered);
}

void Frontend::extractNewFeatures(int min_features, int max_total_features)
{
	if (!curr_frame_) {
		std::cerr << "No current frame available for feature extraction" << std::endl;
		return;
	}

	//if (curr_frame_->keypts.size() < min_features) {
		std::vector<cv::Point2f> new_keypts;
		int features_to_extract = max_total_features - (int)curr_frame_->keypts.size();

		cv::goodFeaturesToTrack(curr_frame_->bw, new_keypts, features_to_extract, 0.01, 100.0);

		std::print("Added {} new keypts\n", new_keypts.size());
		curr_frame_->keypts.insert(curr_frame_->keypts.end(), new_keypts.begin(), new_keypts.end());
	//}
}

bool Frontend::estimatePose()
{
	return false;
}