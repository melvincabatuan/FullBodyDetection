/****************************************************************************/
/*

The MIT License (MIT)
 
Copyright (c) Melvin Cabatuan
 
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
 
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
 
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

/****************************************************************************/

#include "io_github_melvincabatuan_fullbodydetection_MainActivity.h"
#include <android/log.h>
#include <android/bitmap.h>

#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"

using cv::Mat;
using cv::Size;
using cv::Point;
using cv::Rect;
using cv::ellipse;
using cv::Scalar;

using cv::getTickCount;
using cv::getTickFrequency;


#define  LOG_TAG    "FullBodyDetection"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  DEBUG 1


/** Global variables */
char full_body_cascade_path[100];
cv::CascadeClassifier full_body_cascade;

double t; // for measuring time performance


/*
 * Class:     io_github_melvincabatuan_fullbodydetection_MainActivity
 * Method:    predict
 * Signature: (Landroid/graphics/Bitmap;[B)V
 */
JNIEXPORT void JNICALL Java_io_github_melvincabatuan_fullbodydetection_MainActivity_predict
  (JNIEnv * pEnv, jobject clazz, jobject pTarget, jbyteArray pSource){

   AndroidBitmapInfo bitmapInfo;
   uint32_t* bitmapContent; // Links to Bitmap content

   if(AndroidBitmap_getInfo(pEnv, pTarget, &bitmapInfo) < 0) abort();
   if(bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888) abort();
   if(AndroidBitmap_lockPixels(pEnv, pTarget, (void**)&bitmapContent) < 0) abort();

   /// Access source array data... OK
   jbyte* source = (jbyte*)pEnv->GetPrimitiveArrayCritical(pSource, 0);
   if (source == NULL) abort();

   /// cv::Mat for YUV420sp source and output BGRA 
    Mat srcGray(bitmapInfo.height, bitmapInfo.width, CV_8UC1, (unsigned char *)source);
    Mat mbgra(bitmapInfo.height, bitmapInfo.width, CV_8UC4, (unsigned char *)bitmapContent);

/***********************************************************************************************/
    /// Native Image Processing HERE... 
    if(DEBUG){
      LOGI("Starting native image processing...");
    }

    if (full_body_cascade.empty()){
       t = (double)getTickCount();
       sprintf( full_body_cascade_path, "%s/%s", getenv("ASSETDIR"), "haarcascade_fullbody.xml");       
    
      /* Load the face cascades */
       if( !full_body_cascade.load(full_body_cascade_path) ){ 
           LOGE("Error loading cat face cascade"); 
           abort(); 
       };

       t = 1000*((double)getTickCount() - t)/getTickFrequency();
       if(DEBUG){
       LOGI("Loading full body cascade took %lf milliseconds.", t);
     }
    }
            
 
     std::vector<Rect> fbody;


       //-- Detect full body
       t = (double)getTickCount();
 
       /// Detection took cat_face_cascade.detectMultiScale() time = 655.334471 ms
      // cat_face_cascade.detectMultiScale( srcGray, faces, 1.1, 2 , 0 , Size(30, 30) ); // Scaling factor = 1.1;  minNeighbors = 2 ; flags = 0; minimumSize = 30,30

      // cat_face_cascade.detectMultiScale() time = 120.117185 ms
      // cat_face_cascade.detectMultiScale( srcGray, faces, 1.2, 3 , 0 , Size(64, 64));

 
      
      full_body_cascade.detectMultiScale( srcGray, fbody, 1.2, 2 , 0 , Size(14, 28));  // Size(double width, double height) 

      // scalingFactor parameters determine how much the classifier will be scaled up after each run.
      // minNeighbors parameter specifies how many positive neighbors a positive face rectangle should have to be considered a possible match; 
      // when a potential face rectangle is moved a pixel and does not trigger the classifier any more, it is most likely that it’s a false positive. 
      // Face rectangles with fewer positive neighbors than minNeighbors are rejected. 
      // If minNeighbors is set to zero, all potential face rectangles are returned. 
      // The flags parameter is from the OpenCV 1.x API and should always be 0. 
      // minimumSize specifies the smallest face rectangle we’re looking for. 

       t = 1000*((double)getTickCount() - t)/getTickFrequency();
       if(DEBUG){
          LOGI("full_body_cascade.detectMultiScale() time = %lf milliseconds.", t);
      }


       // Iterate through all faces and detect eyes
       t = (double)getTickCount();

       for( size_t i = 0; i < fbody.size(); i++ )
       {
          Point center(fbody[i].x + fbody[i].width / 2, fbody[i].y + fbody[i].height / 2);
          ellipse(srcGray, center, Size(fbody[i].width / 2, fbody[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
       }//endfor
  
       t = 1000*((double)getTickCount() - t)/getTickFrequency();
       if(DEBUG){
          LOGI("Iterate through all faces and detecting eyes took %lf milliseconds.", t);
       }

       /// Display to Android
       cvtColor(srcGray, mbgra, CV_GRAY2BGRA);


      if(DEBUG){
        LOGI("Successfully finished native image processing...");
      }
   
/************************************************************************************************/ 
   
   /// Release Java byte buffer and unlock backing bitmap
   pEnv-> ReleasePrimitiveArrayCritical(pSource,source,0);
   if (AndroidBitmap_unlockPixels(pEnv, pTarget) < 0) abort();
}
