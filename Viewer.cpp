#include "Viewer.h"
#include "Frame.h"

#include <format>

Viewer::Viewer() {
	std::thread viewer_thread = std::thread(std::bind(&Viewer::ThreadLoop, this));
	viewer_thread.detach();
}

void Viewer::setFrames(std::shared_ptr<Frame> prev, std::shared_ptr<Frame> curr) {
	std::lock_guard<std::mutex> lock(viewer_mutex);
	curr_frame = curr;
	prev_frame = prev;
}

void Viewer::updateFPS(double frame_time) {
    double fps = (frame_time > 0.0) ? (1.0 / frame_time) : 0.0;
    fps_avg = fps_alpha * fps + (1.0 - fps_alpha) * fps_avg;
}

void Viewer::annotateFrame() {
	//for (const auto& pt : curr_frame->keypts) {
	//	cv::circle(curr_frame->img, pt, 5, cv::Scalar(0, 255, 0), -1);
	//}
	for (const auto& pt : prev_frame->keypts) {
		cv::circle(curr_frame->img, pt, 2, cv::Scalar(255, 255, 0), -1);
	}
	//for (size_t i = 0; i < prev.keypts.size() && i < curr.keypts.size(); ++i) {
	//	cv::line(curr.img, prev.keypts[i], curr.keypts[i], cv::Scalar(0, 255, 255), 1);
	//}
	for (auto i=0; i < curr_frame->keypts.size() && i < prev_frame->keypts.size(); ++i) {
		if (curr_frame->keypts[i] == prev_frame->keypts[i]) continue;
		cv::line(curr_frame->img, prev_frame->keypts[i], curr_frame->keypts[i], cv::Scalar(0, 255, 255), 1);
		std::cout << std::format("pt {}: ({:.1f}, {:.1f}) -> ({:.1f}, {:.1f})\n", i,
			prev_frame->keypts[i].x, prev_frame->keypts[i].y,
			curr_frame->keypts[i].x, curr_frame->keypts[i].y);
	}
	
	cv::putText(curr_frame->img, std::format("FPS: {:.1f}", fps_avg),
		{ 100, 50 }, cv::FONT_HERSHEY_COMPLEX, 1.0, { 255, 0, 255 }, 1);
	
	cv::putText(curr_frame->img, std::format("# feat: {}", curr_frame->keypts.size()),
		{ 100, 80 }, cv::FONT_HERSHEY_COMPLEX, 1.0, { 255, 0, 255 }, 1);
}

void Viewer::ThreadLoop() {
	pangolin::CreateWindowAndBind("mSLAM Viewer", 1920, 1080);
	//glEnable(GL_DEPTH_TEST);

	pangolin::OpenGlRenderState viewer_cam(
		pangolin::ProjectionMatrix(1920, 1080, 420, 420, 960, 540, 0.1, 1000), // TODO: check these params
		pangolin::ModelViewLookAt(0, -10, -8, 0, 0, 0, pangolin::AxisY) // TODO: check these params
	);

	pangolin::View& viwer_display = pangolin::CreateDisplay()
		.SetBounds(0.0, 1.0, 0.0, 1.0, -1920.0f / 1080.0f)
		.SetHandler(new pangolin::Handler3D(viewer_cam));

	while (!pangolin::ShouldQuit() && viewer_is_running) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//viwer_display.Activate(viewer_cam);

		std::lock_guard<std::mutex> lock(viewer_mutex);
		if (curr_frame->is_valid) {
			//DrawFrame(curr_frame, { 0,1,0 });
			//FollowCurrentFrame

			annotateFrame();
			//for (const auto& pt : curr_frame->keypts) {
			//	cv::circle(curr_frame->img, pt, 3, cv::Scalar(0, 255, 0), -1);
			//}

			//pangolin::glDrawColouredCube();

			cv::imshow("curr frame", curr_frame->img);
			if (cv::waitKey(1) == 27) {
				//viewer_display.
				break;
			}
		}
		pangolin::FinishFrame();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}