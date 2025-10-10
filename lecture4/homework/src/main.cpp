#include "tasks/buff_detector.hpp"
#include "io/camera.hpp"
#include "tasks/buff_solver.hpp"
#include <chrono>
#include <opencv2/opencv.hpp>
#include "tools/plotter.hpp"
#include <iostream>
#include <filesystem>

int main()
{
    // 使用绝对路径
    std::string project_path = "/home/jzx/sp_vision_tutorial_26/lecture4/homework";
    std::string video_path = project_path + "/assets/test.avi";
    
    std::cout << "Opening video: " << video_path << std::endl;
    
    cv::VideoCapture cap(video_path);
    if (!cap.isOpened()) {
        std::cerr << "Error: Cannot open video file" << std::endl;
        std::cerr << "Tried path: " << video_path << std::endl;
        return -1;
    }

    auto_buff::Buff_Detector detector;
    auto_buff::Buff_Solver solver;
    tools::Plotter plotter;

    cv::Mat img;
    int frame_count = 0;
    
    std::cout << "Starting buff detection... Press ESC to exit" << std::endl;

    while (true) {
        cap >> img;
        if (img.empty()) {
            std::cout << "Video ended or cannot read frame" << std::endl;
            break;
        }

        // 调整图像大小（如果需要）
        cv::resize(img, img, cv::Size(640, 480));

        auto fanblades = detector.detect(img);
        frame_count++;

        if (!fanblades.empty()) {
            // 任务一: 解算扇叶中心
            solver.solvePnP(fanblades[0]);

            // 任务二: 反推旋转中心
            cv::Point3f rot_center = solver.getRotationCenter();

            // 准备 PlotJuggler 数据 - 简化格式
            nlohmann::json data;
            if (solver.hasHistoryCenters()) {
                const cv::Point3f& fan_center = solver.getLastCenter();
                
                // 任务一：扇叶中心位置（使用2D距离作为fanblade_point）
                float fan_distance = sqrt(fan_center.x * fan_center.x + fan_center.y * fan_center.y);
                data["fanblade_point"] = fan_distance;
                
                // 任务二：旋转中心位置（使用2D距离作为rotation_center）
                float rot_distance = sqrt(rot_center.x * rot_center.x + rot_center.y * rot_center.y);
                data["rotation_center"] = rot_distance;
                
                // 发布到 PlotJuggler
                plotter.plot(data);
                
                // 控制台输出
                if (frame_count % 10 == 0) {
                    std::cout << "Frame " << frame_count << " - Detected!" << std::endl;
                    std::cout << "  扇叶中心距离: " << fan_distance << std::endl;
                    std::cout << "  旋转中心距离: " << rot_distance << std::endl;
                }
            }

            // 在图像上可视化
            cv::circle(img, fanblades[0].center, 5, cv::Scalar(0, 255, 0), -1);
            cv::putText(img, "Detected", cv::Point(10, 30), 
                       cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
        } else {
            cv::putText(img, "Not Detected", cv::Point(10, 30), 
                       cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);
        }

        cv::imshow("Buff Detection", img);
        
        // 按ESC退出
        int key = cv::waitKey(30);
        if (key == 27) {
            std::cout << "ESC pressed, exiting..." << std::endl;
            break;
        }
    }
    
    std::cout << "Program finished. Total frames processed: " << frame_count << std::endl;
    return 0;
}