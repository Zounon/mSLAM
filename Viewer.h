#ifndef VIEWER_H
#define VIEWER_H

#include <memory>
#include <thread>
#include <functional>
#include <mutex>
#include <pangolin/pangolin.h>
#include <opencv2/opencv.hpp>

class Frame;

class Viewer {
private:
	std::shared_ptr<Frame> curr_frame = nullptr;
	std::shared_ptr<Frame> prev_frame = nullptr;
	std::mutex viewer_mutex;
	bool viewer_is_running = true;
	double fps_avg = 100.0;
	double fps_alpha = 0.03;
	
public:
	Viewer();

	void setFrames(std::shared_ptr<Frame> prev, std::shared_ptr<Frame> curr);

	//void setCurrFrame(std::shared_ptr<Frame> frame);

	void updateFPS(double frame_time);
	void annotateFrame();
	void ThreadLoop();

	bool isRunning() const { return viewer_is_running; }
	void stopRunning() {
		viewer_is_running = false;
	}
};

#endif // VIEWER_H