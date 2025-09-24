#include <opencv2/opencv.hpp>
#include <iostream>
#include "tools.hpp"

int main() {
    // 读取图片
    std::string image_path;
    std::cout << "请输入图片路径: ";
    std::cin >> image_path;
    
    cv::Mat image = cv::imread(image_path);
    
    if (image.empty()) {
        std::cout << "无法读取图片，请检查路径是否正确！" << std::endl;
        return -1;
    }
    
    std::cout << "成功读取图片，尺寸: " << image.cols << "x" << image.rows << std::endl;
    
    // 缩放并居中图片
    cv::Mat result_image;
    ResizeParams params = resizeAndCenterImage(image, result_image);
    
    // 打印参数
    printResizeParams(params);
    
    // 显示结果
    cv::imshow("原始图片", image);
    cv::imshow("缩放居中结果 (640x640)", result_image);
    
    std::cout << "按任意键退出..." << std::endl;
    cv::waitKey(0);
    
    return 0;
}