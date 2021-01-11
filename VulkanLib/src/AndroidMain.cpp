#ifndef WINDOWS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "defs.h"
#include "Window.h"
#include "VulkanSetup.h"
#include "android_native_app_glue.h"

void exit(int status); // --> nicht erforderlich für Kompilierung, aber VS zeigt sonst Fehler an

#define APP_NAME "VulkanApp"
#define ENGINE_NAME "MyVulkanEngine"
#define WINDOW_NAME "My Vulkan App"

#define WND_WIDTH 2560
#define WND_HEIGHT 1600
//#define LOGFILE "/storage/emulated/0/Dokumente/VulkanApp.log.txt"
#define LIB_NAME "libvulkan.so"
static bool initialized_ = false;

// TODO: zu userdata hinzufügen
static FILE* logfile = NULL;
bool* key;
MotionPos* motionPos;

class App
{
	const char *appName = APP_NAME;
	const char *engineName = ENGINE_NAME;
	const char *resourcesPath;
	const char *libName = LIB_NAME;
	MyWindow window;
	uint32_t framecount = 0;
	uint32_t fps = 0;
	timespec tStart, tEnd;
	clock_t start_t;

#ifdef DYNAMIC
	VulkanSetup* (*create_object)(const char* _appName, const char* _engineName, const char* _libName, FILE* _logfile);
#endif
	VulkanSetup *vkSetup;
	RenderScene *renderScene;
public:
	App(android_app* a_app)
	{
		resourcesPath = a_app->activity->internalDataPath;
		char dirPath[128];
		char assetPath[64];
		struct stat st = { 0 };
		int assetDirCount = 3;
		const char* assetDirs[] = { "shader","textures","3dmodels" };

		PRINT("Start App.\n")		

		for (int i = 0; i < assetDirCount; i++) {
			AAssetDir* assetDir = AAssetManager_openDir(a_app->activity->assetManager, assetDirs[i]);
			if (assetDir) {
				PRINT("Open asset directory: %s\n", assetDirs[i])
				sprintf(dirPath, "%s/%s", a_app->activity->internalDataPath, assetDirs[i]);
				if (stat(dirPath, &st) == -1) {
					if (mkdir(dirPath, 0777) == -1) {
						PRINT("Couldn't create '%s' directory!\n", assetDirs[i]);
					}
					else {						
						while (const char* fileName = AAssetDir_getNextFileName(assetDir)) {
							sprintf(assetPath, "%s/%s", assetDirs[i], fileName);
							AAsset* assetFile = AAssetManager_open(a_app->activity->assetManager, assetPath, AASSET_MODE_BUFFER);
							if (assetFile) {
								PRINT("Read %s\n", assetPath)
								size_t fileLength = AAsset_getLength(assetFile);
								char* fileContent = new char[fileLength];
								AAsset_read(assetFile, fileContent, fileLength);
								AAsset_close(assetFile);
								sprintf(dirPath, "%s/%s/%s", a_app->activity->internalDataPath, assetDirs[i], fileName);
								FILE* internalDataFile = fopen(dirPath, "wb");
								if (internalDataFile) {
									PRINT("Copy to %s\n", dirPath)
									fwrite(fileContent, fileLength, 1, internalDataFile);
									fclose(internalDataFile);
								}
								else {
									PRINT("Coudn't write File to %s!", dirPath);
								}
							}
							else {
								PRINT("Couldn't open %s!\n", assetPath)
							}
						}
						AAssetDir_close(assetDir);
					}
				}
				else {
					PRINT("%s already exists!\n", dirPath)
				}
			}
			else {
				PRINT("Couldn't open asset directory: %s!\n", assetDirs[i])
			}
		}

		AAsset* shaderFile = AAssetManager_open(a_app->activity->assetManager, "3dmodels/cube.x", AASSET_MODE_BUFFER);
		if (shaderFile) {
			PRINT("Juhu!!!\n")
			size_t fileLength = AAsset_getLength(shaderFile);

			char* fileContent = new char[fileLength];

			AAsset_read(shaderFile, fileContent, fileLength);
			AAsset_close(shaderFile);

			PRINT("fileLength = %zu\n", fileLength);
			fileContent[fileLength] = 0;
			PRINT("%s\n", fileContent);
		}
		else {
			PRINT("Scheiße!!!\n")
		}
#ifdef DYNAMIC
		void* libVulkan = dlopen("./libVulkan.so", RTLD_LAZY);
		if (!libVulkan)
		{
			PRINT("Loading libVulkan.so failed!\n")
			exit(1);
		}
		create_object = (VulkanSetup * (*)(const char* _appName, const char* _engineName, const char* _libName, FILE * _logfile))dlsym(libVulkan, "create_object");
		if (!create_object)
		{
			PRINT("Find Symbol create_object failed!\n")
			exit(1);
		}
		vkSetup = create_object(appName, engineName, libName, logfile);
#else
		vkSetup = new VulkanSetup(appName, engineName, libName, logfile);
#endif
	}

	~App()
	{
		delete vkSetup;
		PRINT("Close App.\n")
#ifdef LOG
		fclose(logfile);
#endif
	}

	void init(MyWindow _window)
	{
		window = _window;
		vkSetup->init(window);
		renderScene = new RenderScene(vkSetup, key, motionPos, resourcesPath);
		start_t = clock();
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tStart);
	}

	void cleanUpVulkan() {
		vkSetup->cleanUp();
	}

	void draw()
	{
		framecount++;
		renderScene->updateUniformBuffers();
		renderScene->camMotion();
		renderScene->drawFrame();
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tEnd);
		while (((tEnd.tv_sec - tStart.tv_sec) * (long)1e9 + (tEnd.tv_nsec - tStart.tv_nsec)) < (long)(1000000000/60)) clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tEnd);
		if ((clock() - start_t) > CLOCKS_PER_SEC)
		{
			fps = framecount;
			start_t = clock();
			framecount = 0;
		}
		renderScene->updateTextOverlay(fps, motionPos->xScreen, motionPos->yScreen);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tStart);
	}
};

static int32_t handle_input(struct android_app* app, AInputEvent* event) {

	float down_x_new = 0.0f;
	float down_y_new = 0.0f;

	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
		
		size_t pointerCount = AMotionEvent_getPointerCount(event);
		PRINT("pointerCount = %zu\n", pointerCount);
		int32_t action = AMotionEvent_getAction(event);
		unsigned int flags = action & AMOTION_EVENT_ACTION_MASK;
		switch (flags) {
			case AMOTION_EVENT_ACTION_DOWN:
				PRINT("Action Down\n")				
				motionPos->down_x = AMotionEvent_getX(event, 0);
				motionPos->down_y = AMotionEvent_getY(event, 0);
				break;
			case AMOTION_EVENT_ACTION_MOVE:
				PRINT("Action Move\n")
				down_x_new = AMotionEvent_getX(event, 0);
				down_y_new = AMotionEvent_getY(event, 0);
				if (pointerCount == 1) {					
					motionPos->delta_x = down_x_new - motionPos->down_x;
					motionPos->delta_y = down_y_new - motionPos->down_y;					
				}
				else if (pointerCount == 2) {
					motionPos->delta_x_2 = down_x_new - motionPos->down_x;
					motionPos->delta_y_2 = down_y_new - motionPos->down_y;					
				}
				motionPos->down_x = down_x_new;
				motionPos->down_y = down_y_new;
				PRINT("delta_x = %f\n", motionPos->delta_x)
				PRINT("delta_y = %f\n", motionPos->delta_y)
				break;
			case AMOTION_EVENT_ACTION_UP:
				PRINT("Action Up\n")				
				motionPos->delta_x = 0.0f;
				motionPos->delta_y = 0.0f;
				motionPos->delta_x_2 = 0.0f;
				motionPos->delta_y_2 = 0.0f;
				int64_t eventTime = AMotionEvent_getEventTime(event);
				PRINT("eventTime = %ld\n", eventTime);
				int64_t downTime = AMotionEvent_getDownTime(event);
				PRINT("downTime = %ld\n", downTime);
				break;
		}

		motionPos->xScreen = AMotionEvent_getX(event, 0);
		motionPos->yScreen = AMotionEvent_getY(event, 0);
		
		/*
		if (motionPos->xScreen < 200) key[KEY_LEFT] = true; else key[KEY_LEFT] = false;
		if (motionPos->xScreen > 2000) key[KEY_RIGHT] = true; else key[KEY_RIGHT] = false;

		if (motionPos->yScreen < 200) key[KEY_UP] = true; else key[KEY_UP] = false;
		if (motionPos->yScreen > 1300) key[KEY_DOWN] = true; else key[KEY_DOWN] = false;

		PRINT("Position: %d,%d\n", motionPos->xScreen, motionPos->yScreen);
		*/
		return 1;
	}
	return 0;
}

// Process the next main command.
void handle_cmd(android_app* a_app, int32_t cmd)
{
	switch (cmd)
	{
		case APP_CMD_INIT_WINDOW:
			// The window is being shown, get it ready.
			((App*)a_app->userData)->init((MyWindow)a_app->window);
			initialized_ = true;
			key[KEY_W] = true;
			break;
		case APP_CMD_TERM_WINDOW:
			// The window is being hidden or closed, clean it up.
			PRINT("Window terminated.\n")
			((App*)a_app->userData)->cleanUpVulkan();
			initialized_ = false;
			break;
#ifdef LOG
		default:
#endif
			PRINT("Event not handled: %d\n", cmd)
	}
}

jstring getPermissionName(JNIEnv* env, const char* name) {
	jclass manifestpermission = env->FindClass("android/Manifest$permission");
	jfieldID fieldID = env->GetStaticFieldID(manifestpermission, name, "Ljava/lang/String;");
	jstring jPermissionName = (jstring)env->GetStaticObjectField(manifestpermission, fieldID);
	const char*  permissionName = env->GetStringUTFChars(jPermissionName, NULL);
	PRINT("permissionName: %s\n", permissionName)
	return jPermissionName;
}

bool checkPermission(JNIEnv* env, jobject activity, const char* name) {
	jint permissionGranted = -1;
	jclass packageManager = env->FindClass("android/content/pm/PackageManager");
	jfieldID fieldID = env->GetStaticFieldID(packageManager, "PERMISSION_GRANTED", "I");	
	permissionGranted = env->GetStaticIntField(packageManager, fieldID);
	jclass contentContext = env->FindClass("android/content/Context");
	jmethodID checkSelfPermission = env->GetMethodID(contentContext, "checkSelfPermission", "(Ljava/lang/String;)I");
	jint result = env->CallIntMethod(activity, checkSelfPermission, getPermissionName(env, name));
	PRINT("permissionGranted = %d\n", result)
	return (result == permissionGranted);
}

void requestPermissions(JNIEnv* env, jobject activity) {
	PRINT("Request Permissions\n")
	jobjectArray objArray = env->NewObjectArray(2, env->FindClass("java/lang/String"), env->NewStringUTF(""));
	env->SetObjectArrayElement(objArray, 0, getPermissionName(env, "WRITE_EXTERNAL_STORAGE"));
	env->SetObjectArrayElement(objArray, 1, getPermissionName(env, "READ_EXTERNAL_STORAGE"));
	jclass classActivity = env->FindClass("android/app/Activity");
	jmethodID requestPermissions = env->GetMethodID(classActivity, "requestPermissions", "([Ljava/lang/String;I)V");
	env->CallVoidMethod(activity, requestPermissions, objArray, 0);
}

int  checkAndroidPermissions(android_app* a_app) {
	PRINT("Check Permissions\n")
	JavaVM* vm = a_app->activity->vm;
	JNIEnv* env = a_app->activity->env;
	vm->AttachCurrentThread(&env, NULL);
	jobject activity = a_app->activity->clazz;
	
	//checkPermission(env, activity, "CAMERA");

	if ( !(checkPermission(env, activity, "WRITE_EXTERNAL_STORAGE") && checkPermission(env, activity, "READ_EXTERNAL_STORAGE")) ) {
		requestPermissions(env, activity);
		//while (1); --> TODO: onRequestPermissionsResult() - Callback implementieren
	}
	vm->DetachCurrentThread();
	return 0;
}

void android_main(struct android_app* a_app)
{
	char externalDataPath[128];
	App* app;
	time_t current_time;
	char* c_time_string;

	current_time = time(NULL);
	c_time_string = ctime(&current_time);

#ifdef LOG
	sprintf(externalDataPath, "%s/VulkanApp.log.txt", a_app->activity->externalDataPath);
	logfile = fopen(externalDataPath, "w");
	if (logfile == NULL) return;
#endif

	PRINT("\n==================\n*** Vulkan App ***\n==================\n\n")
	PRINT("%s\n", c_time_string)

	if (checkAndroidPermissions(a_app)) return;

	app = new App(a_app);
	key = new bool[256]; // --> wird hier alles mit 0 initialisiert?
	memset(key, 0, sizeof(bool)*256);
	motionPos = new MotionPos;
	memset(motionPos, 0, sizeof(MotionPos));

	a_app->userData = app;

	// Set the callback to process system events
	a_app->onAppCmd = handle_cmd;
	a_app->onInputEvent = handle_input;

	// Used to poll the events in the main loop
	int events;
	android_poll_source* source;

	// Main loop
	do {
		if (ALooper_pollAll(initialized_ ? 1 : 0, nullptr, &events, (void**)&source) >= 0) {
			if (source != NULL) source->process(a_app, source);
		}

		// render if vulkan is ready
		if (initialized_) app->draw();

	} while (a_app->destroyRequested == 0);

	delete app;
}

#endif // !WINDOWS