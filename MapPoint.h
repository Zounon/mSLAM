#ifndef MAPPOINT_H
#define MAPPOINT_H

#include <Eigen/Core>
#include <mutex>
#include <atomic>
#include <memory>
#include <vector>
#include <opencv2/core.hpp>

class Frame;
class Feature;

/*
* 3D point in the 'world'
* 
* Multiple Frames can observe the same thing from different angles;
* when these Features are correlated to be the same point, that info
* is stored as a MapPoint
*/ 

class MapPoint
{
public:
    MapPoint();
    unsigned long getID() const;

    // correlate a feature with a MapPoint 
    void addObservation(std::shared_ptr<Feature> feature);
    bool rmObservation(std::shared_ptr<Feature> feature);

    void setPosition(Eigen::Matrix<double, 3, 1> new_pos);
    Eigen::Matrix<double, 3, 1> getPosition();

    static std::shared_ptr<MapPoint> createMapPoint();

private:
    unsigned long id;
    Eigen::Matrix<double, 3, 1> position_ = Eigen::Matrix<double, 3, 1>::Zero();
    std::vector<std::weak_ptr<Feature>> observations_;
    mutable std::mutex mutex_;
    static std::atomic<unsigned long> nextID_;
    cv::Mat descriptor_; // for loop closure / BA later;

    int num_observations_ = 0;
    int num_found_ = 0;
};

#endif // MAPPOINT_H