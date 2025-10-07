#pragma once

#include <memory>
#include <thread>
#include <functional>
#include <pangolin/pangolin.h>
#include <opencv2/opencv.hpp>

// Forward declaration
class Frame;

class Viewer {
private:
	std::shared_ptr<Frame> curr_frame = nullptr;
	std::mutex viewer_mutex;
	bool viewer_is_running = true;
	
public:
	Viewer();

	void setCurrFrame(std::shared_ptr<Frame> frame);

	void ThreadLoop();
};