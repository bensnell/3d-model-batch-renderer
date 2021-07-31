#pragma once

#include "ofMain.h"
#include "ofxRemoteUIServer.h"
#include "ofxAssimpModelLoader.h"
#include "Miniball.hpp"
#include "ofxSTLModel.h"
#include "ofxGifEncoder.h"

// This app works for aligning the busts's eyes

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        void onGifSaved(string & fileName);
    
    
    ofDirectory dirM;
    string inFolderModelPath = "inputs"; // contains the models we'll pull from
    string inModelExt = "ply";
    int index = 0;
    
    string outFolderPath = "outputs";
    string outExt = "tif";
    
    ofColor modelColor = ofColor(255);
    
    ofCamera cam;
    bool bCamOrtho = true;
    glm::vec3 camPos = {1,1,1};
    glm::vec3 camLookAt = {0,0,0};
    glm::vec2 camClip = {0,1000};
    float camFOV = 50;
    
    ofLight light1;
    glm::vec3 light1Pos = {2,2,2};
    glm::vec3 light1Attenuation = {1,0.02,0};
    

    bool bExporting = false;
    int currentExportFrame = 0;
    int stepDegrees = 5; // rotate this amount each time
    int nTotalFrames = 360 / stepDegrees;
    float totalDuration = 5.0f;
    float frameDuration;
    int nColors = 64;
    ofColor backgroundColor = ofColor(245);
    
    bool gifSaved = true;
    
    ofVboMesh mesh, rotMesh;

    bool validSTL(string path);
    bool validBinarySTL(string path);
    
    
    
    ofFbo fbo;
    glm::vec2 fboDims = {1000, 1000};
    
    
    float scale = 1;
    float ax, ay, az;
    ofVec3f translation;
    
    ofxSTLModel stlModel;
    
    ofxGifEncoder gifEncoder;
    int width = 600;
    int height = 600;
    
    
    ofImage img;
    
    vector<float> instructions; // how far to incrementally turn the model
    
};
