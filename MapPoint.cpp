#include "MapPoint.h"
#include "Feature.h"
#include <algorithm>

std::atomic<unsigned long> MapPoint::nextID_(0);

MapPoint::MapPoint() : id(nextID_++) {}

unsigned long MapPoint::getID() const {
    return id;
}

void MapPoint::addObservation(std::shared_ptr<Feature> feature) {
    std::lock_guard<std::mutex> lock(mutex_);
    observations_.push_back(feature);
}

bool MapPoint::rmObservation(std::shared_ptr<Feature> feature) {
    auto it = std::find_if(observations_.begin(), observations_.end(),
        [&feature](const std::weak_ptr<Feature>& obs) {
            auto sptr = obs.lock(); // lock ret's a shared ptr if the weak ptr's obj exists 
            return sptr && sptr == feature;
        });

    if (it == observations_.end()) { 
        return false; 
    }

    observations_.erase(it);
    feature->map_point_ptr.reset();
    return true;
}

void MapPoint::setPosition(Eigen::Matrix<double, 3, 1> new_pos) {
    position_ = new_pos;
}

Eigen::Matrix<double, 3, 1> MapPoint::getPosition() {
    std::lock_guard<std::mutex> lock(mutex_);
    return position_;
}

std::shared_ptr<MapPoint> MapPoint::createMapPoint() {
    return std::make_shared<MapPoint>();
}