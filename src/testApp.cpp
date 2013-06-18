/***********************************************************************
 
 Copyright (c) 2008, 2009, Memo Akten, www.memo.tv
 *** The Mega Super Awesome Visuals Company ***
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of MSA Visuals nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ***********************************************************************/


#include "MSAPhysics3D.h"
#include "testApp.h"
#include "ofxSimpleGuiToo.h"
#include "MSATimer.h"
#include "ofxTweener.h"

using namespace MSA;

#define	SPRING_MIN_STRENGTH		0.005
#define SPRING_MAX_STRENGTH		0.1

#define	SPRING_MIN_WIDTH		1
#define SPRING_MAX_WIDTH		3

#define NODE_MIN_RADIUS			5
#define NODE_MAX_RADIUS			15

#define MIN_MASS				1
#define MAX_MASS				3

/*#define minBounce				0.2
#define MAX_BOUNCE				0.7*/

#define	FIX_PROBABILITY			10		// % probability of a particle being fixed on creation

#define FORCE_AMOUNT			70

#define EDGE_DRAG				0.98

#define	GRAVITY					1

#define MAX_ATTRACTION			10
#define MIN_ATTRACTION			3

#define SECTOR_COUNT			1		// currently there is a bug at sector borders

int min_depth	=			100;
int max_depth	=			2000;

int step = 				20;
int pRadius = 20;
float minBounce	=			0.2;
float maxBounce	=			0.7;

bool				mouseAttract	= false;
bool				doMouseXY		= false;		// pressing left mmouse button moves mouse in XY plane
bool				doMouseYZ		= false;		// pressing right mouse button moves mouse in YZ plane
bool				doRender		= true;
int					forceTimer		= false;

int 	imgNum = int(ofRandom(11.49)); 
bool				drawLive		= true;
bool				drawSnapshot	= true;


float				rotSpeed		= 0;
float				mouseMass		= 1;
float rot =     61;

static int			width;
static int			height;


Physics::World3D		physics;
//Physics::Particle3D		mouseNode;
Timer::Timer            myTimer;


ofImage partImage [11];

//ofImage					ballImage;


GLfloat mat_shininess[] = { 50.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };

void initScene() {
	// clear all particles and springs etc
	physics.clear();
	
	// you can add your own particles to the physics system
//	physics.addParticle(&mouseNode);
//	mouseNode.makeFixed();
//	mouseNode.setMass(MIN_MASS);
//	mouseNode.moveTo(Vec3f(0, 0, 0));
//	mouseNode.setRadius(NODE_MAX_RADIUS);
	
	/* or tell the system to create and add particles
	 physics.makeParticle(Vec3f(-width/4, 0, -width/4), MIN_MASS)->makeFixed();		// create a node in top left back and fix
	 physics.makeParticle(Vec3f( width/4, 0, -width/4), MIN_MASS)->makeFixed();		// create a node in top right back and fix
	 physics.makeParticle(Vec3f(-width/4, 0,  width/4), MIN_MASS)->makeFixed();		// create a node in top left front and fix
	 physics.makeParticle(Vec3f( width/4, 0,  width/4), MIN_MASS)->makeFixed();		// create a node in top right front and fix*/
	
	
	
	
}

int kinectXToMsaX (int _kinXInt) {
	int converted = _kinXInt * (ofGetWidth()/640) + (-width/2); 
	return converted; 
}

int kinectYToMsaY (int _kinYInt) {
	int converted = _kinYInt * (ofGetWidth()/480) + ((-height/4) - 100);
	return converted; 
}


int msaXToKinectX (int _msaXInt) {
	int converted = _msaXInt / (ofGetWidth()/640) + (-width/ (2*(ofGetWidth()/640)));
	return converted; 
}

int msaYToKinectY (int _msaYInt) {
	int converted = _msaYInt / (ofGetWidth()/480) + ((-height/(4*(ofGetWidth()/480))) ) + 240; 
	return converted; 
}

int convertPXtoCanvas(int _px) {
	int converted = ofMap(_px, width/2, -width/2, 0, 1280);
	
	return converted;
}


int convertPZtoCanvas(int _pz) {
	int converted = ofMap(_pz, -width/2, width/2, 0, 720);
	
	return converted;
}

int convertPYtoCanvas(int _py) {
	int converted = ofMap(_py, height, 0, 0, 720, true);
	
	return converted;
}


//--------------------------------------------------------------

void testApp::takeSnapshot() {
	
	
	if (drawLive) {
		float mass		= ofRandom(MIN_MASS, MAX_MASS);
		float bounce	= ofRandom(minBounce, maxBounce);
		//float radius	= ofRandom(5, 20);
		
		for(int i=0; i<physics.numberOfParticles(); i++) {
			Physics::Particle3D *p = physics.getParticle(i);
			
			glPushMatrix();
			
			glTranslatef(width/2, 0, -width / 3);	
			
			if (p->isFixed()) {
				// physics.makeParticle returns a particle pointer so you can customize it
				//Physics::Particle3D *p2 = physics.makeParticle(Vec3f(p->getPosition().x + ofRandom(-5,5), p->getPosition().y + ofRandom(-5,5), p->getPosition().z + 100));
				Physics::Particle3D *p2 = physics.makeParticle(Vec3f(p->getPosition().x+ ofRandom(-3,3), p->getPosition().y+ ofRandom(-3,3), p->getPosition().z));
				
				// and set a bunch of properties (you don't have to set all of them, there are defaults)
				p2->setMass(mass)->setBounce(bounce)->setRadius(pRadius)->enableCollision()->makeFree()
				->setRed(ofNormalize(kinect.getColorAt(msaXToKinectX(p->getPosition().x), msaYToKinectY(p->getPosition().y)).r, 0, 255))
				->setGreen(ofNormalize(kinect.getColorAt(msaXToKinectX(p->getPosition().x), msaYToKinectY(p->getPosition().y)).g, 0, 255))
				->setBlue(ofNormalize(kinect.getColorAt(msaXToKinectX(p->getPosition().x), msaYToKinectY(p->getPosition().y)).b, 0, 255)) ->setImage(p->myImage) ->addVelocity(Vec3f(ofRandom(-1, 1), ofRandom(-1, 1), ofRandom(-1, 1)))
				
				;
				
				//->setImage(ofToInt(ofToString(ofRandom(0, 3))));
				
				if (p && p->isFixed()) p->kill();
				
			}
			
			glPopMatrix(); 
			
		}
		drawLive = false; 
	}
	
	else if (!drawLive) {
		
		for(int i=0; i<physics.numberOfParticles(); i++) {
			Physics::Particle3D *p = physics.getParticle(i);
			if (p && p->isFree()) p->kill();
			
		}
		
		drawLive = true; 
		
	}
	
	
	
}







void addRandomParticle() {
	
	float posX		= ofRandom(-(width/2)/step, (width/2)/step) * step;
	
	
	//float posY		= ofRandom(-height, height);
	float posY		=  (ofRandom(-height/step, height/step) * step) - 100;
	
	float posZ		= 0;
	float mass		= ofRandom(MIN_MASS, MAX_MASS);
	float bounce	= ofRandom(minBounce, minBounce);
	float radius	= ofMap(mass, MIN_MASS, MAX_MASS, NODE_MIN_RADIUS, NODE_MAX_RADIUS);
	
	// physics.makeParticle returns a particle pointer so you can customize it
	Physics::Particle3D *p = physics.makeParticle(Vec3f(posX, posY, posZ));
	
	// and set a bunch of properties (you don't have to set all of them, there are defaults)
	p->setMass(mass)->setBounce(bounce)->setRadius(radius)->enableCollision()->makeFree()->setImage(p->myImage);
	
	// add an attraction to the mouseNode
	//if(mouseAttract) physics.makeAttraction(&mouseNode, p, ofRandom(MIN_ATTRACTION, MAX_ATTRACTION));
}

void addRandomSpring() {
	Physics::Particle3D *a = physics.getParticle((int)ofRandom(0, physics.numberOfParticles()));
	Physics::Particle3D *b = physics.getParticle((int)ofRandom(0, physics.numberOfParticles()));
	physics.makeSpring(a, b, ofRandom(SPRING_MIN_STRENGTH, SPRING_MAX_STRENGTH), ofRandom(10, width/2));
}


void killRandomParticle() {
	Physics::Particle3D *p = physics.getParticle(floor(ofRandom(0, physics.numberOfParticles())));
	if(p && p->isFree()) p->kill();
}

void killRandomSpring() {
	Physics::Spring3D *s = physics.getSpring( floor(ofRandom(0, physics.numberOfSprings())));
	if(s) s->kill();
}

void killRandomConstraint() {
	Physics::Constraint3D *c = physics.getConstraint(floor(ofRandom(0, physics.numberOfConstraints())));
	if(c) c->kill();
}


//void toggleMouseAttract() {
//	mouseAttract = !mouseAttract;
//	if(mouseAttract) {
//		// loop through all particles and add attraction to mouse
//		// (doesn't matter if we attach attraction from mouse-mouse cos it won't be added internally
//		for(int i=0; i<physics.numberOfParticles(); i++) physics.makeAttraction(&mouseNode, physics.getParticle(i), ofRandom(MIN_ATTRACTION, MAX_ATTRACTION));
//	} else {
//		// loop through all existing attractsions and delete them
//		for(int i=0; i<physics.numberOfAttractions(); i++) physics.getAttraction(i)->kill();
//	}
//}

void addRandomForce(float f) {
	forceTimer = f;
	for(int i=0; i<physics.numberOfParticles(); i++) {
		Physics::Particle3D *p = physics.getParticle(i);
		if(p->isFree()) p->addVelocity(Vec3f(ofRandom(-f, f), ofRandom(-f, f), ofRandom(-f, f)));
	}
}

void lockRandomParticles() {
	for(int i=0; i<physics.numberOfParticles(); i++) {
		Physics::Particle3D *p = physics.getParticle(i);
		if(ofRandom(0, 100) < FIX_PROBABILITY) p->makeFixed();
		else p->makeFree();
	}
	//mouseNode.makeFixed();
}

void unlockRandomParticles() {
	for(int i=0; i<physics.numberOfParticles(); i++) {
		Physics::Particle3D *p = physics.getParticle(i);
		p->makeFree();
	}
	//mouseNode.makeFixed();
}



void testApp::updateMSAKinect() {
	
	int w = 640;
	int h = 480;
	
	
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			
			
			if(kinect.getDistanceAt(x, y) > min_depth && kinect.getDistanceAt(x, y) < max_depth ) {
				
				bool alreadyExists = false;
				
				
				for(int i=0; i<physics.numberOfParticles(); i++) {
					Physics::Particle3D *p = physics.getParticle(i);
					
					if (p->getPosition().x < kinectXToMsaX(x) + 10 && p->getPosition().x > kinectXToMsaX(x) - 10 
						&&  p->getPosition().y < kinectYToMsaY(y) + 10 && p->getPosition().y > kinectYToMsaY(y) - 10 && p->isFixed()) 
					{
						alreadyExists = true; 
					}
					
				}
				
				if (!alreadyExists) {
					Physics::Particle3D *p = physics.makeParticle(Vec3f(kinectXToMsaX(x), kinectYToMsaY(y), - ofMap(kinect.getDistanceAt(x,y), min_depth, max_depth, -width/2, width/2)));				
					p->enableCollision()->makeFixed()->setRadius(pRadius)->setImage(p->myImage);
				}
				
				
				
			}
		}
	}
	
}

//--------------------------------------------------------------

void testApp::setup(){
	 ofEnableAlphaBlending();

    
	partImage[0].loadImage("part1.png");
	partImage[1].loadImage("part2.png");
	partImage[2].loadImage("part3.png");
	partImage[3].loadImage("part4.png");
	partImage[4].loadImage("part5.png");
	partImage[5].loadImage("part6.png");
	partImage[6].loadImage("part7.png");
	partImage[7].loadImage("part8.png");
	partImage[8].loadImage("part9.png");
	partImage[9].loadImage("part10.png");
	partImage[10].loadImage("part11.png");
	

	
	canvas.allocate(ofGetWidth(), ofGetHeight()); 
	
	canvas.begin(); 
	glColor4f (1,1,1,1); 
	ofRect(0,0,ofGetWidth(), ofGetHeight()); 
	
	
	/*for (int i = 0; i < 100; i++) {
	 glColor4f(ofRandom(0, 1), ofRandom(0, 1), ofRandom(0, 1), 1);
	 ofRect(ofRandomWidth(),ofRandomHeight(), 20, 20); 
	 }*/
	canvas.end();
	
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	// enable depth->video image calibration
	kinect.setRegistration(true);
	
	
	kinect.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)
	kinect.open();
	
	// zero the tilt on startup
	angle = 0;
	kinect.setCameraTiltAngle(angle);
	
	//-----
	
	
	ofBackground(255, 255, 255);
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	
	//ballImage.loadImage("ball.png");
	
	width = ofGetWidth();
	height = ofGetHeight();
	
	//	physics.verbose = true;			// dump activity to log
	physics.setGravity(Vec3f(0, ofRandom(.01,.2), ofRandom(-.1,-GRAVITY)));
	
	// set world dimensions, not essential, but speeds up collision
	physics.setWorldSize(Vec3f(-width/2, -height, -width/2), Vec3f(width/2, height, width/2));
	physics.setSectorCount(SECTOR_COUNT);
	physics.setDrag(0.97f);
	physics.setDrag(1);		// FIXTHIS
	physics.enableCollision();
	
	initScene();
	
	// setup lighting
	
	GLfloat light_position[] = { 0, height/2, 0.0, 0.0 };
	glShadeModel(GL_FLAT);
	
	
	
	
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	
	glEnable(GL_COLOR_MATERIAL);
	
	
	//glTexEnv(GL_ADD); 
	glEnable(GL_LIGHT0);
	
	// enable back-face culling (so we can see through the walls)
	//glCullFace(GL_BACK);
	//glEnable(GL_CULL_FACE);
	
	
	gui.addTitle("My GUI");
	//gui.addToggle("myBool1 Animate", myBool1Animate);
	//gui.addSlider("myFloat1: noise", myFloat1, 0.0, 1); 
	gui.addSlider("Step", step, 10, 40); 
	gui.addSlider("Min Depth", min_depth, 1, 1000); 
	gui.addSlider("Max Depth", max_depth, 1001, 10000);
	gui.addSlider("Rotation Speed", rotSpeed, -2, 2);
	gui.addSlider("Radius", pRadius, 4, 200);
	gui.addSlider("MinBounce", minBounce, 0, 1.5);
	gui.addSlider("MaxBounce", maxBounce, .5, 2);
	//gui.addComboBox("box1", box1, 12, NULL);
	//gui.addButton("Randomize Background", randomizeButton);
	
	
	
	gui.addTitle("Stats").setNewColumn(true);
	gui.addFPSCounter();
	
	
	gui.show();
	gui.toggleDraw();
}


void testApp::update() {
	
	Tweener.update(); 
	
	
	if (rot > 59) {
		Tweener.removeAllTweens();
		rot = 58.9; 
		Tweener.addTween(rot, -60, 10, &ofxTransitions::easeInOutSine, 1);
	}
	
	if (rot < -59) {
		Tweener.removeAllTweens();
		rot = -58.9;
		Tweener.addTween(rot, 60, 10, &ofxTransitions::easeInOutSine, 1);
	}
	
	
	width = ofGetWidth();
	height = ofGetHeight();
	
	kinect.update(); 
	physics.update();
	
	if (drawLive) {
		
		updateMSAKinect();
		
		if(kinect.isFrameNew()) {
			
			
			for(int i=0; i<physics.numberOfParticles(); i++) {
				Physics::Particle3D *p = physics.getParticle(i);
				
				
				if (p->isFixed()) {
					
					
					//---------UPDATE OR KILL ----------------------
					//get live kinect depth at p.x and p.y
					float liveDepth = ( 
									   -ofMap(kinect.getDistanceAt( msaXToKinectX(p->getPosition().x), 
																   msaYToKinectY(p->getPosition().y)
																   ), min_depth, max_depth, -width/2, width/2)
									   );
					
					
					//if depth is different from p.z (Aka depth has changed since last update)
					
					if (liveDepth > (p->getPosition().z) + 75 || liveDepth < (p->getPosition().z) - 75) {
						
						//update particle depth (moveTo to preserve velocity) 
						if(kinect.getDistanceAt(msaXToKinectX(p->getPosition().x), msaYToKinectY(p->getPosition().y)) > min_depth && 
						   kinect.getDistanceAt(msaXToKinectX(p->getPosition().x), msaYToKinectY(p->getPosition().y)) < max_depth) {
							
							//keepers++;
							p->moveTo(Vec3f(p->getPosition().x,p->getPosition().y, liveDepth), false); 
						}
						
						
						//or kill any particles outside of the threshold. 
						else
						{
							//Physics::Particle3D *p = physics.getParticle(i);
							if(p && p->isFixed()) p->kill();
						}
						
					}
					
				}
			
				
				
				
				
				
			}
			
			
			
		}
		
	}
	
}


//--------------------------------------------------------------
void testApp::draw() {
	
	
	
	
	ofEnableAlphaBlending();
	glEnable(GL_DEPTH_TEST);
	glAlphaFunc(GL_GREATER, 0.5);
	
	glPushMatrix();
	
	glTranslatef(width/2, 0, -width / 3);		// center scene
	
	glRotatef(rot, 0, 1, 0);			// rotate scene
	rot += rotSpeed;						// slowly increase rotation (to get a good 3D view)
	
	
	
	
	ofEnableNormalizedTexCoords();
	
	
	canvas.getTextureReference().bind();  
  
	glBegin(GL_QUADS);
	
	glTexCoord2f (0.0, 1.0);
	glVertex3f(width/2, 0, -width/2);
	
	glTexCoord2f (0.0, 0.0);
	glVertex3f(width/2, height+1, -width/2);
	
	glTexCoord2f (1.0, 0.0);
	glVertex3f(-width/2, height+1, -width/2);
	
	glTexCoord2f (1.0, 1.0); 
	glVertex3f(-width/2, 0, -width/2);
	
	glEnd();
	
	canvas.getTextureReference().unbind();  
	
	
	// draw particles
	//glAlphaFunc(GL_ALWAYS, 0.5);
	
	
	for (int j = 0; j < 11; j++) { 
		
		//go through bind types
		
		partImage[j].getTextureReference().bind();
		
		for(int i=0; i<physics.numberOfParticles(); i++) {
         
			Physics::Particle3D *p = physics.getParticle(i);
			
			//glDisable(GL_ALPHA_TEST);
			
			
			glEnable(GL_ALPHA_TEST);
			
			if (p->getImage() == j) {
				
				
				
				
				if(p->isFixed()) 
					
					
					//color with live kinect colors
					glColor4f(
							  ofNormalize(kinect.getColorAt(msaXToKinectX(p->getPosition().x), msaYToKinectY(p->getPosition().y)).r, 0, 255),
							  ofNormalize(kinect.getColorAt(msaXToKinectX(p->getPosition().x), msaYToKinectY(p->getPosition().y)).g, 0, 255),
							  ofNormalize(kinect.getColorAt(msaXToKinectX(p->getPosition().x), msaYToKinectY(p->getPosition().y)).b, 0, 255),
							  1);
				
				
				else {
					  
					  
						  
					  p->setRadius(ofClamp(ofMap(myTimer.getSeconds(),0,2,pRadius + 70,1), 4, pRadius));
					
					
					if (p->getRadius() < 5) {
						p->kill();
                        //p->m; 
					}
					
				
					  
					
					  glColor4f(p->getRed(),p->getGreen(),p->getBlue(), 1);
					  
					  
				}
				
				glEnable(GL_ALPHA_TEST);
				
				// draw ball
				glPushMatrix();
				glTranslatef(p->getPosition().x, p->getPosition().y, p->getPosition().z);
				glRotatef(180-rot, 0, 1, 0);
				
				glBegin(GL_QUADS);
				glTexCoord2f(0, 0); glVertex2f(-p->getRadius(), -p->getRadius());
				glTexCoord2f(1, 0); glVertex2f(p->getRadius(), -p->getRadius());
				glTexCoord2f(1, 1); glVertex2f(p->getRadius(), p->getRadius());
				glTexCoord2f(0, 1); glVertex2f(-p->getRadius(), p->getRadius());
				glEnd();
				glPopMatrix();
				
				
				
				
				
				
				float alpha = ofMap(p->getPosition().y, -height * 1.5, height, 0, 1);
				if(alpha>0) {
					glPushMatrix();
					glTranslatef(p->getPosition().x, height, p->getPosition().z);
					glRotatef(-90, 1, 0, 0);
					glColor4f(0, 0, 0, alpha * alpha * alpha * alpha);
					//				ofCircle(0, 0, p->getRadius());
					float r = p->getRadius() * alpha;
					glBegin(GL_QUADS);
					glTexCoord2f(0, 0); glVertex2f(-r, -r);
					glTexCoord2f(1, 0); glVertex2f(r, -r);
					glTexCoord2f(1, 1); glVertex2f(r, r);
					glTexCoord2f(0, 1); glVertex2f(-r, r);
					glEnd();
					glPopMatrix();
					
					
				}
				
				glEnable(GL_ALPHA_TEST);
				glColor4f(1,1,1,1);
			    glDisable(GL_ALPHA_TEST);
	
			}
			
		}
		
		partImage[j].getTextureReference().unbind();
		
		
	}
	
	
	
	ofDisableNormalizedTexCoords();
	
	
	glPopMatrix(); 
	
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	
	
	
	for(int i=0; i<physics.numberOfParticles(); i++) {
		Physics::Particle3D *p = physics.getParticle(i);
		
		if (p->isFree() && p->getPosition().z < -width/2 + (pRadius + 25)) {
			canvas.begin();
			
			
			glColor4f(p->getRed(), p->getGreen(), p->getBlue(),1);
			
			partImage[p->getImage()].draw(convertPXtoCanvas(p->getPosition().x),convertPYtoCanvas(p->getPosition().y), p->getRadius(), p->getRadius()); 
			
			//ofRect(convertPXtoCanvas(p->getPosition().x),convertPZtoCanvas(p->getPosition().z), p->getRadius(), p->getRadius()); 
			canvas.end();
		}
		
	}
	
	glEnable(GL_ALPHA_TEST);
	glColor4f(1,1,1,1);
	glDisable(GL_ALPHA_TEST);
    
    kinect.draw(ofGetWindowWidth() - kinect.width/3 - 30,ofGetWindowHeight() - kinect.height/3 - 30,kinect.width/3, kinect.height/3); 
	
	if (gui.isOn()) {
		glColor4f(255, 0, 0, 1);
		drawString(     "Number of particles: " + ofToString(physics.numberOfParticles(), 2)
				   + " | Current rot: " + ofToString(rot, 2)
				   + " | Timer: " + ofToString(myTimer.getSeconds(), 2)
				   , 20, 15);
		gui.draw();
	}
	
	
	
	
}	





void testApp::exit () {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	switch(key) {
		//case 'a': toggleMouseAttract(); break;
		case 'p': for(int i=0; i<100; i++) addRandomParticle(); break;
		case 'P': for(int i=0; i<100; i++) killRandomParticle(); break;
		case 's': addRandomSpring(); break;
		case 'S': killRandomSpring(); break;
		case 'c': physics.isCollisionEnabled() ? physics.disableCollision() : physics.enableCollision(); break;
		case 'C': killRandomConstraint(); break;
		case 'r': doRender ^= true; break;
		case 'f': addRandomForce(FORCE_AMOUNT); break;
		case 'F': addRandomForce(FORCE_AMOUNT * 3); break;
		case 'l': lockRandomParticles(); break;
		case 'u': unlockRandomParticles(); break;
		case ' ': gui.toggleDraw(); break;
		case 'x': doMouseXY = true; break;
		case 'z': doMouseYZ = true; break;
		case ']': rotSpeed += 0.01f; break;
		case '[': rotSpeed -= 0.01f; break;
		//case '+': mouseNode.setMass(mouseNode.getMass() +0.1); break;
		//case '-': mouseNode.setMass(mouseNode.getMass() -0.1); break;
		case 'g': takeSnapshot(); myTimer.start(); physics.setGravity(Vec3f(0, 0, ofRandom(-.3,-1))) ;
			
			
			//glColor4f(1,1,1,1);
			
			/*canvas.begin();
			
			glColor4f (1,1,1,.5); 
			ofRect(0,0,ofGetWidth(), ofGetHeight()); 
			
			canvas.end();*/ 
			
			//glColor4f(1,1,1,1);
			
			
			
			break;
            
            case 'q': 
            canvas.begin();
            
            glColor4f (1,1,1,.5); 
            ofRect(0,0,ofGetWidth(), ofGetHeight()); 
            
            canvas.end();
            break; 
            
            
            
		case 't':	
			
			
			canvas.begin(); 
			glColor4f(ofRandom(0, 1), ofRandom(0, 1), ofRandom(0, 1), 1);
			ofRect(convertPXtoCanvas(0),convertPZtoCanvas(0), 20, 20); 
			canvas.end();
			
			/*
			 canvas.begin(); 
			 glColor4f(ofRandom(0, 1), ofRandom(0, 1), ofRandom(0, 1), 1);
			 ofRect(ofRandomWidth(),ofRandomHeight(), 20, 20); 
			 canvas.end();*/
			
			glEnable(GL_ALPHA_TEST);
			glColor4f(1,1,1,1);
			glDisable(GL_ALPHA_TEST);
			
			break;
			
			
		case 'y':	
			
			
			canvas.begin(); 
			glColor4f (1,1,1,1); 
			ofRect(0,0,ofGetWidth(), ofGetHeight()); 
			canvas.end();
			
			/*
			 canvas.begin(); 
			 glColor4f(ofRandom(0, 1), ofRandom(0, 1), ofRandom(0, 1), 1);
			 ofRect(ofRandomWidth(),ofRandomHeight(), 20, 20); 
			 canvas.end();*/
			
			glEnable(GL_ALPHA_TEST);
			glColor4f(1,1,1,1);
			glDisable(GL_ALPHA_TEST);
			
			break;
			
			
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;
			
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
            


			
	}
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){
	switch(key) {
		case 'x': doMouseXY = false; break;
		case 'z': doMouseYZ = false; break;
	}
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ) {
	static int oldMouseX = -10000;
	static int oldMouseY = -10000;
	int velX = x - oldMouseX;
	int velY = y - oldMouseY;
	//if(doMouseXY) mouseNode.moveBy(Vec3f(velX, velY, 0));
	//if(doMouseYZ) mouseNode.moveBy(Vec3f(velX, 0, velY));
	oldMouseX = x;
	oldMouseY = y;
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	switch(button) {
		case 0:	doMouseXY = true; mouseMoved(x, y); break;
		case 2: doMouseYZ = true; mouseMoved(x, y); break;
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(){
	doMouseXY = doMouseYZ = false;
}
