#ifndef MAP_H
#define MAP_H

#include <mutex>
#include <unordered_map>
#include <memory>

class Frame;
class MapPoint;

class Map
{
public:
    void insertMapPoint(std::shared_ptr<MapPoint> map_point);

private:
    std::mutex mutex;
    std::unordered_map<unsigned long, std::shared_ptr<Frame>> frames;
    std::unordered_map<unsigned long, std::shared_ptr<MapPoint>> map_points;

    std::shared_ptr<Frame> curr_frame;

    int max_frames;
};

#endif // MAP_H