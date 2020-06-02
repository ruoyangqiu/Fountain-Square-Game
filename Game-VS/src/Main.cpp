#include <iostream>
#include <stdlib.h> // rand()
#include <memory>   // shared_ptr
#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Game-Engine/GameObject.h"
#include "Game-Engine/Shader.h"
#include "Game-Engine/Model.h"
#include "Game-Engine/LOGL_Camera.h"
#include "Audio-Engine/AudioEngine.h"
#include "Audio-Engine/FootstepController.h"
#include "Audio-Engine/CoinSoundController.h"
#include "GameData.h"
// custom game objects
#include "Game-Engine/Bird.h"
#include "Game-Engine/Harp.h"
#include "Game-Engine/AsteroidRing.h"
#include "Game-Engine/Coin.h"

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void ProcessInput(GLFWwindow* window);


// Character/camera data
struct CharacterInventory {
	int numCoins = 0;


};

CharacterCamera camera(STARTING_PLAYER_LOCATION);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// List for all game objects
std::vector<GameObject*> gameObjects;
std::vector<Animation*> animationObjects;
std::vector<InstancedObject*> instancedObjects;
//std::vector<AABB*> aabbObjects;
std::vector<Coin*> coins;

// timing globals
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float currentFrame = 0.0f;

// audio engine
// Use make_shared function when possible.
std::shared_ptr<AudioEngine> audioEngine;// = make_shared<AudioEngine>(L"The Beatles", L"Im Happy Just to Dance With You");
FootstepController* footstepController;
CoinSoundController* coinSoundController;


static glm::mat4 getProjection() {
	return glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
}

// Performs the OpenGL calls to render a GameObject with a provided shader.
static void renderGameObject(GameObject &gameObject, Shader* shader) {
	// enable shader before setting uniforms
	shader->use();
	// view/projection transformations
	glm::mat4 projection = getProjection();
	glm::mat4 view = camera.GetViewMatrix();
	shader->setMat4("projection", projection);
	shader->setMat4("view", view);
	// render the loaded model
	shader->setMat4("model", gameObject.getModel());
	gameObject.draw(shader);
}


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Fountain Game", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	// tell GLFW what to do with mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	Shader gameObjectShader("res/shaders/1.model_loading.vs", "res/shaders/1.model_loading.fs");
	Shader* instancedObjectShader = new Shader("res/shaders/instanced_model_loading.vs", "res/shaders/instanced_model_loading.fs");

	/*
		Initialize game objects and add to list
	*/

	GameObject* fountain = new GameObject(OBJ_FOUNTAIN, tranFountain, scaleFountain, rotFountain);
	GameObject* house = new GameObject(OBJ_HOUSE, tranHouse, scaleHouse, rotHouse);
	GameObject* rock = new GameObject(OBJ_ROCK, tranRock, scaleRock, rotRock);
	GameObject* ground = new GameObject(OBJ_GROUND, tranGround, scaleGround, rotGround);
	GameObject* grass = new GameObject(OBJ_GRASS, tranGrass, scaleGrass, rotGrass);
	GameObject* cooltree = new GameObject(OBJ_COOLTREE, tranCooltree, scaleCooltree, rotCooltree);
	GameObject* oak = new GameObject(OBJ_OAK, tranPine, scalePine, rotPine);
	GameObject* house2 = new GameObject(OBJ_HOUSE2, tranHouse2, scaleHouse2, rotHouse2);
	//GameObject* japaneseTree = new GameObject(OBJ_JAPANESE_TREE, tranJapaneseTree, scaleJapaneseTree, rotJapaneseTree);
	GameObject* cottage = new GameObject(OBJ_COTTAGE, tranCottage, scaleCottage, rotCottage);
	GameObject* willowtree = new GameObject(OBJ_WILLOWTREE, tranWillowtree, scaleWillowtree, rotWillowtree);
	GameObject* well = new GameObject(OBJ_WELL, tranWell, scaleWell, rotWell);
	GameObject* townhouse = new GameObject(OBJ_TOWNHOUSE, tranGreenPine, scaleGreenPine, rotGreenPine);
	GameObject* fir1 = new GameObject(OBJ_OAK, tranfir1, scalefir1, rotfir1);
	GameObject* fir2 = new GameObject(OBJ_OAK, tranfir2, scalefir2, rotfir2);
	GameObject* fir3 = new GameObject(OBJ_OAK, tranfir3, scalefir3, rotfir3);
	GameObject* house3 = new GameObject(OBJ_HOUSE3, tranHouse, scaleHouse, rotHouse);
	//GameObject* house4 = new GameObject(OBJ_HOUSE4, tranHouse4, scaleHouse4, rotHouse4);
	GameObject* japaneseTree2 = new GameObject(OBJ_JAPANESE_TREE2, tranJapaneseTree2, scaleJapaneseTree2, rotJapaneseTree2);
	GameObject* fir4 = new GameObject(OBJ_BUSH, tranfir4, scalefir4, rotfir4);

	// List for all game objects
	gameObjects.push_back(fountain);
	gameObjects.push_back(house3);
	//gameObjects.push_back(house4);
	gameObjects.push_back(ground);
	gameObjects.push_back(rock);
	gameObjects.push_back(grass);
	gameObjects.push_back(cooltree);
	gameObjects.push_back(oak);
	gameObjects.push_back(house2);
	//gameObjects.push_back(japaneseTree);
	gameObjects.push_back(cottage);
	gameObjects.push_back(willowtree);
	gameObjects.push_back(well);
	gameObjects.push_back(townhouse);
	gameObjects.push_back(japaneseTree2);
	gameObjects.push_back(fir1);
	gameObjects.push_back(fir2);
	gameObjects.push_back(fir3);
	gameObjects.push_back(fir4);

		
    /*
        Initialize and store animatable game objects 
    */

    Bird* birds = new Bird(OBJ_BIRDS, tranBirds, scaleBirds, rotBirds);
    Harp* harp  = new Harp(OBJ_HARP, tranHarp, scaleHarp, rotHarp);

    gameObjects.push_back(birds);
    gameObjects.push_back(harp);

    // add to list of animation objects which need to be updated each frame
    animationObjects.push_back(birds);
    animationObjects.push_back(harp);
	
    // Load all animated coins and add to game objects, animation objects, and coin list
	for (auto trans : coinTranslations) {
		Coin* coin = new Coin(OBJ_COIN, trans, scaleCoins, rotCoins);
        gameObjects.push_back(coin);
        animationObjects.push_back(coin);
        coins.push_back(coin);
	}


	for (auto gameObject : gameObjects) {
		gameObject->setScale(gameObject->getScale() * GLOBAL_SCALE);
		gameObject->setTranslation(gameObject->getTranslation()* GLOBAL_POSITION_SCALE);
	}
	/*
		Initialize instanced game objects
	*/

	AsteroidRing* asteroidRing = new AsteroidRing(OBJ_ROCK, instancedObjectShader);
	instancedObjects.push_back(asteroidRing);
	animationObjects.push_back(asteroidRing);




	/*
		AUDIO ENGINE and SOUND LOADING
	*/
	
	// Initialize Audio Engine
	audioEngine = std::make_shared<AudioEngine>();
	audioEngine->init();
	// load sounds
	audioEngine->loadSound(soundOneShot);
	audioEngine->loadSound(musicLoop2d);
	audioEngine->loadSound(soundOneShot3D);
	audioEngine->loadSound(soundLoop3D);
	audioEngine->loadSound(soundLoop3DMoving);
	audioEngine->loadSound(soundTree);
	audioEngine->loadSound(soundJapaneseTree);
	audioEngine->loadSound(soundCoinPickup);
	audioEngine->loadSound(soundCoinSuccess);
	// load FMOD soundbanks
	audioEngine->loadFMODStudioBank(FMOD_SOUNDBANK_MASTER);
	audioEngine->loadFMODStudioBank(FMOD_SOUNDBANK_MASTER_STRINGS);
	audioEngine->loadFMODStudioBank(FMOD_SOUNDBANK_SFX);
	// load FMOD event and its parameters
	audioEngine->loadFMODStudioEvent(FMOD_EVENT_CHARACTER_FOOTSTEPS, PARAM_CHARACTER_FOOTSTEPS_SURFACE);
	audioEngine->loadFMODStudioEvent(FMOD_EVENT_2D_LOOP_COUNTRY_AMBIENCE);
	audioEngine->loadFMODStudioEvent(FMOD_EVENT_2D_ONESHOT_EXPLOSION);
	// set Event Parameters
	audioEngine->setEventVolume(FMOD_EVENT_CHARACTER_FOOTSTEPS, 0.4f);
	// setup sound event controllers
	footstepController = new FootstepController(audioEngine);
	coinSoundController = new CoinSoundController(audioEngine, coins.size());
	/*
		play inital soundscape
	*/
	audioEngine->playSound(soundLoop3D);
	//audioEngine->playSound(musicLoop2d);
	audioEngine->playSound(soundTree);
	audioEngine->playSound(soundJapaneseTree);
    
    // draw in wireframe mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    /* render loop */ 
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        
        // input
        // -----
        ProcessInput(window);

        // render
        // ------
        glClearColor(COLOR_SKY.x, COLOR_SKY.y, COLOR_SKY.z, COLOR_SKY.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // enable blended overwrite of color buffer
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        

        // do collision detection
        for (auto coin : coins) {
            if (!coin->isDestroyed()) {
                if (coin->collides(camera)) {
                    coin->setDestroyed(true);
					coinSoundController->characterPickedUpCoin();
					// TODO move below sound-playing logic into CoinSoundController
					if (coinSoundController->nCharacterCoins == coinSoundController->nTotalCoins)
						audioEngine->playSound(soundCoinSuccess);
					else
						audioEngine->playSound(soundCoinPickup);
                }
            }
        }

        // update animation objects with current frame
        for (int i = 0; i < animationObjects.size(); i++)
            animationObjects[i]->update(currentFrame);
        
        //glDisable(GL_DEPTH_TEST);

        // render Game Objects
        for (int i = 0; i < gameObjects.size(); i++) 
            renderGameObject(*gameObjects[i], &gameObjectShader);
        
        // render instanced objects
        for(InstancedObject *instancedObject : instancedObjects)
            instancedObject->drawInstances(getProjection(), camera.GetViewMatrix());


        /*
            Audio Engine per-frame updates
        */
		audioEngine->update(); // per-frame FMOD update

        // set current player position
        audioEngine->set3DListenerPosition(camera.Position.x, camera.Position.y, camera.Position.z,
                                          camera.Front.y,    camera.Front.x,    camera.Front.z,
                                          camera.Up.y,       camera.Up.x,       camera.Up.z );
        
        // Update location of 3D sounds
        //glm::vec3 newBirdTran = birds->getTranslation();
        //audioEngine->update3DSoundPosition(SFX_LOOP_BIRD, newBirdTran.x, newBirdTran.y, newBirdTran.z);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// audio timing
float key1LastTime = 0.0f, key2LastTime = 0.0f, key3LastTime = 0.0f, key4LastTime = 0.0f, key5LastTime = 0.0f, key0LastTime = 0.0f;
float MIN_SOUND_KEY_RETRIGGER_TIME = 0.2f;

// Checks if a key can be triggered depending on how recently it was last triggered. 
// If true sets the key's most recent trigger time to current frame
bool keyCanRetrigger(float &currFrame, float &lastTriggerFrame) {
	bool canRetrigger = currFrame - lastTriggerFrame >= MIN_SOUND_KEY_RETRIGGER_TIME;
	if (canRetrigger) lastTriggerFrame = currFrame;
	return canRetrigger;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// WASD Handling (Character Movement)
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
		footstepController->processFootstepKey(currentFrame);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
		footstepController->processFootstepKey(currentFrame);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
		footstepController->processFootstepKey(currentFrame);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
		footstepController->processFootstepKey(currentFrame);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		camera.ProcessKeyboard(RUNNING_START, deltaTime);
		footstepController->setRunning(true);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
		camera.ProcessKeyboard(RUNNING_STOP, deltaTime);
		footstepController->setRunning(false);
	}

	// Number Keys: Audio Controls
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && keyCanRetrigger(currentFrame, key1LastTime))
		audioEngine->playSound(soundOneShot);
	
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && keyCanRetrigger(currentFrame, key2LastTime))
		audioEngine->soundIsPlaying(musicLoop2d) ? audioEngine->stopSound(musicLoop2d) : audioEngine->playSound(musicLoop2d);
	
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && keyCanRetrigger(currentFrame, key3LastTime))
		audioEngine->playSound(soundOneShot3D);
	
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && keyCanRetrigger(currentFrame, key4LastTime)) 
		audioEngine->soundIsPlaying(soundLoop3D) ? audioEngine->stopSound(soundLoop3D) : audioEngine->playSound(soundLoop3D);
	
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && keyCanRetrigger(currentFrame, key5LastTime))
		audioEngine->eventIsPlaying(FMOD_EVENT_2D_LOOP_COUNTRY_AMBIENCE) ? 
				audioEngine->stopEvent(FMOD_EVENT_2D_LOOP_COUNTRY_AMBIENCE):
				audioEngine->playEvent(FMOD_EVENT_2D_LOOP_COUNTRY_AMBIENCE);
	
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS && keyCanRetrigger(currentFrame, key0LastTime))
		audioEngine->isMuted() ? 
				audioEngine->unmuteAllSound():
				audioEngine->muteAllSounds();

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}