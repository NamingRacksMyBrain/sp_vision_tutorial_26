#include "my_camera.hpp"
#include <unordered_map>

myCamera::myCamera() : handle_(nullptr) {
    if (!initializeCamera_()) {
        throw std::runtime_error("Failed to initialize camera");
    }
    if (!setCameraParameters_()) {
        cleanup_();
        throw std::runtime_error("Failed to set camera parameters");
    }
}

myCamera::~myCamera() {
    cleanup_();
}

cv::Mat myCamera::read() {
    int ret = MV_CC_StartGrabbing(handle_);
    if (ret != MV_OK) {
        return cv::Mat();
    }

    unsigned int nMsec = 100;
    ret = MV_CC_GetImageBuffer(handle_, &raw_, nMsec);
    if (ret != MV_OK) {
        return cv::Mat();
    }

    MV_CC_PIXEL_CONVERT_PARAM cvt_param;
    cv::Mat img(cv::Size(raw_.stFrameInfo.nWidth, raw_.stFrameInfo.nHeight), CV_8U, raw_.pBufAddr);

    cvt_param.nWidth = raw_.stFrameInfo.nWidth;
    cvt_param.nHeight = raw_.stFrameInfo.nHeight;
    cvt_param.pSrcData = raw_.pBufAddr;
    cvt_param.nSrcDataLen = raw_.stFrameInfo.nFrameLen;
    cvt_param.enSrcPixelType = raw_.stFrameInfo.enPixelType;
    cvt_param.pDstBuffer = img.data;
    cvt_param.nDstBufferSize = img.total() * img.elemSize();
    cvt_param.enDstPixelType = PixelType_Gvsp_BGR8_Packed;

    auto pixel_type = raw_.stFrameInfo.enPixelType;
    const static std::unordered_map<MvGvspPixelType, cv::ColorConversionCodes> type_map = {
        {PixelType_Gvsp_BayerGR8, cv::COLOR_BayerGR2RGB},
        {PixelType_Gvsp_BayerRG8, cv::COLOR_BayerRG2RGB},
        {PixelType_Gvsp_BayerGB8, cv::COLOR_BayerGB2RGB},
        {PixelType_Gvsp_BayerBG8, cv::COLOR_BayerBG2RGB}
    };
    cv::cvtColor(img, img, type_map.at(pixel_type));

    MV_CC_FreeImageBuffer(handle_, &raw_);
    return img;
}

bool myCamera::initializeCamera_() {
    MV_CC_DEVICE_INFO_LIST device_list;
    int ret = MV_CC_EnumDevices(MV_USB_DEVICE, &device_list);
    if (ret != MV_OK || device_list.nDeviceNum == 0) {
        return false;
    }

    ret = MV_CC_CreateHandle(&handle_, device_list.pDeviceInfo[0]);
    if (ret != MV_OK) {
        return false;
    }

    ret = MV_CC_OpenDevice(handle_);
    if (ret != MV_OK) {
        MV_CC_DestroyHandle(handle_);
        handle_ = nullptr;
        return false;
    }
    return true;
}

bool myCamera::setCameraParameters_() {
    int ret;
    ret = MV_CC_SetEnumValue(handle_, "BalanceWhiteAuto", MV_BALANCEWHITE_AUTO_CONTINUOUS);
    if (ret != MV_OK) return false;
    ret = MV_CC_SetEnumValue(handle_, "ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    if (ret != MV_OK) return false;
    ret = MV_CC_SetEnumValue(handle_, "GainAuto", MV_GAIN_MODE_OFF);
    if (ret != MV_OK) return false;
    ret = MV_CC_SetFloatValue(handle_, "ExposureTime", 10000);
    if (ret != MV_OK) return false;
    ret = MV_CC_SetFloatValue(handle_, "Gain", 20);
    if (ret != MV_OK) return false;
    ret = MV_CC_SetFrameRate(handle_, 60);
    if (ret != MV_OK) return false;
    return true;
}

void myCamera::cleanup_() {
    if (handle_ != nullptr) {
        MV_CC_StopGrabbing(handle_);
        MV_CC_CloseDevice(handle_);
        MV_CC_DestroyHandle(handle_);
        handle_ = nullptr;
    }
}