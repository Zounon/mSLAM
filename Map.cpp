#include "Map.h"
#include "MapPoint.h"

void Map::insertMapPoint(std::shared_ptr<MapPoint> map_point)
{
	std::lock_guard<std::mutex> lock(mutex);
	map_points[map_point->getID()] = map_point;
}