#include "buff_solver.hpp"
#include <opencv2/calib3d.hpp>
#include <iostream>
#include <cmath>

namespace auto_buff
{
Buff_Solver::Buff_Solver() : filtered_rotation_center_(0, 0, 0) {
    // 相机内参矩阵
    camera_matrix_ = (cv::Mat_<double>(3, 3) <<
        1606.4621945373342, 0, 718.35189471286367,
        0, 1607.7959825661596, 556.20295066305948,
        0, 0, 1);

    // 畸变系数
    dist_coeffs_ = (cv::Mat_<double>(1, 5) <<
        -0.11902379858301006, 0.10035853057524424, 5.8754877878169769e-05,
        -0.00053565612841224299, 0);
}

void Buff_Solver::solvePnP(const FanBlade& fanblade) {
    if (fanblade.points.size() < 4) {
        std::cerr << "Insufficient points for PnP" << std::endl;
        return;
    }

    // 2D图像点: 使用外环四个点
    std::vector<cv::Point2f> image_points = {
        fanblade.points[0],  // top
        fanblade.points[1],  // left
        fanblade.points[2],  // bottom
        fanblade.points[3]   // right
    };

    // 去畸变
    std::vector<cv::Point2f> undistorted_points;
    cv::undistortPoints(image_points, undistorted_points, camera_matrix_, dist_coeffs_, cv::noArray(), camera_matrix_);

    // 3D世界点
    const float radius = 150.0f;  // mm
    std::vector<cv::Point3f> object_points = {
        cv::Point3f(0, radius, 0),
        cv::Point3f(-radius, 0, 0),
        cv::Point3f(0, -radius, 0),
        cv::Point3f(radius, 0, 0)
    };

    // PnP解算
    cv::Mat rvec, tvec;
    bool success = cv::solvePnP(object_points, undistorted_points, camera_matrix_, cv::noArray(), rvec, tvec, false, cv::SOLVEPNP_IPPE);
    
    if (!success) {
        std::cerr << "PnP failed" << std::endl;
        return;
    }

    cv::Point3f fan_center(static_cast<float>(tvec.at<double>(0)),
                           static_cast<float>(tvec.at<double>(1)),
                           static_cast<float>(tvec.at<double>(2)));

    // 更新历史中心数据
    history_centers_.push_back(fan_center);
    if (history_centers_.size() > HISTORY_SIZE) {
        history_centers_.pop_front();
    }
}

cv::Point3f Buff_Solver::getRotationCenter() {
    cv::Point3f current_center;
    
    if (history_centers_.size() < 5) {
        cv::Point3f avg(0, 0, 0);
        for (const auto& p : history_centers_) {
            avg += p;
        }
        current_center = avg / static_cast<float>(history_centers_.size());
    } else {
        current_center = fitCircleCenter(history_centers_);
    }
    
    // 应用低通滤波
    if (history_centers_.size() == 1) {
        filtered_rotation_center_ = current_center;
    } else {
        filtered_rotation_center_.x = alpha_ * current_center.x + (1 - alpha_) * filtered_rotation_center_.x;
        filtered_rotation_center_.y = alpha_ * current_center.y + (1 - alpha_) * filtered_rotation_center_.y;
        filtered_rotation_center_.z = alpha_ * current_center.z + (1 - alpha_) * filtered_rotation_center_.z;
    }
    
    return filtered_rotation_center_;
}

cv::Point3f Buff_Solver::fitCircleCenter(const std::deque<cv::Point3f>& points) {
    size_t n = points.size();
    if (n < 5) {
        cv::Point3f avg(0, 0, 0);
        for (const auto& p : points) avg += p;
        return avg / static_cast<float>(n);
    }

    // 去除异常点
    std::vector<cv::Point2f> valid_points;
    float avg_z = 0.0f;
    
    // 计算均值
    cv::Point2f mean(0, 0);
    for (const auto& p : points) {
        mean.x += p.x;
        mean.y += p.y;
        avg_z += p.z;
    }
    mean.x /= n;
    mean.y /= n;
    avg_z /= n;
    
    // 计算标准差
    float std_dev = 0;
    for (const auto& p : points) {
        float dist = std::sqrt(std::pow(p.x - mean.x, 2) + std::pow(p.y - mean.y, 2));
        std_dev += dist;
    }
    std_dev /= n;
    
    // 只保留在2倍标准差内的点
    for (const auto& p : points) {
        float dist = std::sqrt(std::pow(p.x - mean.x, 2) + std::pow(p.y - mean.y, 2));
        if (dist < 2 * std_dev) {
            valid_points.push_back(cv::Point2f(p.x, p.y));
        }
    }
    
    if (valid_points.size() < 3) {
        return cv::Point3f(mean.x, mean.y, avg_z);
    }
    
    // 使用有效点进行圆拟合
    n = valid_points.size();
    Eigen::MatrixXd A(n, 3);
    Eigen::VectorXd B(n);
    
    for (size_t i = 0; i < n; ++i) {
        float x = valid_points[i].x;
        float y = valid_points[i].y;
        A(i, 0) = 2 * x;
        A(i, 1) = 2 * y;
        A(i, 2) = 1;
        B(i) = x * x + y * y;
    }
    
    Eigen::VectorXd X = (A.transpose() * A).ldlt().solve(A.transpose() * B);
    return cv::Point3f(static_cast<float>(X(0)), static_cast<float>(X(1)), avg_z);
}

}  // namespace auto_buff