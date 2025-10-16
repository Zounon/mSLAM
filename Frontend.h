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
	int num_features = 100;
	int min_distance /*btwn features*/ = 20; // TODO: min dist should be a fn of input img;
	//int frame_id = 0;
	std::shared_ptr<Frame> last_keyframe = nullptr;

	cv::Ptr<cv::GFTTDetector> gftt = cv::GFTTDetector::create(num_features, 0.01, 20);

	bool addFrame(std::shared_ptr<Frame> frame);
	void setKeyframe();

	void trackFeatures();

	void extractNewFeatures(int min_features, int max_total_features);

    void triangulatePoints();

    bool estimatePose();

    std::shared_ptr<Frame> getCurrFrame() const { return curr_frame_; }
	std::shared_ptr<Frame> getPrevFrame() const { return prev_frame_; }

private:
	std::shared_ptr<Frame> prev_frame_ = nullptr;
	std::shared_ptr<Frame> curr_frame_ = nullptr;
	std::shared_ptr<Viewer> viewer_ = nullptr;

	Sophus::SE3<double> relative_motion_;

	cv::Mat K = (cv::Mat_<double>(3, 3) <<
		718.856, 0, 607.1928,
		0, 718.856, 185.2157,
		0, 0, 1);
	//cv::Mat<float> K = { 718.856, 0, 607.1928,
	//	0, 718.856, 185.2157,
	//	0, 0, 1 };
};

#endif // FRONTEND_H