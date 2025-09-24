#ifndef HOMEWORK_TOOLS_HPP
#define HOMEWORK_TOOLS_HPP

#include <opencv2/opencv.hpp>
#include <fmt/format.h>

// ====== 结构体声明 ======
// 功能：存储缩放结果和相关参数
struct ResizeParams {
    double scale_ratio;
    int offset_x;
    int offset_y;
    int original_width;
    int original_height;
};

// ====== 函数声明 ======
// 功能：将图像等比例缩放并居中放置在指定大小的画布中
// 修正：添加输出参数 cv::Mat& output_image
ResizeParams resizeAndCenterImage(const cv::Mat& inputImage, cv::Mat& output_image, int targetSize = 640);

// 添加打印函数的声明
void printResizeParams(const ResizeParams& params);

#endif // HOMEWORK_TOOLS_HPP