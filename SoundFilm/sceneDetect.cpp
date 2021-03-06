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
#include <fstream>
#include <sstream>

using namespace std;
using namespace cv;

double cmpHist(Mat& last, Mat& current, int compare_method) {
    // Adopted from a demo on openCV tutorial:
    // http://docs.opencv.org/2.4/doc/tutorials/imgproc/histograms/histogram_comparison/histogram_comparison.html
    Mat hsv_last, hsv_current;
    
    cvtColor( last, hsv_last, COLOR_BGR2HSV );
    cvtColor( current, hsv_current, COLOR_BGR2HSV );
    
    int h_bins = 50; int s_bins = 60;
    int histSize[] = { h_bins, s_bins };
    float h_ranges[] = { 0, 180 };
    float s_ranges[] = { 0, 256 };
    
    const float* ranges[] = { h_ranges, s_ranges };
    
    int channels[] = { 0, 1 };
    
    MatND hist_last;
    MatND hist_current;
    
    calcHist( &hsv_last, 1, channels, Mat(), hist_last, 2, histSize, ranges, true, false );
    normalize( hist_last, hist_last, 0, 1, NORM_MINMAX, -1, Mat() );
    
    calcHist( &hsv_current, 1, channels, Mat(), hist_current, 2, histSize, ranges, true, false );
    normalize( hist_current, hist_current, 0, 1, NORM_MINMAX, -1, Mat() );

    /// Apply the histogram comparison methods
    double diff = 0;
    //for( int i = 0; i < 4; i++ ) {
    //cout << "hist last: " << hist_last << endl;
    //cout << "hist next: " << hist_current << endl;
        diff = compareHist( hist_last, hist_current, compare_method );
        //printf( " Method [%d] Difference : %f\n", 2, diff );
    //}
    
    //printf( "Done \n" );
    return diff;
}

string getTimecode(double fps, double curr_frame) {
    ostringstream timecode;
    ostringstream hr, min, sec, f;
    int i_hr, i_min, i_sec, i_f;
    
    i_f = ceil(fmod(curr_frame, fps));
    i_sec = floor(curr_frame / fps);
    i_hr = floor(i_sec / 3600);
    i_sec = i_sec % 3600;
    i_min = floor(i_sec / 60);
    i_sec = i_sec % 60;
    
    return convertTime(i_hr, i_min, i_sec);
}

string timeCal(string time1, string time2, bool ifPlus) {
    istringstream stream1 (time1);
    istringstream stream2 (time2);
    int h1, m1, s1, h2, m2, s2, h, s, m;
    char temp;
    stream1 >> h1 >> temp >> m1 >> temp >> s1;
    stream2 >> h2 >> temp >> m2 >> temp >> s2;
    if (ifPlus) {
        s = s1 + s2;
        m = m1 + m2;
        h = h1 + h2;
        if(s >= 60) {
            m += s / 60;
            s %= 60;
        }
        if (m >= 60) {
            m %= 60;
            h += m / 60;
        }
    } else {
        s = s1 - s2;
        m = m1 - m2;
        h = h1 - h2;
        if (s < 0) {
            s += 60;
            m -= 1;
        }
        if (m < 0) {
            m += 60;
            h -= 1;
        }
    }
    
    
    return convertTime(h, m, s);
}

string convertTime(int h, int m, int s) {
    ostringstream timecode, hr, min, sec, f;
    
    if (h < 10) {
        hr << 0 << h;
    } else {
        hr << h;
    }
    if (m < 10) {
        min << 0 << m;
    } else {
        min << m;
    }
    if (s < 10) {
        sec << 0 << s;
    } else {
        sec << s;
    }
    /*if (i_f < 100) {
     if (i_f < 10) {
     f << 00 << i_f;
     } else {
     f << 0 << i_f;
     }
     } else {
     f << i_f;
     }*/
    
    
    timecode << hr.str() << ":" << min.str() << ":" << sec.str();
    return timecode.str();
}

bool joinVideo(int i, int j, int method, double threshold) {
    if (j > 0) {
        if (compareVid(i, j, method, threshold)) {
            ofstream outputFile("Resources/output/video/temp.txt");
            outputFile << "file " << j << ".mp4" << endl;
            outputFile << "file " << i << ".mp4" << endl;
            system("ffmpeg -f concat -i Resources/output/video/temp.txt -c copy Resources/output/video/temp.mp4");
            system("rm Resources/output/video/temp.txt");
            
            ostringstream cmd, cmd2, cmd3, cmd4;
            cmd << "rm Resources/output/video/" << i << ".mp4" << endl;
            cmd2 << "rm Resources/output/video/" << j << ".mp4" << endl;
            cmd3 << "mv Resources/output/video/temp.mp4 Resources/output/video/" << j << ".mp4";
            cmd4 << "rm Resources/output/video/" << i << ".jpeg" << endl;
            system(cmd.str().c_str());
            system(cmd2.str().c_str());
            system(cmd3.str().c_str());
            system(cmd4.str().c_str());
            
            return true;
        }
    }
    return false;
}

int detect(string fname, int method, double threshold) {
    VideoCapture vid;
    vid.open(fname);
    if(!vid.isOpened()) {
        throw "Error when opening the file";
    }
    cout << "open file successful" << endl;
    double fps = vid.get(CV_CAP_PROP_FPS);
    Mat current_frame;
    Mat temp_frame;
    system("rm -rf Resources/output/video/*");
    //ofstream outputFile("Resources/output/video/video.txt");
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
    string last_timecode = "00:00:00";
    string curr_timecode = "00:00:00";
    string duration = "00:00:00";
    double index = 1;
    bool ifDiff;
    for (int i = 1; i < frameCount; i+=2 ) {
        if(k%2 != 0) {
            //if(floor(fmod(vid.get(CV_CAP_PROP_POS_FRAMES),fps/2)) == 0) {
            //for (int j = 0; j < fps; j++) {
                vid.read(next_frame);
            //}
            //imshow("Display", next_frame);
            //waitKey(500);
            //cout << "detecting at " << vid.get(CV_CAP_PROP_POS_MSEC)/1000 << "s" << endl;
            //cout << "next frame is " << vid.get(CV_CAP_PROP_POS_FRAMES) << endl;
            double diff = cmpHist(current_frame, next_frame, method);
            if (method == 0 || method == 2) {
                ifDiff = diff < threshold;
            } else {
                ifDiff = diff > threshold;
            }
            if(ifDiff) {
                if(vid.get(CV_CAP_PROP_POS_FRAMES) - timestamp > 2.5*fps) {
                    //cout << "scene changed at " << vid.get(CV_CAP_PROP_POS_MSEC)/1000 << "s ";
                    //cout << "scene changed at " << vid.get(CV_CAP_PROP_POS_FRAMES) << " frame" << endl;
                    //cout << " and diffrence is " << diff << endl;
                    //curr_time = vid.get(CV_CAP_PROP_POS_MSEC)/1000;
                    curr_time = vid.get(CV_CAP_PROP_POS_FRAMES);
                    
                    ostringstream cmd;
                    ostringstream cmd2;
                    
                    curr_timecode = getTimecode(fps, curr_time);
                    last_timecode = timeCal(last_timecode, duration, true);
                    duration = timeCal(curr_timecode, last_timecode, false);

                    cmd << "ffmpeg -ss " << last_timecode << " -i " << fname << " -t "
                        << duration << " Resources/output/video/" << index << ".mp4" << endl;
                    cmd2 << "ffmpeg -i " << " Resources/output/video/" << index << ".mp4" << " -ss " << getTimecode(fps, (curr_time - last_time)/2) << " -vframes 1 " << " Resources/output/video/" << index << ".jpeg" << endl;

                    //cout << "index is " << index << endl;
                    system(cmd.str().c_str());
                    system(cmd2.str().c_str());
                    if (!joinVideo(index, index-1, method, threshold)) {
                        //outputFile << "file " << index << ".mp4" << endl;
                        index++;
                    }
                    
                    //cout << cmd.str() << endl;
                    //cout << cmd2.str() << endl;
                }
                
                timestamp = vid.get(CV_CAP_PROP_POS_FRAMES);
                last_time = curr_time;
            }
            
            //cout << "current frame is " << vid.get(CV_CAP_PROP_POS_FRAMES) << endl;
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
            double diff = cmpHist(next_frame, current_frame, method);
            
            if (method == 0 || method == 2) {
                ifDiff = diff < threshold;
            } else {
                ifDiff = diff > threshold;
            }
            if(ifDiff) {
                if(vid.get(CV_CAP_PROP_POS_FRAMES) - timestamp > 2.5*fps) {
                    //cout << "scene changed at " << vid.get(CV_CAP_PROP_POS_MSEC)/1000 << "s ";
                    //cout << "scene changed at " << vid.get(CV_CAP_PROP_POS_FRAMES) << " frame" << endl;
                    //cout << "and diffrence is " << diff << endl;
                    //curr_time = vid.get(CV_CAP_PROP_POS_MSEC)/1000;
                    curr_time = vid.get(CV_CAP_PROP_POS_FRAMES);
                    
                    ostringstream cmd;
                    ostringstream cmd2;
                    
                    
                    curr_timecode = getTimecode(fps, curr_time);
                    last_timecode = timeCal(last_timecode, duration, true);
                    duration = timeCal(curr_timecode, last_timecode, false);

                    cmd << "ffmpeg -ss " << last_timecode << " -i " << fname << " -t "
                    << duration << " Resources/output/video/" << index << ".mp4" << endl;
                    cmd2 << "ffmpeg -i " << " Resources/output/video/" << index << ".mp4" << " -ss " << getTimecode(fps, (curr_time - last_time)/2) << " -vframes 1 " << " Resources/output/video/" << index << ".jpeg" << endl;
                    
                    //cout << "index is " << index << endl;
                    system(cmd.str().c_str());
                    system(cmd2.str().c_str());
                     
                    if (!joinVideo(index, index-1, method, threshold)) {
                        //outputFile << "file " << index << ".mp4" << endl;
                        index++;
                    }
                    
                    //cout << cmd.str() << endl;
                    //cout << cmd2.str() << endl;
                }
                
                timestamp = vid.get(CV_CAP_PROP_POS_FRAMES);
                last_time = curr_time;
            }
            
            //cout << "current frame is " << vid.get(CV_CAP_PROP_POS_FRAMES) << endl;
            diff_last = diff;
        }
        vid.read(temp_frame);
        k ++;
        /*last_frame = current_frame;
        cout << "last = current "<< endl;
        imshow( "Display", last_frame );
        waitKey(1000);*/
        // }
    }
    
    ostringstream cmd2;
    ostringstream cmd;
    last_timecode = timeCal(last_timecode, duration, true);
    duration = timeCal(getTimecode(fps, frameCount), last_timecode, false);
    cmd << "ffmpeg -ss " << last_timecode << " -i " << fname << " -t "
        << getTimecode(fps, frameCount - last_time) << " Resources/output/video/" << index << ".mp4" << endl;
    cmd2 << "ffmpeg -i " << " Resources/output/video/" << index << ".mp4" << " -ss " << getTimecode(fps, (frameCount - last_time)/2)
        << " -vframes 1 " << " Resources/output/video/" << index << ".jpeg" << endl;
    system(cmd.str().c_str());
    system(cmd2.str().c_str());
    //outputFile << "file " << index << ".mp4" << endl;
    //cout << cmd.str() << endl;
    //cout << cmd2.str() << endl;
    //system("ffmpeg -f concat -i Resources/output/video/video.txt -c copy Resources/output.mp4");
     
        cout << "scene detect end" << endl;
    //} else {
    //    throw "Error when reading first frame";
    //}
    
    vid.release();
    
    return index;
}

bool compareVid(int f1, int f2, int method, double threshold) {
    Mat src_base, src_test1;
    ostringstream fname1;
    ostringstream fname2;
    
    fname1 << "Resources/output/video/" << f1 << ".jpeg";
    fname2 << "Resources/output/video/" << f2 << ".jpeg";
    
    
    src_base = imread(fname1.str());
    src_test1 = imread(fname2.str());
    double diff = cmpHist(src_base, src_test1, method);
    //cout << "video " << f1 << " and " << f2 << "'s difference is " << diff << endl;
    if (method == 0 || method == 2) {
        if (diff > threshold + 0.1) {
            return true;
        }
    } else {
        if (diff < threshold + 0.1) {
            return true;
        }
    }
    return false;
}
