#include <Eigen/Core>
#include <mutex>
#include <atomic>

class Frame;
class Feature;



/*
* 3D point in the 'world'
* 
* Multiple Frames can observe the same thing from different angles;
* when these Features are correlated to be the same point, that info
* is sotred as a MapPoint
*/ 


class MapPoint
{
public:
	MapPoint() : id(nextID++)
	unsigned long getID() const {return id;}

	// correlate a feature with a MapPoint 
	void addObservation(std::shared_ptr<Feature> feature) {
		std::lock_guard<std::mutex> lock(mutex);
		observations.push_back(feature);
	}

	bool rmObservation(std::shared_ptr<Feature> feature) {
		auto it = std::find_if(observations.begin(), obersvations.end(),
			[&feature](const std::weak_ptr<Feature>& obs) {
				auto sptr = obs.lock(); // lock ret's a shared ptr if the weak ptr's obj exists 
				return sptr && sptr == feature;
			});

		if (it == observations.end()) { return false; }

		observations.erase(it);
		feature->map_point_ptr.reset();
		return true;
	}


private:
	unsigned long id;
	Eigen::Matrix<double, 3, 1> position = Eigen::Matrix<double, 3, 1>::Zero();
	std::vector<std::weak_ptr<Feature>> observations;
	mutable std::mutex mutex;
	static std::atomic<unsigned long> nextID;
	
	void setPosition(Eigen::Matrix<double, 3, 1> new_pos) { 
		pos = new_pos; 
	}

	Eigen::Matrix<double, 3, 1> getPosition() { 
		std::lock_guard<std::mutex> lock(mutex);
		return pos; 
	} 

	static std::shared_ptr<MapPoint> createMapPoint() {
		return std::make_shared<MapPoint>();
	}
};

std::atomic<unisnged long> MapPoint::nextID(0);