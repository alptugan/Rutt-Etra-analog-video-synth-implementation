#include "ofApp.h"

static int const kProgressBarHeight = 30;

//--------------------------------------------------------------
void ofApp::setup(){
    // set window width and height
    ofSetWindowShape(1024, 768);
    
    camWidth 		=  320;	// try to grab at this size.
    camHeight 		=  240;
    
    //vidGrabber.initGrabber(camWidth,camHeight);
    
    
    
	video.load("surf_sm.MOV");
    video.setLoopState(OF_LOOP_NORMAL);
    video.setVolume(0);
	video.play();
    
    // Sound
    mp3.load(SOUND_SRC);
    
    // FFT
    fftSmoothed = new float[8192];
    for (int i = 0; i < 8192; i++){
        fftSmoothed[i] = 0;
    }
    
    nBandsToGet = V_H;
    
    mp3.play();
    yStep = 3;
    xStep = 1;
    
    
    // FBO

    shader.load( "shaderVert.c", "shaderFrag.c" );
    
    fbo.allocate( V_W, V_H,GL_RGBA,1 );
    
    
#ifdef TARGET_OPENGLES
    shader.load("shaders_gles/noise.vert","shaders_gles/noise.frag");
#else
    if(ofIsGLProgrammableRenderer()){
        shader.load("shaders_gl3/noise.vert", "shaders_gl3/noise.frag");
        //cout << "shaders_gl3" << endl;
    }else{
        //shader.load("shaders/noise.vert", "shaders/noise.frag");
    }
#endif
    
    doShader = false;
    
    mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
    //mesh.setMode(OF_PRIMITIVE_POINTS);
    //video.setFrame(330);
    //video.setPaused(true);

}

//--------------------------------------------------------------
void ofApp::update(){
    
    if(!video.isLoaded()) {
        return;
    }
    
    // Sound
    ofSoundUpdate();
    
    float * val = ofSoundGetSpectrum(nBandsToGet);		// request 128 values for fft
    for (int i = 0;i < nBandsToGet; i++){
        
        // let the smoothed calue sink to zero:
        fftSmoothed[i] *= 0.96f;
        
        // take the max, either the smoothed or the incoming:
        if (fftSmoothed[i] < val[i]) fftSmoothed[i] = val[i];
        
    }
    
    //lets scale the vol up to a 0-1 range
    scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
    
    //lets record the volume into an array
    volHistory.push_back( scaledVol );
    
    //if we are bigger the the size we want to record - lets drop the oldest value
    if( volHistory.size() >= 400 ){
        volHistory.erase(volHistory.begin(), volHistory.begin()+1);
    }
    
     video.update();
    
    if (video.isFrameNew()) {
        vidPixels.clear();
        mesh.clear();
        
        vidPixels = video.getPixels();
        
        // Todo: Video height/wigth verilen degerden kucukse patlama var.
        // o yuzden video cozunurlugunu gecme. Scale ile cozuyoruz.
        // cozum ??
        
        for (int y = 0; y < V_H; y += yStep) {
            
            for (int x = 0; x < V_W; x += xStep) {
                ofColor cl = vidPixels.getColor(x, y);
                if(cl.a > 0) {
                    mesh.addColor(ofColor(cl,255));
                    //shader.setuniform4
                    
                    //cout << fftSmoothed[y]*30.f << endl;
                    // set the z-deth of the line according to brightness of the current pixel color at position
                    mesh.addVertex(ofVec3f(x, y,  cl.getBrightness()*0.3 + fftSmoothed[y]*30.f));
                }
            }
        }
        
    }
   
}

//--------------------------------------------------------------
void ofApp::draw(){
    // light.enable();
    ofBackground(0);
    
    ////////////////////////////////////////////////////////////
    // Start 3D Camera /////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    
    
    ofNoFill();
    // below line is same as ofEnableDepthTest();
    glEnable(GL_DEPTH_TEST);
    
    
    cam.begin();
    
    
    //uncomment for video grabber
    //  ofTranslate(-vidGrabber.width/2, -vidGrabber.height/2);
    
    ofPushMatrix();
    // Scale the canvas x2
    ofScale(2, -2, 2);
    //translate based on size of video
    ofTranslate(-V_W/2, -V_H/2);
    
    
    
    
    
    if( doShader ){
        shader.begin();
        //we want to pass in some varrying values to animate our type / color
        
        //we also pass in the mouse position
        //we have to transform the coords to what the shader is expecting which is 0,0 in the center and y axis flipped.
        shader.setUniform2f("mouse", mouseX - ofGetWidth()/2, ofGetHeight()/2-mouseY );
        
        shader.setUniform1i("H", V_H );
        shader.setUniform1i("W", V_W);
        
        shader.setUniform1i("xStep", xStep);
        shader.setUniform1i("yStep", yStep);
        
        shader.setUniformTexture("mesh", *video.getTexturePtr(),1);
        
    }
    
    
    
    if( doShader ){
        shader.end();
    }
    
    //fbo.begin();
    //ofBackground(0);
    //finally draw our text
    mesh.draw();
    
    //fbo.end();
    
    //fbo.draw(0,0);
    
    
    ofPopMatrix();
    
    cam.end();
    glDisable(GL_DEPTH_TEST);
    ////////////////////////////////////////////////////////////
    // Finish 3D Camera
	////////////////////////////////////////////////////////////
    
    ofSetColor(255);
    
    ofColor yelloColor(255, 255, 0, 255 * 0.7); // yellow.
    ofColor blackColor(0); // black.

    //---------------------------------------------------------- draw video texture to fullscreen.
    /*ofRectangle screenRect(0, 0, ofGetWidth(), ofGetHeight());
    ofRectangle videoRect(0, 0, video.getWidth(), video.getHeight());
    ofRectangle videoFullscreenRect = videoRect;
    videoFullscreenRect.scaleTo(screenRect, OF_ASPECT_RATIO_KEEP_BY_EXPANDING);
    
    video.draw(videoFullscreenRect);*/
    
    //---------------------------------------------------------- draw progress bar.
    
    float progress = video.getPosition();
    ofRectangle progressRect;
    progressRect.width = ofGetWidth();
    progressRect.height = kProgressBarHeight;
    progressRect.y = ofGetHeight() - progressRect.height;
    
    ofFill();
    ofSetColor(blackColor);
    ofDrawRectangle(progressRect);
    
    progressRect.width = ofMap(progress, 0.0, 1.0, 0.0, ofGetWidth());
    
    ofSetColor(yelloColor);
    ofDrawRectangle(progressRect);
    ofSetColor(255);
    
    //---------------------------------------------------------- draw info.
    int x = 20;
    int y = 0;
    string str = "";
    
    str = "LOADED = ";
    if(video.isLoaded() == true) {
        str += "TRUE";
    } else {
        str += "FALSE";
    }
    ofDrawBitmapStringHighlight(str, x, y+=22, yelloColor, blackColor);
    
    str = "PLAYING = ";
    if(video.isPlaying() == true) {
        str += "TRUE";
    } else {
        str += "FALSE";
    }
    ofDrawBitmapStringHighlight(str, x, y+=22, yelloColor, blackColor);
    
    str = "NEW FRAME = ";
    if(video.isFrameNew() == true) {
        str += "TRUE";
    } else {
        str += "FALSE";
    }
    ofDrawBitmapStringHighlight(str, x, y+=22, yelloColor, blackColor);
    
    str = "POSITION = ";
    str += ofToString(video.getPosition(), 3);
    ofDrawBitmapStringHighlight(str, x, y+=22, yelloColor, blackColor);
    
    str = "FRAME NUM = ";
    str += ofToString(video.getCurrentFrame());
    ofDrawBitmapStringHighlight(str, x, y+=22, yelloColor, blackColor);
    
    str = "FRAME RATE = ";
    str += ofToString(ofGetFrameRate());
    ofDrawBitmapStringHighlight(str, x, y+=22, yelloColor, blackColor);
}

void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    float curVol = 0.0;
    
    // samples are "interleaved"
    int numCounted = 0;
    
    //lets go through each sample and calculate the root mean square which is a rough way to calculate volume
    for (int i = 0; i < bufferSize; i++){
        left[i]		= input[i*2]*0.5;
        right[i]	= input[i*2+1]*0.5;
        
        curVol += left[i] * left[i];
        curVol += right[i] * right[i];
        numCounted+=2;
    }
    
    //this is how we get the mean of rms :)
    curVol /= (float)numCounted;
    
    // this is how we get the root of rms :)
    curVol = sqrt( curVol );
    
    smoothedVol *= 0.93;
    smoothedVol += 0.07 * curVol;
    
    bufferCounter++;

}

//--------------------------------------------------------------
void ofApp::handleProgressBarClick(int x, int y) {
    ofRectangle progressRect(0, ofGetHeight()-kProgressBarHeight, ofGetWidth(), kProgressBarHeight);
    if(progressRect.inside(x, y) == true) {
        float progress = ofMap(x, 0, ofGetWidth(), 0.0, 1.0, true);
        video.setPosition(progress);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == ' ') {
        bool bPaused = video.isPaused();
        video.setPaused(!bPaused);
    }
    
    if(key=='y'){
        if(yStep<100)
            yStep++;
        
    }
    
    if(key=='u'){
        if(yStep>1)
            yStep--;
    }
    
    if( key == 's' ){
        doShader = !doShader;
    }
    
    if(key == 'r') {
        cam.reset();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    handleProgressBarClick(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    handleProgressBarClick(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}