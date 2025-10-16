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
	constexpr int MIN_NUM_FEATURES = 30;

	cv::VideoCapture cap("C:\\Users\\mm\\source\\repos\\mSLAM\\media\\slow-snowcoverd-road.MP4");
	if (!cap.isOpened()) {
		std::cerr << "Error: can't open video file" << std::endl;
		return -1;
	}

	Frontend frontend;
	Viewer viewer;
	frontend.addFrame(std::make_shared<Frame>(cap));
	frontend.getCurrFrame()->setKeyFrame();
	frontend.getCurrFrame()->setPose(Sophus::SE3d());
	frontend.extractNewFeatures();



	while (viewer.isRunning()) {
		auto clock_start = std::chrono::high_resolution_clock::now();

		if (!frontend.addFrame(std::make_shared<Frame>(cap))) {
			viewer.stopRunning();
			break;
		};

		// if (frontend.getCurrFrame()->features_.size() < MIN_NUM_FEATURES) {
			// frontend.getCurrFrame()->setKeyFrame();
			// frontend.last_keyframe = frontend.getCurrFrame();
			frontend.extractNewFeatures();
		// }

		// frontend.trackFeatures(); 
		frontend.estimatePose();
		

        viewer.setFrames(frontend.getPrevFrame(), frontend.getCurrFrame());

		auto clock_end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = clock_end - clock_start;
        viewer.updateFPS(elapsed.count());
	}
	
	cap.release();
	cv::destroyAllWindows();
	return 0;
}