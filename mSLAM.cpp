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
	frontend.addFrame(std::make_shared<Frame>(cap));
	frontend.getCurrFrame()->setKeyFrame();

	cv::goodFeaturesToTrack(
		frontend.getCurrFrame()->bw,
		frontend.getCurrFrame()->keypts,
		200, 0.01, 100.0);


	while (viewer.isRunning()) {
		auto clock_start = std::chrono::high_resolution_clock::now();

		frontend.addFrame(std::make_shared<Frame>(cap));


		frontend.trackFeatures(); 
		frontend.estimatePose();
		
		if (frontend.getCurrFrame()->keypts.size() < 30) {
			frontend.getCurrFrame()->setKeyFrame();
			frontend.last_keyframe = frontend.getCurrFrame();
			frontend.extractNewFeatures(50, 300);
		}


        viewer.setFrames(frontend.getPrevFrame(), frontend.getCurrFrame());

		///////////////// POST MAIN ////////////////////////////
		auto clock_end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = clock_end - clock_start;
        viewer.updateFPS(elapsed.count());
	}
	
	cap.release();
	cv::destroyAllWindows();
	return 0;
}