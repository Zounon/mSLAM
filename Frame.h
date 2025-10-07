#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include <iostream>

class Frame
{
public:
    bool is_valid = false;
    cv::Mat img;
    cv::Mat bw;
    std::vector<cv::Point2f> keypts;

    Frame(cv::VideoCapture& cap);
    // Frame(cv::VideoCapture& cap, cv::Mat mask);

private:
};