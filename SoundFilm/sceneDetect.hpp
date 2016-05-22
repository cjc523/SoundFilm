//
//  sceneDetect.hpp
//  SoundFilm
//
//  Created by Peter Chen on 19/05/2016.
//  Copyright © 2016 a. All rights reserved.
//

#ifndef sceneDetect_hpp
#define sceneDetect_hpp

#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
using namespace std;
using namespace cv;

void detect(string fname);
double cmpHist(Mat& last, Mat& current);
void comparePic(string fname1, string fname2);

#endif /* sceneDetect_hpp */
