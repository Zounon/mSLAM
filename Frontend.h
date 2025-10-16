#ifndef FRONTEND_H
#define FRONTEND_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include <sophus/se3.hpp>

#include "Frame.h"
#include "Viewer.h"

class Frontend
{
public:
	// int num_features = 100;
	int min_distance /*btwn features*/ = 20; // TODO: min dist should be a fn of input img;
	//int frame_id = 0;
	std::shared_ptr<Frame> last_keyframe = nullptr;

	bool addFrame(std::shared_ptr<Frame> frame);
	void setKeyframe();
	void trackFeatures();
	std::vector<cv::DMatch> matchFeatures();
	void extractNewFeatures();
    void triangulatePoints();
    bool estimatePose();

    std::shared_ptr<Frame> getCurrFrame() const { return curr_frame_; }
	std::shared_ptr<Frame> getPrevFrame() const { return prev_frame_; }

	// TODO: descover true camera intrinsics
	double fx = 401.0; 
	double fy = 401.0; 
	double cx = 960.0; 
	double cy = 540.0;

	cv::Mat K = (cv::Mat_<double>(3,3) <<
		fx, 0,  cx,
		0, fy, cy,
		0, 0,  1);

private:
	cv::Ptr<cv::ORB> detector_ = cv::ORB::create(50, 1.2f, 4);
	cv::Ptr<cv::DescriptorMatcher> matcher_ = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE_HAMMING);
	std::shared_ptr<Frame> prev_frame_ = nullptr;
	std::shared_ptr<Frame> curr_frame_ = nullptr;
	std::shared_ptr<Viewer> viewer_ = nullptr;
};

#endif // FRONTEND_H