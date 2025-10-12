#include <memory>
#include <opencv2/core.hpp>


class Frame;
class MapPoint;

/*
* Features of an image, usually corners
*
* Within Frame there can be many Features to detect (goodFeaturesToTrack)
* Features are unique to the Frame they are detected in
* However two Features (from separate Frames) can detect the same point
* these corrleated Features then make a MapPoint
*/
class Feature
{
public:
	std::weak_ptr<Frame> frame_ptr; // the Frame this Feature was generated from 
	cv::KeyPoint keypt; // 2D position in image Frame
	std::weak_ptr<MapPoint> map_point_ptr; // 3D point in world 




};