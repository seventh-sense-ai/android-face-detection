//
// Created by seventhsense on 08/09/20.
//

#include "facedetector.h"
#include <opencv2/opencv.hpp>
#include <algorithm>

FaceDetector::FaceDetector(int max_side) {
    pBuffer = (unsigned char *) malloc(DETECT_BUFFER_SIZE);
    this->max_side = max_side;
}

void FaceDetector::close() {
    free(pBuffer);
}

std::vector<std::vector<float>> FaceDetector::detect(cv::Mat &frame) {
    float scale = 1.0;
    cv::Mat image = frame.clone();

    // We need to determine the scale of the image based on the max_side
    if(image.cols > this->max_side || image.rows > this->max_side) {
        scale = ((float) this->max_side) / (float) std::max(image.cols, image.rows);
        cv::resize(image, image, cv::Size(), scale, scale);
    }

    cv::Mat bgr_image;
    cv::cvtColor(image, bgr_image, cv::COLOR_RGBA2BGR);

    int * pResults = NULL;
    pResults = facedetect_cnn(pBuffer, (unsigned char *) (bgr_image.ptr(0)),
                              bgr_image.cols, bgr_image.rows, (int)bgr_image.step);
    std::vector<std::vector<float>> results;
    for(int i = 0; i < (pResults ? *pResults : 0); i++)
    {
        std::vector<float > result;
        short * p = ((short*)(pResults+1))+142*i;
        int confidence = p[0];
        float x = p[1];
        float y = p[2];
        float w = p[3];
        float h = p[4];

        // Left
        result.push_back(x / scale);
        // Top
        result.push_back(y / scale);
        // Right
        result.push_back((x + w) / scale);
        // Bottom
        result.push_back((y + h) / scale);

        // Landmarks
        for(int i=5; i<15; i++) {
            float mark = p[i];
            result.push_back(mark / scale);
        }
        // Score
        result.push_back(confidence);

        results.push_back(result);
    }
    return results;
}