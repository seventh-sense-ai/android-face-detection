# Introduction

This face detection and alignment library provides Android software developers an easy to use
method of detecting a face in a Bitmap image and obtaining the `aligned` face thumbnail, where eyes, 
nose and mouth have been standardized to fixed coordinates.

# Installation

Just add the dependency to your module's gradle file:
```
implementation 'ai.seventhsense:facedetection:1.0.5'
```

# Usage

First obtain an instance of a Face Detector (right now the only one we include is based on the
open-source [libfacedetection](https://github.com/ShiqiYu/libfacedetection) library).

```
import ai.seventhsense.facedetection.Detection;
import ai.seventhsense.facedetection.LFDDetector;

...

LFDDetector detector = LFDDetector.getInstance(getApplicationContext());
```

Once you have the detector instance, you can use it to detect the largest face in a Bitmap:
```
Bitmap bm = BitmapFactory.decodeResource(getResources(), R.drawable.tom_cruise);
Detection detection = detector.detect(bm);
```

The `Detection` object contains the co-ordinates of the bounding box of the largest face and also
an aligned thumbnail which can be used with Seventh Sense' Facial Recognition algorithms.

# Compiling from source

Compiling the project completely from source code is a two step process.

The first step of compiling `libfacedetection` can be skipped as we have already bundled the
compiled binaries in this repository.

## Compiling libfacedetection

Android binaries for `libfacedetection` are already bundled with this project and located at 
`app/src/main/cpp/libs`. You typically do not need to build them again. If you would like to 
proceed with the bundled binaries, please skip this section.

If you would like to build them yourself, first git clone the  [libfacedetection](https://github.com/ShiqiYu/libfacedetection)
project.

Make sure you have Android Studio and NDK installed. Then, you can compile the library using cmake:
```
git clone https://github.com/ShiqiYu/libfacedetection
cd libfacedetection
mkdir build
cd build
cmake                                                           \
    -DCMAKE_TOOLCHAIN_FILE=/path/to/your/Android/Sdk/ndk-bundle/build/cmake/android.toolchain.cmake \
    -DANDROID_NDK=/path/to/your/Android/Sdk/ndk-bundle                               \
    -DANDROID_ABI=arm64-v8a                               \
    -DANDROID_PLATFORM=android-24                           \
    -DANDROID_STL=c++_shared                                \
    -DENABLE_NEON=ON \
    -DENABLE_AVX512=OFF \
    -DENABLE_AVX2=OFF \
    -DDEMO=OFF \
    -DUSE_OPENMP=ON ..
make
```

Repeat the process above with ABIs of `arm64-v8a` and `armeabi-v7a` and copy the generated 
`libfacedetection.a` to `app/src/main/cpp/libs/arm64-v8a` and `app/src/main/cpp/libs/armeabi-v7a`
respectively.

## Compiling this project

This project also statically links to OpenCV. Download [OpenCV-android-sdk](https://sourceforge.net/projects/opencvlibrary/files/4.5.0/opencv-4.5.0-android-sdk.zip/download).

Unzip the OpenCV SDK and then specify its path in `settings.gradle`

Sync your gradle and you should be able to build the project.

# License

Please see LICENSE file
