//#include "mSLAM.h"
#include <print>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <ranges>
#include <chrono>

class Frontend
{
public:
	int num_features = 100;
	int min_distance /*btwn features*/ = 20; // TODO: min dist should be a fn of input img;
	//int frame_id = 0;

	cv::Ptr<cv::GFTTDetector> gftt = cv::GFTTDetector::create(num_features, 0.01, 20);

	void trackFeatures(cv::Mat& prev_bw, cv::Mat& curr_bw,
		std::vector<cv::Point2f>& prev_keypts, std::vector<cv::Point2f>& curr_keypts)
	{
		if (prev_keypts.size() < 10) {
			std::cerr << "Too few keypts to track" << std::endl;
			return;
		}

		std::vector<uchar> status;
		std::vector<float> err;

		cv::calcOpticalFlowPyrLK(prev_bw, curr_bw, prev_keypts, curr_keypts,
			status, err,
			cv::Size(11, 11), 3,
			cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, 0.01),
			0, 1e-4);

		std::vector<cv::Point2f> prev_keypts_filtered, curr_keypts_filtered;
		for (size_t i = 0; i < status.size(); ++i) {
			if (status[i] == 1 && err[i] < 12.0f) {
				prev_keypts_filtered.push_back(prev_keypts[i]);
				curr_keypts_filtered.push_back(curr_keypts[i]);
			}
		}
		prev_keypts = std::move(prev_keypts_filtered);
		curr_keypts = std::move(curr_keypts_filtered);
	}
};

class Frame
{
public:
	//int id;
	bool is_valid = false;
	cv::Mat img;
	cv::Mat bw;
	std::vector<cv::Point2f> keypts;

	Frame(cv::VideoCapture& cap)
	{
		if (!cap.read(img) || img.empty()) {
			std::cerr << "Failed to load img from cap device" << std::endl;
			return;
		};
		is_valid = true;


		img = img(cv::Range(img.size().height * 2 / 10, img.size().height * 7 / 10), cv::Range(0, img.size().width)).clone();

		//int height = img.size().height;
		//int width = img.size().width;
		//cv::Mat mask = cv::Mat::zeros(img.size(), CV_8UC1);
		//cv::Rect mask_shape(width / 10, height / 10, width * 8 / 10, height * 6 / 10);
		//cv::rectangle(mask, mask_shape, cv::Scalar(255), -1);
		//img.setTo(cv::Scalar(0, 0, 20), ~mask);


		cv::extractChannel(img, bw, 1);
		//std::print("Frame size: {} {} \n", img.size().width, img.size().height);
		//std::print("BW Frame size: {} {}\n", bw.size().width, bw.size().height);

	}
	
	//Frame(cv::VideoCapture& cap, cv::Mat mask)
	//{
	//	if (!cap.read(img) || img.empty()) {
	//		std::cerr << "Failed to load img from cap device" << std::endl;
	//		return;
	//	};
	//	is_valid = true;

	//	

	//	cv::extractChannel(img, bw, 1);

	//}
	
};


int main()
{

	cv::VideoCapture cap("C:\\Users\\mm\\source\\repos\\mSLAM\\media\\slow-snowcoverd-road.MP4");
	if (!cap.isOpened()) {
		std::cerr << "Error: can't open video file" << std::endl;
		return -1;
	}

	///////////////// PRE MAIN INIT ////////////////////////////
	Frontend frontend;
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

		cv::imshow("display", curr.img);
		if (cv::waitKey(1) == 27) {
			break;
		}


		prev = std::move(curr);
	}

	cap.release();
	cv::destroyAllWindows();
	return 0;
}