#pragma once

#include "ofMain.h"
#include "ofxGui.h"
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
    
    ofxPanel panel;
    ofParameterGroup params;
    ofParameter<string> inFolderModelPath; // contains the models we'll pull from
    ofParameter<string> inModelExt;
    ofParameter<string> outFolderPath;
    ofParameter<int> index;
    string settingsFile = "settings.xml";
    
    ofDirectory dirM;

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
    
    ofCamera cam;
    
    ofLight light;
    
    float scale = 1;
    float ax, ay, az;
    ofVec3f translation;
    
    ofxSTLModel stlModel;
    
    ofxGifEncoder gifEncoder;
    int width = 600;
    int height = 600;
    
    
    ofFbo fbo;
    ofImage img;
    
    vector<float> instructions; // how far to incrementally turn the model
    
};
