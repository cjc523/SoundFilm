//
//  sceneDetect.cpp
//  SoundFilm
//
//  Created by Jiacheng Chen on 19/05/2016.
//  Copyright © 2016 a. All rights reserved.
//

#include "sceneDetect.hpp"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

double pixel_mean(Mat frame, double height, double width) {
    double mean = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            mean += frame.at<char>(i, j);
        }
    }
    return mean/(height*width);
}

double pixel_var(Mat frame, double height, double width, double mean) {
    double var = 0;
    double pix;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            pix = frame.at<char>(i, j);
            var += (pix - mean) * (pix - mean);
        }
    }
    return var/(height*width);
}

double cmpHist(Mat& last, Mat& current) {
    // Adopted from a demo on openCV tutorial:
    // http://docs.opencv.org/2.4/doc/tutorials/imgproc/histograms/histogram_comparison/histogram_comparison.html
    Mat hsv_last, hsv_current;
    
    /// Convert to HSV
    cvtColor( last, hsv_last, COLOR_BGR2HSV );
    cvtColor( current, hsv_current, COLOR_BGR2HSV );
    
    /// Using 50 bins for hue and 60 for saturation
    int h_bins = 50; int s_bins = 60;
    int histSize[] = { h_bins, s_bins };
    
    // hue varies from 0 to 179, saturation from 0 to 255
    float h_ranges[] = { 0, 180 };
    float s_ranges[] = { 0, 256 };
    
    const float* ranges[] = { h_ranges, s_ranges };
    
    // Use the o-th and 1-st channels
    int channels[] = { 0, 1 };
    
    
    /// Histograms
    MatND hist_last;
    MatND hist_current;
    
    calcHist( &hsv_last, 1, channels, Mat(), hist_last, 2, histSize, ranges, true, false );
    normalize( hist_last, hist_last, 0, 1, NORM_MINMAX, -1, Mat() );
    
    calcHist( &hsv_current, 1, channels, Mat(), hist_current, 2, histSize, ranges, true, false );
    normalize( hist_current, hist_current, 0, 1, NORM_MINMAX, -1, Mat() );
    
    /// Apply the histogram comparison methods
    double diff = 0;
    //for( int i = 0; i < 4; i++ ) {
        int compare_method = 0;
        diff = compareHist( hist_last, hist_current, compare_method );
        //printf( " Method [%d] Difference : %f\n", i, diff );
    //}
    
    //printf( "Done \n" );
    return diff;
}

void detect(string fname) {
    VideoCapture vid;
    vid.open(fname);
    if(!vid.isOpened()) {
        throw "Error when opening the file";
    }
    cout << "open file successful" << endl;
    //int height = vid.get(CV_CAP_PROP_FRAME_HEIGHT);
    //int width = vid.get(CV_CAP_PROP_FRAME_WIDTH);
    //int size = height * width;
    double fps = round(vid.get(CV_CAP_PROP_FPS));
    
    //Mat last_frame;
    Mat current_frame;
    /*double fmean;
    double fvar;
    double last_fmean;
    double last_fvar;
    //double fps = round(vid.get(CV_CAP_PROP_FPS));
    if(vid.read(frame)) {
        cout << "starting scene detection" << endl;
        cvtColor(frame, gray_frame, COLOR_BGR2GRAY);
        last_fmean = pixel_mean(gray_frame, height, width);
        last_fvar = pixel_var(gray_frame, height, width, fmean);
        //cout << "fmean = " << fmean << endl;
        //cout << "fvar = " << fvar << endl;
        while(vid.read(frame)) {
            //cout << fmod(vid.get(CV_CAP_PROP_POS_FRAMES),fps) << endl;
            //if(floor(fmod(vid.get(CV_CAP_PROP_POS_FRAMES),fps)) == 0) {
                //cout << "at frame" << vid.get(CV_CAP_PROP_POS_FRAMES);
                cvtColor(frame, gray_frame, COLOR_BGR2GRAY);
                fmean = pixel_mean(gray_frame, height, width);
                fvar = pixel_var(gray_frame, height, width, fmean);
                //H0: mean - last_mean = 0
                double root = (sqrt((fabs(fvar - last_fvar))/size));
                double z_cal;
                //cout << "root is " << root << endl;
                if (root == 0) {
                    z_cal = 0;
                } else {
                    z_cal = fabs((fmean - last_fmean)/root);
                }
                //cout << "fmean = " << fmean << endl;
                //cout << "fvar = " << fvar << endl;
                //cout << "z value is " << z_cal << endl;
                //double z = 1.96; //significant level = 95%
                double z = 1500;
                if(z_cal > z) {
                    cout << "scene changed at " << vid.get(CV_CAP_PROP_POS_MSEC)/1000 << "s" << endl;
                }
                last_fmean = fmean;
                last_fvar = fvar;
           // }
        }
        cout << "scene detect end" << endl;
    } else {
        throw "Error when reading first frame";
    }*/
    
    //double last_timestamp = 0;
    //double current_timeStamp;
    //int frameCount = vid.get(CV_CAP_PROP_FRAME_COUNT);
    namedWindow( "Display", WINDOW_AUTOSIZE );
    //if(vid.read(last_frame)) {
        cout << "starting scene detection" << endl;
        
        while (vid.read(current_frame)) {
            Mat next_frame;
            //if(floor(fmod(vid.get(CV_CAP_PROP_POS_FRAMES),fps)) == 0) {
                vid.read(next_frame);
            //cout << "detecting at " << vid.get(CV_CAP_PROP_POS_MSEC)/1000 << "s" << endl;
            //cout << "current frame is " << vid.get(CV_CAP_PROP_POS_FRAMES) << endl;
            double diff = cmpHist(current_frame, next_frame);
            if (diff <= 0.2) {
                cout << "scene changed at " << vid.get(CV_CAP_PROP_POS_MSEC)/1000 << "s" << endl;
                cout << "diffrence is " << diff << endl;
            }
                /*last_frame = current_frame;
                cout << "last = current "<< endl;
                imshow( "Display", last_frame );
                waitKey(1000);*/
            //}
        
        
        }
        
        cout << "scene detect end" << endl;
    //} else {
    //    throw "Error when reading first frame";
    //}
    
    vid.release();
    
    
}

void comparePic(string fname1, string fname2) {
    /*Mat frame1, gray_frame1, frame2, gray_frame2;
    frame1 = imread(fname1);
    frame2 = imread(fname2);
    if(frame1.empty() || frame2.empty()) {
        throw "Error when opening the file";
    }
    cout << "open file successful" << endl;
    cvtColor(frame1, gray_frame1, COLOR_BGR2GRAY);
    cvtColor(frame2, gray_frame2, COLOR_BGR2GRAY);
    double fmean1;
    double fvar1;
    double fmean2;
    double fvar2;
    double height1 = frame1.rows;
    double width1 = frame1.cols;
    double height2 = frame2.rows;
    double width2 = frame2.cols;
    cout << "starting image compare" << endl;
    fmean1 = pixel_mean(gray_frame1, height1, width1);
    fvar1 = pixel_var(gray_frame1, height1, width1, fmean1);
    fmean2 = pixel_mean(gray_frame2, height2, width2);
    fvar2 = pixel_var(gray_frame2, height2, width2, fmean2);
    
    cout << "fmean1 = " << fmean1 << endl;
    cout << "fmean2 = " << fmean2 << endl;
    double z_cal;
    double root = (sqrt(fvar1/(height1*width1) - fvar2/(height2*width2)));
    if (root == 0) {
        z_cal = 0;
    } else {
        z_cal = fabs((fmean1 - fmean2)/root);
    }
    double z = 1.96; //significant level = 95%*/
    Mat frame1, frame2;
    frame1 = imread(fname1);
    frame2 = imread(fname2);
    if(frame1.empty() || frame2.empty()) {
    throw "Error when opening the file";
    }
    cout << "open file successful" << endl;
    
    cmpHist(frame1, frame2);
    /*if(diff > z) {
        cout << "images are different and z value is " << z_cal << endl;
    } else {
        cout << "images are NOT different and z value is " << z_cal << endl;
    }*/

    cout << "image compare end" << endl;
}
