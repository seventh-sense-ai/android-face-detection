//
// Created by seventhsense on 08/09/20.
//

#ifndef SDK7_FACEDETECTOR_H
#define SDK7_FACEDETECTOR_H
#include <stdio.h>
#include "facedetectcnn.h"
#define DETECT_BUFFER_SIZE 0x20000
#include <opencv2/core/mat.hpp>
class FaceDetector {
private:
    unsigned char * pBuffer;
    int max_side;
public:
    FaceDetector(int max_side);
    void close();
    std::vector<std::vector<float>> detect(cv::Mat&);
};


#endif //SDK7_FACEDETECTOR_H
