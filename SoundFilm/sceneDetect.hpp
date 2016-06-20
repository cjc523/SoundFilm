//
//  sceneDetect.hpp
//  SoundFilm
//
//  Created by Jiacheng Chen on 19/05/2016.
//  Copyright © 2016 a. All rights reserved.
//

#ifndef sceneDetect_hpp
#define sceneDetect_hpp

#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
using namespace std;
using namespace cv;

string timeCal(string time1, string time2, bool ifPlus);
string convertTime(int h, int m, int s);
int detect(string fname, int method, double threshold);
double cmpHist(Mat& last, Mat& current, int compare_method);
string getTimecode(double fps, double curr_frame);
bool compareVid(int f1, int f2, int method);
void comparePic(string fname1, string fname2);

#endif /* sceneDetect_hpp */
