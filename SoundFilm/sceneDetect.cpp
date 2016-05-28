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
#include <opencv2/imgcodecs.hpp>
#include <iostream>

using namespace std;
using namespace cv;

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
        int compare_method = 3;
    //cout << "hist last: " << hist_last << endl;
    //cout << "hist next: " << hist_current << endl;
        diff = compareHist( hist_last, hist_current, compare_method );
        //printf( " Method [%d] Difference : %f\n", 2, diff );
    //}
    
    //printf( "Done \n" );
    return diff;
}

int detect(string fname) {
    VideoCapture vid;
    vid.open(fname);
    if(!vid.isOpened()) {
        throw "Error when opening the file";
    }
    cout << "open file successful" << endl;
    int fps = round(vid.get(CV_CAP_PROP_FPS));
    Mat current_frame;
    Mat temp_frame;
    system("rm -rf Resources/output/video/*");
    //double last_timestamp = 0;
    //double current_timeStamp;
    int frameCount = vid.get(CV_CAP_PROP_FRAME_COUNT);
    namedWindow( "Display", WINDOW_AUTOSIZE );
    //if(vid.read(last_frame)) {
        cout << "starting scene detection" << endl;
    vid.read(current_frame);
    Mat next_frame;
    int k = 1;
    int timestamp = 0;
    double diff_last = 0;
    double last_time = 0;
    double curr_time = 0;
    double index = 1;
    for (int i = 1; i < frameCount; i++ ) {
        if(k%2 != 0) {
            //if(floor(fmod(vid.get(CV_CAP_PROP_POS_FRAMES),fps/2)) == 0) {
            //for (int j = 0; j < fps; j++) {
                vid.read(next_frame);
            //}
            //imshow("Display", next_frame);
            //waitKey(500);
            //cout << "detecting at " << vid.get(CV_CAP_PROP_POS_MSEC)/1000 << "s" << endl;
            //cout << "next frame is " << vid.get(CV_CAP_PROP_POS_FRAMES) << endl;
            double diff = cmpHist(current_frame, next_frame);
            //if ((fabs((diff - diff_last)/diff_last) >= 0.98 && fabs((diff - diff_last)/diff_last) < 1.2)
              // || (fabs((diff - diff_last)/diff_last) >= 10 && fabs((diff - diff_last)/diff_last) < 20)) {
            if(diff > 0.5) {
                if(vid.get(CV_CAP_PROP_POS_FRAMES) - timestamp > 1.5*fps) {
                    /*cout << "scene changed at " << vid.get(CV_CAP_PROP_POS_MSEC)/1000 << "s" << endl;
                    cout << "scene changed at " << vid.get(CV_CAP_PROP_POS_FRAMES) << " frame" << endl;
                    cout << "diffrence is " << diff << endl;*/
                    ostringstream cmd;
                    ostringstream cmd2;
                    cmd << "ffmpeg -ss " << last_time << " -i " << fname << " -t "
                    << curr_time - last_time << " Resources/output/video/" << index << ".mp4" << endl;
                    cmd2 << "ffmpeg -i " << " Resources/output/video/" << index << ".mp4" << " -ss " << (curr_time - last_time)/2
                    << " -vframes 1 " << " Resources/output/video/" << index << ".jpeg" << endl;
                    index++;
                    //cout << "index is " << index << endl;
                    system(cmd.str().c_str());
                    system(cmd2.str().c_str());
                }
                
                timestamp = vid.get(CV_CAP_PROP_POS_FRAMES);
                curr_time = vid.get(CV_CAP_PROP_POS_MSEC)/1000;
                last_time = curr_time;
                
            }
            //cout << "current frame is " << vid.get(CV_CAP_PROP_POS_FRAMES) << endl;
            vid.read(temp_frame);
            diff_last = diff;
        } else {
            
            //if(floor(fmod(vid.get(CV_CAP_PROP_POS_FRAMES),fps/2)) == 0) {
            //for (int j = 0; j < fps; j++) {
                vid.read(current_frame);
            //}
            //imshow("Display", current_frame);
            //waitKey(500);
            //cout << "detecting at " << vid.get(CV_CAP_PROP_POS_MSEC)/1000 << "s" << endl;
            //cout << "next frame is " << vid.get(CV_CAP_PROP_POS_FRAMES) << endl;
            double diff = cmpHist(next_frame, current_frame);
            //if ((fabs((diff - diff_last)/diff_last) >= 0.98 && fabs((diff - diff_last)/diff_last) < 1.2)
                //|| (fabs((diff - diff_last)/diff_last) >= 10 && fabs((diff - diff_last)/diff_last) < 20)) {
                if(diff > 0.5) {
                if(vid.get(CV_CAP_PROP_POS_FRAMES) - timestamp > 2.5*fps) {
                    /*cout << "scene changed at " << vid.get(CV_CAP_PROP_POS_MSEC)/1000 << "s" << endl;
                    cout << "scene changed at " << vid.get(CV_CAP_PROP_POS_FRAMES) << " frame" << endl;
                    cout << "diffrence is " << diff << endl;*/
                    
                    ostringstream cmd;
                    ostringstream cmd2;
                    cmd << "ffmpeg -ss " << last_time << " -i " << fname << " -t "
                    << curr_time - last_time << " Resources/output/video/" << index << ".mp4" << endl;
                    cmd2 << "ffmpeg -i " << " Resources/output/video/" << index << ".mp4" << " -ss " << (curr_time - last_time)/2
                    << " -vframes 1 " << " Resources/output/video/" << index << ".jpeg" << endl;
                    index++;
                    //cout << "index is " << index << endl;
                    system(cmd.str().c_str());
                    system(cmd2.str().c_str());
                }
                    
                timestamp = vid.get(CV_CAP_PROP_POS_FRAMES);
                curr_time = vid.get(CV_CAP_PROP_POS_MSEC)/1000;
                last_time = curr_time;
            }
            //cout << "current frame is " << vid.get(CV_CAP_PROP_POS_FRAMES) << endl;
            vid.read(temp_frame);
            diff_last = diff;
        }
        
        k ++;
        /*last_frame = current_frame;
        cout << "last = current "<< endl;
        imshow( "Display", last_frame );
        waitKey(1000);*/
        // }
    }
    /*
    ostringstream cmd2;
    ostringstream cmd;
    cmd << "ffmpeg -ss " << last_time << " -i " << fname << " -t "
        << frameCount/fps - last_time << " Resources/output/video/" << index << ".mp4" << endl;
    cmd2 << "ffmpeg -i " << " Resources/output/video/" << index << ".mp4" << " -ss " << (curr_time - last_time)/2
        << " -vframes 1 " << " Resources/output/video/" << index << ".jpeg" << endl;
    system(cmd.str().c_str());
    system(cmd2.str().c_str());
     */
        cout << "scene detect end" << endl;
    //} else {
    //    throw "Error when reading first frame";
    //}
    
    vid.release();
    system("rm -rf Resources/output/video/*");
    return index;
}

void comparePic(string fname1, string fname2) {
    Mat src_base, hsv_base;
    Mat src_test1, hsv_test1;
    Mat hsv_half_down;
    
    src_base = imread(fname1);
    src_test1 = imread(fname2);
    
    /// Convert to HSV
    cvtColor( src_base, hsv_base, COLOR_BGR2HSV );
    cvtColor( src_test1, hsv_test1, COLOR_BGR2HSV );
    
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
    MatND hist_base;
    MatND hist_test1;
    
    /// Calculate the histograms for the HSV images
    calcHist( &hsv_base, 1, channels, Mat(), hist_base, 2, histSize, ranges, true, false );
    normalize( hist_base, hist_base, 0, 1, NORM_MINMAX, -1, Mat() );
    
    calcHist( &hsv_test1, 1, channels, Mat(), hist_test1, 2, histSize, ranges, true, false );
    normalize( hist_test1, hist_test1, 0, 1, NORM_MINMAX, -1, Mat() );
    
    /// Apply the histogram comparison methods
    for( int i = 0; i < 4; i++ )
    {
        int compare_method = i;
        double base_test1 = compareHist( hist_base, hist_test1, compare_method );
        
        printf( " Method [%d] Base-Test(1) : %f \n", i, base_test1);
    }
    
    printf( "Done \n" );
}
