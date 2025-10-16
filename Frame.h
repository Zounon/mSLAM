#ifndef FRAME_H
#define FRAME_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <sophus/se3.hpp>
#include <vector>
#include <iostream>
#include <mutex>
#include <unordered_map>
#include <memory>

class Feature;


class Frame
{
public:
    cv::Mat img;
    cv::Mat bw;
    std::mutex mutex;
    std::vector<std::shared_ptr<Feature>> features_;
    cv::Mat descriptors; /*of all features found in this img*/
    bool is_valid = false;

    Frame(cv::VideoCapture& cap);

    bool isKeyFrame() {
        return is_keyframe_;
    }

    void setKeyFrame() {
        is_keyframe_ = true;
    }


// private:
    bool is_keyframe_ = false;
    unsigned long id;
	Sophus::SE3<double> pose = Sophus::SE3<double>();

    std::unordered_map<unsigned long, std::shared_ptr<Feature>> tracked_features_; // only when Frame is keyframe;

	Sophus::SE3<double> getPose();
    void setPose(Sophus::SE3<double> new_pose);
};

#endif // FRAME_H
