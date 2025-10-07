#include "Viewer.h"
#include "Frame.h"

Viewer::Viewer() {
	std::thread viewer_thread = std::thread(std::bind(&Viewer::ThreadLoop, this));
	viewer_thread.detach();
}

void Viewer::setCurrFrame(std::shared_ptr<Frame> frame) {
	std::lock_guard<std::mutex> lock(viewer_mutex);
	curr_frame = frame;
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
		viwer_display.Activate(viewer_cam);

		std::lock_guard<std::mutex> lock(viewer_mutex);
		if (curr_frame->is_valid) {
			//DrawFrame(curr_frame, { 0,1,0 });
			//FollowCurrentFrame

			cv::imshow("curr frame", curr_frame->img);
			cv::waitKey(1);
		}
		pangolin::FinishFrame();
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}