#ifndef AUTO_BUFF__SOLVER_HPP
#define AUTO_BUFF__SOLVER_HPP

#include <vector>
#include <deque>
#include <opencv2/opencv.hpp>
#include <eigen3/Eigen/Dense>
#include "buff_type.hpp"

namespace auto_buff
{
class Buff_Solver {
private:
    cv::Mat camera_matrix_;
    cv::Mat dist_coeffs_;
    static const size_t HISTORY_SIZE = 30;
    cv::Point3f filtered_rotation_center_;
    float alpha_ = 0.1f;
    
    cv::Point3f fitCircleCenter(const std::deque<cv::Point3f>& points);

public:
    std::deque<cv::Point3f> history_centers_;
    
    Buff_Solver();
    
    void solvePnP(const FanBlade& fanblade);
    
    cv::Point3f getRotationCenter();
    
    const std::deque<cv::Point3f>& getHistoryCenters() const {
        return history_centers_;
    }
    
    bool hasHistoryCenters() const {
        return !history_centers_.empty();
    }
    
    cv::Point3f getLastCenter() const {
        if (!history_centers_.empty()) {
            return history_centers_.back();
        }
        return cv::Point3f(0, 0, 0);
    }
};
}  // namespace auto_buff
#endif  // AUTO_BUFF__SOLVER_HPP