#include <print>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <ranges>
#include <chrono>
#include <thread>
#include <pangolin/pangolin.h>
#include "Frame.h"
#include "Frontend.h"
#include "Viewer.h"


int main()
{

	cv::VideoCapture cap("C:\\Users\\mm\\source\\repos\\mSLAM\\media\\slow-snowcoverd-road.MP4");
	if (!cap.isOpened()) {
		std::cerr << "Error: can't open video file" << std::endl;
		return -1;
	}

	///////////////// PRE MAIN INIT ////////////////////////////
	Frontend frontend;
	Viewer viewer;
	Frame prev = Frame(cap);
	cv::goodFeaturesToTrack(prev.bw, prev.keypts, 200, 0.01, 100.0);

	while (true) {
		auto clock_start = std::chrono::high_resolution_clock::now();

		Frame curr = Frame(cap);
		if (!curr.is_valid) {
			std::cerr << "Error: can't grab frame from cap device" << std::endl;
			break;
		}

		///////////////// FEATURE EXTRACTION ////////////////////////////
		frontend.trackFeatures(prev.bw, curr.bw, prev.keypts, curr.keypts);


		if (curr.keypts.size() < 50) {
			std::vector<cv::Point2f> new_keypts;
			cv::goodFeaturesToTrack(curr.bw, new_keypts, 200 - (int)curr.keypts.size(), 0.01, 100.0);
			std::print("Added {} new keypts\n", new_keypts.size());
			curr.keypts.insert(curr.keypts.end(), new_keypts.begin(), new_keypts.end());
		}




		////////////////// VISUALIZATION ////////////////////////////
		for (const auto& pt : curr.keypts) {
			cv::circle(curr.img, pt, 3, cv::Scalar(0, 255, 0), -1);
		}
		for (size_t i = 0; i < prev.keypts.size() && i < curr.keypts.size(); ++i) {
			cv::line(curr.img, prev.keypts[i], curr.keypts[i], cv::Scalar(0, 255, 255), 1);
		}

		///////////////// SEND FRAME TO PANGOLIN VIEWER ////////////////////////////
		viewer.setCurrFrame(std::make_shared<Frame>(curr));

		///////////////// POST MAIN ////////////////////////////
		auto clock_end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = clock_end - clock_start;
		double fps_alpha = 0.03, fps_avg = 100.0;
		double fps = (elapsed.count() > 0.0) ? (1.0 / elapsed.count()) : 0.0;
		fps_avg = fps_alpha * fps + (1.0 - fps_alpha) * fps_avg;

		cv::putText(curr.img, std::format("FPS :{:.1f}", fps_avg),
			{ 100,50 }, cv::FONT_HERSHEY_COMPLEX, 1.0, { 255,0,255 }, 1);

		cv::putText(curr.img, std::format("# feat {}", curr.keypts.size()),
			{ 100,80 }, cv::FONT_HERSHEY_COMPLEX, 1.0, { 255,0,255 }, 1);

		//cv::imshow("display", curr.img);
		//if (cv::waitKey(1) == 27) {
		//	break;
		//}
		prev = std::move(curr);
	}

	cap.release();
	cv::destroyAllWindows();
	return 0;
}