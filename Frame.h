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
    std::vector<cv::Point2f> keypts;
    std::mutex mutex;
    std::vector<std::shared_ptr<Feature>> features_;
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

// class Frame
// {
// public:
//     cv::Mat img;
//     cv::Mat bw;
//     std::vector<cv::Point2f> pts;
//     bool is_valid = false;

//     Frame() = default;

//     explicit Frame(cv::VideoCapture& cap)
//     {
//         read(cap);
//     }

//     bool read(cv::VideoCapture& cap)
//     {
//         if (!cap.read(img) || img.empty()) {
//             is_valid = false;
//             bw.release();
//             pts.clear();
//             return false;
//         }

//         cv::extractChannel(img, bw, 1);
//         pts.clear();
//         is_valid = true;
//         return true;
//     }

//     [[nodiscard]] bool isValid() const
//     {
//         return is_valid;
//     }
// };

