#pragma once

#include "ofMain.h"
#include "ofxAVFoundationVideoPlayer.h"

#define V_W 320
#define V_H 240
#define SOUND_SRC "river.mp3"
using namespace std;
class ofApp : public ofBaseApp{

public:
    
    void setup();
    void update();
    void draw();
    
    void handleProgressBarClick(int x, int y);
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    ofxAVFoundationVideoPlayer video;
    
    ofVideoGrabber 		vidGrabber;
    int 				camWidth;
    int 				camHeight;
    
    ofLight             light;
    ofEasyCam cam;
    
    
    //playing video + rutt etra stuff
    int yStep;
    
    int xStep;
    
    ofPixels vidPixels;
    
    ofMesh mesh;

    
    
    //sound stuff
    ofSoundStream soundStream;
    void audioIn(float * input, int bufferSize, int nChannels);
    
    vector <float> left;
    vector <float> right;
    vector <float> volHistory;
    
    int 	bufferCounter;
    int 	drawCounter;
    
    float smoothedVol;
    float scaledVol;
    
    //fft stuff
    ofSoundPlayer 		mp3;
    
    
    float 				* fftSmoothed;
    // we will draw a rectangle, bouncing off the wall:
    float 				px, py, vx, vy;
    
    int nBandsToGet;
    float prevx, prevy;

    

    
    // Shader
    ofShader shader;	//Shader
    ofFbo fbo;			//Buffer for intermediate drawing
    ofImage image;		//Sunflower image
    
    
    bool doShader;
    
    
    // GUI
    
    
};
