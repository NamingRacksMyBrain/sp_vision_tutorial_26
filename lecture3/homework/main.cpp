#include "io/my_camera.hpp"
#include "tasks/yolo.hpp"
#include "tasks/armor.hpp"
#include "opencv2/opencv.hpp"
#include "tools/img_tools.hpp"

int main()
{
    // 初始S化相机、yolo类
    myCamera camera;
    auto_aim::YOLO detector("./configs/yolo.yaml");

    while (1) {
        // 调用相机读取图像
        cv::Mat img = camera.read();
        if (img.empty()) {
            std::cerr << "Failed to capture image from camera" << std::endl;
            break;
        }

        // 调用yolo识别装甲板
        std::list<auto_aim::Armor> armors = detector.detect(img);
        
        for (const auto& armor : armors) {
            tools::draw_points(img, armor.points, cv::Scalar(0, 0, 255));
        }

        // 显示图像
        cv::resize(img, img , cv::Size(640, 480));
        cv::imshow("img", img);
        if (cv::waitKey(0) == 'q') {
            break;
        }
    }

    return 0;
}