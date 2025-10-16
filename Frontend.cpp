#include "Frontend.h"
#include "Frame.h"
#include "Feature.h"
#include "MapPoint.h"
#include <format>
#include <iostream>



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

		// std::print("Added {} new keypts\n", new_keypts.size());
		std::cerr << std::format("Added {} new keypts\n", new_keypts.size());
		curr_frame_->keypts.insert(curr_frame_->keypts.end(), new_keypts.begin(), new_keypts.end());
		if (prev_frame_->features_.empty()) {
			for (cv::Point2f k : curr_frame_->keypts) {
				auto new_feature = std::make_shared<Feature>();
				new_feature->keypoint = cv::KeyPoint(k, 7.0);
				new_feature->frame_ptr = curr_frame_;
				curr_frame_->features_.push_back(new_feature);
			}
		}
	//}
}

void Frontend::trackFeatures()
{
	//if (prev_frame_->features_.empty() || prev_frame_->features_.size() < 10) {
	//	// std::print(stderr, "Too few keypts to track");
	//	std::cerr << "Too few keypts to track" << std::endl;
	//	return;
	//}

	std::vector<cv::Point2f> prev_pts, curr_pts;
	for (auto& feat: prev_frame_->features_) {
		prev_pts.push_back(feat->keypoint.pt);
	}

	curr_pts = prev_pts;
	
	if (curr_pts.empty()) {
		// std::print(stderr, "List of feature points is empty");
		std::cerr << "List of feature points is empty" << std::endl;
		return;
	}
	

	std::vector<uchar> status;
	std::vector<float> err;

	cv::calcOpticalFlowPyrLK(
		prev_frame_->bw, curr_frame_->bw,
		prev_pts, curr_pts,
		status, err,
		cv::Size(21, 21), 3,
		cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, 0.01),
		cv::OPTFLOW_USE_INITIAL_FLOW);
		//0, 1e-4);

	std::vector<uchar> back_status;
	std::vector<float> back_err;
	std::vector<cv::Point2f> back_tracked;

	cv::calcOpticalFlowPyrLK(
		curr_frame_->bw, prev_frame_->bw,
		curr_pts, back_tracked,
		back_status, back_err,
		cv::Size(21, 21), 3,
		cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, 0.01));

	std::vector<cv::Point2f> prev_keypts_filtered, curr_keypts_filtered;
	const float max_fb_error = 1.0f;
	const float max_flow_error = 8.0f;

	for (size_t i = 0; i < status.size(); ++i) {
		if (status[i] && back_status[i]) {
			float fb_err = cv::norm(prev_pts[i] - back_tracked[i]);
			if (fb_err < max_fb_error && err[i] < max_flow_error) {
				//float displacement = cv::norm(prev_pts[i] - curr_pts[i]);
				//if (displacement < 100.0f) {
					prev_keypts_filtered.push_back(prev_pts[i]);
					curr_keypts_filtered.push_back(curr_pts[i]);
				//}
			}
		}
	}
	prev_frame_->keypts = std::move(prev_keypts_filtered);
	curr_frame_->keypts = std::move(curr_keypts_filtered);


	for (size_t i = 0; i < status.size(); ++i) {
		if (!status[i]) continue;

		auto new_feature = std::make_shared<Feature>();
		new_feature->keypoint.pt = curr_pts[i];
		new_feature->frame_ptr = curr_frame_;

		auto& prev_feat = prev_frame_->features_[i];
		if (prev_feat->map_point_ptr) {
			new_feature->map_point_ptr = prev_feat->map_point_ptr;
			new_feature->map_point_ptr->addObservation(new_feature);
		} else {
			auto map_point = std::make_shared<MapPoint>();
			new_feature->map_point_ptr = map_point;
			prev_feat->map_point_ptr = map_point;
			map_point->addObservation(prev_feat);
			map_point->addObservation(new_feature);
		}

		curr_frame_->features_.push_back(new_feature);
	}
}


void Frontend::triangulatePoints()
{
	auto curr_keyframe = curr_frame_;
}

bool Frontend::estimatePose()
{
	if (!prev_frame_ || !curr_frame_) {
		std::cerr << "Need both previous and current frames for pose estimation" << std::endl;
		return false;
	}
	if (prev_frame_->keypts.size() < 8 || curr_frame_->keypts.size() < 8) {
		std::cerr << "Insufficient keypoints for pose estimation (need at least 8)" << std::endl;
		return false;
	}
	//cv::Mat K = (cv::Mat_<double>(3, 3) <<
	//	718.856, 0, 607.1928,
	//	0, 718.856, 185.2157,
	//	0, 0, 1);




	std::vector<uchar> inlier_mask;
	cv::Mat E;
	try {
		 E = cv::findEssentialMat(
			prev_frame_->keypts, curr_frame_->keypts,
			K, cv::RANSAC, 0.999, 1.0, inlier_mask
		);
	} catch (const cv::Exception& e) {
		std::cerr << "Error in findEssentialMat: " << e.what() << std::endl;
		return false;
	}

	if (E.empty()) {
		std::cerr << "Essential matrix computation failed" << std::endl;
		return false;
	}

	cv::Mat R, t;
	int inliers = cv::recoverPose(E, prev_frame_->keypts, curr_frame_->keypts, K, R, t, inlier_mask);

	// std::print("Recovered pose with {} inliers\n", inliers);
	std::cerr << std::format("Recovered pose with {} inliers\n", inliers);



	return false;
}

