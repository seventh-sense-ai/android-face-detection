#include <jni.h>
#include <string>
#include <omp.h>
#include <dirent.h>
#include <android/asset_manager_jni.h>
#include <opencv2/core/mat.hpp>
#include "handle.h"

#include "facedetector.h"
#include <android/bitmap.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

std::string jstring2string(JNIEnv *env, jstring jStr) {
    if (!jStr)
        return "";

    const jclass stringClass = env->GetObjectClass(jStr);
    const jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
    const jbyteArray stringJbytes = (jbyteArray) env->CallObjectMethod(jStr, getBytes,
                                                                       env->NewStringUTF("UTF-8"));

    size_t length = (size_t) env->GetArrayLength(stringJbytes);
    jbyte *pBytes = env->GetByteArrayElements(stringJbytes, NULL);

    std::string ret = std::string((char *) pBytes, length);
    env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);

    env->DeleteLocalRef(stringJbytes);
    env->DeleteLocalRef(stringClass);
    return ret;
}

extern "C"
JNIEXPORT void JNICALL
Java_ai_seventhsense_facedetection_LFDDetector_initialize(JNIEnv *env, jobject thiz, jint max_side) {
    FaceDetector *det = new FaceDetector((int) max_side);
    setHandle(env, thiz, det);
}

extern "C"
JNIEXPORT void JNICALL
Java_ai_seventhsense_facedetection_LFDDetector_closeNative(JNIEnv *env, jobject thiz) {
    FaceDetector *inst = getHandle<FaceDetector>(env, thiz);
    inst->close();
    delete inst;
}

void convert_bitmap_to_rgba_mat(JNIEnv* env, jobject& bitmap, cv::Mat& dst,
                            bool needUnPremultiplyAlpha) {
    AndroidBitmapInfo info;
    void* pixels = 0;

    try {
        CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
        CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                  info.format == ANDROID_BITMAP_FORMAT_RGB_565);
        CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
        CV_Assert(pixels);
        dst.create(info.height, info.width, CV_8UC4);
        if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
            cv::Mat tmp(info.height, info.width, CV_8UC4, pixels);
            if (needUnPremultiplyAlpha)
                cvtColor(tmp, dst, cv::COLOR_mRGBA2RGBA);
            else
                tmp.copyTo(dst);
        } else {
            // info.format == ANDROID_BITMAP_FORMAT_RGB_565
            cv::Mat tmp(info.height, info.width, CV_8UC2, pixels);
            cvtColor(tmp, dst, cv::COLOR_BGR5652RGBA);
        }
        AndroidBitmap_unlockPixels(env, bitmap);
        return;
    } catch (const cv::Exception& e) {
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("org/opencv/core/CvException");
        if (!je)
            je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());
        return;
    } catch (...) {
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, "Unknown exception in JNI code {nBitmapToMat}");
        return;
    }
}

void convert_mat_to_bitmap(JNIEnv *env,  cv::Mat& mat, jobject& bitmap,bool needPremultiplyAlpha) {
    AndroidBitmapInfo info;
    void *pixels = 0;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);//Get Bitmap information
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888//Picture format RGBA_8888 or RGB_565
              || info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(mat.dims==2&&info.height==(uint32_t)mat.rows && info.width==(uint32_t)mat.cols);
    CV_Assert(mat.type()==CV_8UC1||mat.type()==CV_8UC3||mat.type()==CV_8UC4);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);

    if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        cv::Mat tmp(info.height, info.width, CV_8UC4, pixels);
        switch (mat.type()){
            case CV_8UC1:
                cvtColor(mat,tmp,cv::COLOR_GRAY2RGBA);
                break;
            case CV_8UC3:
                cvtColor(mat,tmp,cv::COLOR_RGB2RGBA);
                break;
            case CV_8UC4:
                cvtColor(mat,tmp,cv::COLOR_RGBA2mRGBA);
                if (needPremultiplyAlpha) {
                    cvtColor(mat, tmp, cv::COLOR_RGBA2mRGBA);
                } else {
                    mat.copyTo(tmp);
                }
                break;
            default:break;
        }
    } else {
        cv::Mat tmp(info.height, info.width, CV_8UC2, pixels);
        switch (mat.type()){
            case CV_8UC1:
                cvtColor(mat,tmp,cv::COLOR_GRAY2BGR565);
                break;
            case CV_8UC3:
                cvtColor(mat,tmp,cv::COLOR_RGB2BGR565);
                break;
            case CV_8UC4:
                cvtColor(mat,tmp,cv::COLOR_RGBA2BGR565);
                break;
            default:break;
        }
    }
    AndroidBitmap_unlockPixels(env, bitmap);
}


extern "C"
JNIEXPORT jfloatArray JNICALL
Java_ai_seventhsense_facedetection_LFDDetector_detectNative(JNIEnv *env, jobject thiz,
        jobject bitmap, jobject thumbnail, jfloat thumbnail_scale) {
    FaceDetector *inst = getHandle<FaceDetector>(env, thiz);
    cv::Mat rgba;
    convert_bitmap_to_rgba_mat(env, bitmap, rgba, false);
    std::vector<std::vector<float>> result = inst->detect(rgba);

    std::vector<std::vector<float>> filtered_result;
    int biggest_index = -1;
    float max_area = -1;
    // Select the biggest face
    for (int i = 0; i < result.size(); i++) {
        std::vector<float> vals = result.at(i);
        float left = vals[0];
        float top = vals[1];
        float right = vals[2];
        float bottom = vals[3];
        float area = (right - left) * (bottom - top);
        if(area > max_area) {
            max_area = area;
            biggest_index = i;
        }
    }

    if(biggest_index != -1){
        filtered_result.push_back(result[biggest_index]);

        // Generate the crop
        std::vector<cv::Point2f> source, target;

        source.push_back(cv::Point2f( result[biggest_index][4], result[biggest_index][5]));
        source.push_back(cv::Point2f( result[biggest_index][6],result[biggest_index][7]));
        source.push_back(cv::Point2f(result[biggest_index][8], result[biggest_index][9]));
        source.push_back(cv::Point2f( result[biggest_index][10], result[biggest_index][11]));
        source.push_back(cv::Point2f( result[biggest_index][12], result[biggest_index][13]));

        target.push_back(cv::Point2f((30.2946 + 8) * thumbnail_scale, 51.6963 * thumbnail_scale));
        target.push_back(cv::Point2f((65.5318 + 8) * thumbnail_scale, 51.5014 * thumbnail_scale));
        target.push_back(cv::Point2f((48.0252 + 8) * thumbnail_scale, 71.7366 * thumbnail_scale));
        target.push_back(cv::Point2f((33.5493 + 8) * thumbnail_scale, 92.3655 * thumbnail_scale));
        target.push_back(cv::Point2f((62.7299 + 8) * thumbnail_scale, 92.2041 * thumbnail_scale));

        cv::Mat transform = cv::estimateAffinePartial2D(source, target, cv::noArray(),
                cv::LMEDS);
        cv::Mat crop;
        cv::warpAffine(rgba, crop, transform, cv::Size((int) (112.0 * thumbnail_scale), (int) (112.0 * thumbnail_scale)));
        convert_mat_to_bitmap(env, crop, thumbnail, false);
    }

    result = filtered_result;

    jfloat fill[result.size() * 15];
    int startIndex = 0;
    for (int i = 0; i < result.size(); i++) {
        std::vector<float> vals = result.at(i);
        for(int val : vals) {
            fill[startIndex++] = val;
        }
    }
    jfloatArray toRet = env->NewFloatArray(result.size() * 15);
    env->SetFloatArrayRegion(toRet, 0, result.size() * 15, fill);
    return toRet;
}

extern "C"
JNIEXPORT void JNICALL
Java_ai_seventhsense_facedetection_LFDDetector_setMaxSide(JNIEnv *env, jobject thiz,
                                                          jint max_side) {
    FaceDetector *inst = getHandle<FaceDetector>(env, thiz);
    inst->set_max_side((int) max_side);
}