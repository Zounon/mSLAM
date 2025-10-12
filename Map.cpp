#include <mutex>
#include <unordered_map>

class Frame;
class MapPoint;

class Map
{
public:
	//void 
	void insertMapPoint(std::shared_ptr<MapPoint> map_point);



private:
	std::mutex mutex;
	std::unordered_map<unsigned long, std::shared_ptr<Frame>> frames;
	std::unordered_map<unsigned long, std::shared_ptr<MapPoint>> map_points;

	std::shared_ptr<Frame> curr_frame = nullptr;

	int max_frames = 5;
};


void Map::insertMapPoint(std::shared_ptr<MapPoint> map_point)
{
	std::lock_guard<std::mutex> lock(mutex);
	map_points[map_point->id] = map_point;
}