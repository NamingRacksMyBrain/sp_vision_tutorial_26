#include "tasks/buff_detector.hpp"
#include <chrono>
#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>
#include "tools/plotter.hpp"
#include "tasks/buff_solver.hpp"

int main()
{
    cv::VideoCapture cap("assets/test.avi");

    if (!cap.isOpened()) {
        std::cerr << "无法打开视频文件!" << std::endl;
        return -1;
    }

    auto_buff::Buff_Detector detector;
    auto_buff::Buff_Solver solver;
    tools::Plotter plotter;

    while (true) {
        cv::Mat img;
        cap >> img;

        if (img.empty()) {
            std::cout << "视频播放完毕或无法读取帧" << std::endl;
            break;
        }

        auto fanblades = detector.detect(img);

        cv::Mat display_img = img.clone();
        for (const auto& fanblade : fanblades) {
            cv::Scalar color;
            std::string type_name;
            switch (fanblade.type) {
                case auto_buff::_target:
                    color = cv::Scalar(0, 255, 0);
                    type_name = "_target";
                    break;
                case auto_buff::_light:
                    color = cv::Scalar(0, 255, 255);
                    type_name = "_light";
                    break;
                case auto_buff::_unlight:
                    color = cv::Scalar(0, 0, 255);
                    type_name = "_unlight";
                    break;
            }

            // 绘制关键点
            for (size_t i = 0; i < fanblade.points.size(); ++i) {
                cv::circle(display_img, fanblade.points[i], 3, color, -1);
                cv::putText(display_img, std::to_string(i),
                           cv::Point(fanblade.points[i].x + 5, fanblade.points[i].y - 5),
                           cv::FONT_HERSHEY_SIMPLEX, 0.5, color, 1);
            }

            // 绘制中心点
            cv::circle(display_img, fanblade.center, 5, color, -1);
            cv::putText(display_img, "CENTER",
                       cv::Point(fanblade.center.x + 10, fanblade.center.y - 10),
                       cv::FONT_HERSHEY_SIMPLEX, 0.5, color, 1);

            // 绘制类型标签
            cv::putText(display_img, type_name,
                       cv::Point(fanblade.center.x - 20, fanblade.center.y - 20),
                       cv::FONT_HERSHEY_SIMPLEX, 0.7, color, 2);

            if (!fanblades.empty()) {
                // 任务一: 解算扇叶中心
                solver.solvePnP(fanblades[0]);

                // 任务二: 反推旋转中心
                cv::Point3f rot_center = solver.getRotationCenter();

                // 准备 PlotJuggler 数据
                nlohmann::json data;
                const cv::Point3f& fan_center = solver.history_centers_.back();  // 获取最新扇叶中心
                data["fan_center_x"] = fan_center.x;
                data["fan_center_y"] = fan_center.y;
                data["fan_center_z"] = fan_center.z;
                data["rot_center_x"] = rot_center.x;
                data["rot_center_y"] = rot_center.y;
                data["rot_center_z"] = rot_center.z;
                plotter.plot(data);
            }
        }

        // 显示检测结果
        cv::resize(display_img, display_img, {}, 0.8, 0.8);
        cv::imshow("Detection Results", display_img);
        if (cv::waitKey(30) == 27) { // 按ESC键退出
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}