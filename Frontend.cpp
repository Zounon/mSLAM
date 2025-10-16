#include "Frontend.h"
#include "Frame.h"
#include "Feature.h"
#include "MapPoint.h"
#include <format>
#include <iostream>

#include <opencv2/core/eigen.hpp>


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

void Frontend::extractNewFeatures()
{
	std::vector<cv::KeyPoint> keypoints;
	cv::Mat descriptors;
	detector_->detectAndCompute(
		curr_frame_->bw, 
		cv::noArray(), /*TODO: add mask of curr keypts*/
		keypoints, 
		descriptors
	);
	curr_frame_->descriptors = descriptors;

	// for (const auto& kp : keypoints) {
	for (auto i=0; i < keypoints.size(); ++i) {
		auto feature = std::make_shared<Feature>();
		feature->keypoint = keypoints[i];
		feature->descriptor = descriptors.row(i).clone();
		feature->frame_ptr = curr_frame_;
		// feature->track_id =  // TODO: when needed for Backend

		curr_frame_->features_.push_back(feature);
	}
}

void Frontend::trackFeatures()
{
}

std::vector<cv::DMatch> Frontend::matchFeatures() {
	std::vector<cv::DMatch> matches;
	matcher_->match(prev_frame_->descriptors, curr_frame_->descriptors, matches);

	// Sort by distance (ascending)
	std::sort(matches.begin(), matches.end());
	// TODO: filter matches
	return matches;
}


void Frontend::triangulatePoints()
{
	std::vector<cv::DMatch> matches = matchFeatures();
	cv::Mat P1 = cv::Mat::eye(3, 4, CV_64F);
	P1 = K * P1; 
}

bool Frontend::estimatePose()
{
	// TODO: replace w/ expects
	// if (!prev_frame_ || !curr_frame_) {
	// }
	std::vector<cv::DMatch> matches = matchFeatures();
	std::vector<cv::Point2f> prev_points, curr_points;
	for (auto m : matches) {
		prev_points.push_back(prev_frame_->features_[m.queryIdx]->keypoint.pt);
		curr_points.push_back(curr_frame_->features_[m.trainIdx]->keypoint.pt);
	}

	if (prev_points.size() < 8 || curr_points.size() < 8) {
		std::cerr << "ERROR estimating pose: not enough points" << std::endl;
		return false;
	}

	std::vector<uchar> inlier_mask;
	cv::Mat E = cv::findEssentialMat(
		prev_points, curr_points,
		K, cv::RANSAC, 0.99, 1.0, inlier_mask
	);

	cv::Mat cv_R, cv_t;
	int inliers = cv::recoverPose(
		E, prev_points, curr_points,
		K, cv_R, cv_t, inlier_mask
	);

	Eigen::Matrix3d R;
	Eigen::Vector3d t;

	cv::cv2eigen(cv_R, R);
	cv::cv2eigen(cv_t, t);

	Sophus::SE3d T_prev_to_curr(R, t);

    Sophus::SE3d T_prev = prev_frame_->getPose();
    Sophus::SE3d T_curr = T_prev * T_prev_to_curr;
    
    curr_frame_->setPose(T_curr);

	//std::print("Recovered pose with {} inliers\n", inliers);
    return true;
}

