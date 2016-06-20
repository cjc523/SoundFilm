//
//  addMusic.cpp
//  SoundFilm
//
//  Created by Jiacheng Chen on 27/05/2016.
//  Copyright © 2016 a. All rights reserved.
//

#include "addMusic.hpp"
#include "sceneDetect.hpp"
#include <iostream>
#include <fstream>
#include <map>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

struct Scene {
    int id;
    int mid;
    string time = "00:00:00";
};

void addMusic(string fname, int totalClips) {
    Scene * list = new Scene[totalClips];
    system("rm -rf Resources/output/audio/*");
    ofstream outputFile("Resources/output/audio/music.txt");
    for (int i = 1; i <= totalClips; i++) {
        ostringstream vname;
        
        vname << "Resources/output/video/" << i <<".mp4";
        VideoCapture vid;
        vid.open(vname.str());
        if(!vid.isOpened()) {
            throw "Error when opening the file";
        }
        double fps = vid.get(CV_CAP_PROP_FPS);
        double frameCount = vid.get(CV_CAP_PROP_FRAME_COUNT);
        string duration = getTimecode(fps, frameCount);
        int max = 9;
        int min = 1;
        int ran = rand()%(max-min + 1) + min;
        //cout << duration << endl;
        outputFile << "file " << i << ".wav" << endl;
        
        list[i-1].id = i;
        list[i-1].mid = ran;
        //list[i-1].time = timeCal(list[i-1].time, duration, true);
        for (int j = i-2; j > 0; j--) {
            if (compareVid(i, j, 3)) {
                list[i-1].mid = list[j-1].mid;
                list[i-1].time = list[j-1].time;
                ran = list[i-1].mid;
                break;
            }
        }
        
        ostringstream cmd;
        cmd << "ffmpeg -ss " << list[i-1].time << " -i Resources/Audio/Slow/" << ran << ".wav" << " -t "
            << duration << " Resources/output/audio/" << i << ".wav" << endl;
        list[i-1].time = timeCal(list[i-1].time, duration, true);
        system(cmd.str().c_str());
        //cout << cmd.str() << endl;
        vid.release();
    }
    
    outputFile.close();
    
    ostringstream cmd;
    cmd << "ffmpeg -f concat -i Resources/output/audio/music.txt -c copy Resources/output.wav" << endl;
    system(cmd.str().c_str());
}