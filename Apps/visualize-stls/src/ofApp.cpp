#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    params.setName("Params");
    params.add(inFolderModelPath.set("In Models Folder", ""));
    params.add(inModelExt.set("In Model Ext", "stl"));
    params.add(outFolderPath.set("Out Folder", ""));
    params.add(index.set("Starting Index", 0, 0, 100000));
    panel.setup();
    panel.add(params);
    panel.loadFromFile(settingsFile);
    
    stlModel.minVal = -0.5;
    stlModel.maxVal = 0.5;
    
    // Todo: create output folder if it doesn't exist
    ofDirectory tmp;
    cout << outFolderPath << endl;
    if (!tmp.doesDirectoryExist(outFolderPath.get())) {
        tmp.createDirectory(outFolderPath.get());
    }
    tmp.close();
    
    dirM.open(inFolderModelPath.toString());
    dirM.allowExt(inModelExt.toString());
    dirM.listDir();
    
    cam.setFarClip(10000);
    cam.setNearClip(0.001);
    cam.setPosition(0, 2, 0);
    cam.lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
    cam.setFov(34);
    
    light.setup();
    light.enable();
    light.setPosition(2, 10, 4);
    light.setAttenuation(1,0.02,0);
    
    frameDuration = totalDuration / float(nTotalFrames);
    gifEncoder.setup(width, height, frameDuration, nColors);
    ofAddListener(ofxGifEncoder::OFX_GIF_SAVE_FINISHED, this, &ofApp::onGifSaved);
    
    fbo.allocate(width, height, GL_RGBA);
    img.allocate(width, height, OF_IMAGE_COLOR_ALPHA);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // If the previous GIF hasn't yet been saved, there's nothing to do
    if (!gifSaved) return;
    
    // Check if we're currently exporting
    if (!bExporting) {
        
        // Attempt to load a new STL
        string mpath = dirM.getPath(index);
        if (validSTL(mpath)) {
            
            // Successfully loadable
            
            // Load the model
            mesh.clear();
            stlModel.readToMesh(mpath, &mesh, 1);
            rotMesh = mesh;
            
            // Reset all transforms
            scale = 1;
            translation = ofVec3f(0,0,0);
            ax = 0;
            ay = 0;
            az = 0;
            
            // Mark that we will be exporting
            bExporting = true;
            // Set the current export frame to 0
            currentExportFrame = 0;
            
            // Clear the current GIF Maker
            gifEncoder.reset();
            
        } else {
            
            // Attempt to load a new model
            cout << "This is not an STL model." << endl;
            index = CLAMP(index+1, 0, dirM.size()-1);
            // try again next time
        }
        
    } else {
        
        // Check if we need to end the current export
        if (currentExportFrame == nTotalFrames) {
            // Stop exporting
            bExporting = false;
            
            // Save the gif
            gifSaved = false;
            gifEncoder.save(outFolderPath.toString() + "/" + dirM.getName(index).substr(0,dirM.getName(index).length()-inModelExt.toString().length()) + "gif");
            
            // Increment the counter
            index++;
        
        } else {
        
            // If the previous instruction has been received, then add a new one
            if (instructions.empty()) {
                instructions.push_back(stepDegrees);
            }
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(225);
    ofSetColor(255);
    
    if (bExporting && !instructions.empty()) {
        
        // Receive this instruction
        float stepValue = instructions.front();
        instructions.erase(instructions.begin());
        
        // Rotate accordingly
        az += stepValue;
        
        // Create a new frame and add it to the gif maker
        fbo.begin();
        ofClear(0, 0, 0, 0);
        ofBackground(backgroundColor);
        ofSetColor(255);
        
        cam.begin();
        light.enable();
        rotMesh.enableNormals();
        rotMesh.enableColors();
        ofEnableDepthTest(); // this must be on
        ofEnableLighting();
        
        // Draw the mesh
        ofPushMatrix();
        ofTranslate(translation);
        ofRotateXDeg(ax);
        ofRotateYDeg(ay);
        ofRotateZDeg(az);
        ofScale(scale, scale, scale);
        rotMesh.draw(OF_MESH_FILL);
        ofPopMatrix();
        
        ofDisableLighting();
        ofDisableDepthTest();
        cam.end();
        
        fbo.end();
        fbo.readToPixels(img.getPixels());
        img.update();
        gifEncoder.addFrame(img);
        
        // Increment the frame
        currentExportFrame++;
    }
    
    // Draw the fbo
    fbo.draw(0, 0);
    
    ofSetColor(255);
    ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 10, 20);
    
    ofSetColor(255, 0, 0);
    ofDrawRectangle(0, ofGetHeight()-3, float(ofGetWidth())*float(index)/float(dirM.size()), 3);
    
    ofSetColor(0);
    ofDrawBitmapString(ofToString(index)+"/"+ofToString(dirM.size()),ofGetWidth()-65,ofGetHeight()-6);
    ofSetColor(255);
}

//--------------------------------------------------------------
void ofApp::exit() {
    
    panel.saveToFile(settingsFile);
}

//--------------------------------------------------------------
void ofApp::onGifSaved(string &fileName) {
    cout << "gif saved as " << fileName << endl;
    
    // If the counter is too high, then exit
    if (index >= dirM.size()) ofExit();
    
    gifSaved = true;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    
}

//--------------------------------------------------------------
bool ofApp::validSTL(string path) {

    ofFile inFile;
    inFile.open(path, ofFile::ReadOnly);
    char* bytes = new char[6];
    inFile.read(bytes, 6);
    std::string beg( reinterpret_cast< char const* >(bytes) ) ;
    if (beg == "solid ") {
        inFile.close();
        return true;
    }

    inFile.seekg(80);
    unsigned int nFacets;
    inFile.read((char*)(&nFacets), 4);
    bool bBinaryOK = (84 + nFacets * 50) == inFile.getSize();
    inFile.close();
    return bBinaryOK;
}

//--------------------------------------------------------------
bool ofApp::validBinarySTL(string path) {
    
    ofFile inFile;
    inFile.open(path, ofFile::ReadOnly);
    
    inFile.seekg(80);
    unsigned int nFacets;
    inFile.read((char*)(&nFacets), 4);
    bool bBinaryOK = (84 + nFacets * 50) == inFile.getSize();
    inFile.close();
    return bBinaryOK;
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
