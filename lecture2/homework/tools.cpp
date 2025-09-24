#include "tools.hpp"
#include <fmt/core.h>

ResizeParams resizeAndCenterImage(const cv::Mat& input_image, cv::Mat& output_image, int targetSize) {
    ResizeParams params;
    params.original_width = input_image.cols;
    params.original_height = input_image.rows;
    
    // 创建 targetSize x targetSize 的黑色画布
    output_image = cv::Mat::zeros(targetSize, targetSize, input_image.type());
    
    // 计算缩放比例
    double scale_x = static_cast<double>(targetSize) / input_image.cols;
    double scale_y = static_cast<double>(targetSize) / input_image.rows;
    params.scale_ratio = std::min(scale_x, scale_y);
    
    // 计算缩放后的尺寸
    int new_width = static_cast<int>(input_image.cols * params.scale_ratio);
    int new_height = static_cast<int>(input_image.rows * params.scale_ratio);
    
    // 缩放图像
    cv::Mat resized_image;
    cv::resize(input_image, resized_image, cv::Size(new_width, new_height));
    
    // 计算居中位置
    params.offset_x = (targetSize - new_width) / 2;
    params.offset_y = (targetSize - new_height) / 2;
    
    // 将缩放后的图像复制到画布中央
    cv::Mat roi = output_image(cv::Rect(params.offset_x, params.offset_y, new_width, new_height));
    resized_image.copyTo(roi);
    
    return params;
}

void printResizeParams(const ResizeParams& params) {
    fmt::print("=== 图像缩放参数 ===\n");
    fmt::print("原始尺寸: {}x{}\n", params.original_width, params.original_height);
    fmt::print("缩放比例: {:.4f}\n", params.scale_ratio);
    fmt::print("偏移量: X={}, Y={}\n", params.offset_x, params.offset_y);
    fmt::print("缩放后尺寸: {}x{}\n", 
               static_cast<int>(params.original_width * params.scale_ratio),
               static_cast<int>(params.original_height * params.scale_ratio));
    fmt::print("===================\n");
}