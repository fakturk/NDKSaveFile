#include <jni.h>
#include <string>
#include <android/sensor.h>
#include <android/log.h>
#include <time.h>
#include <android/looper.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "accNative", __VA_ARGS__))

ASensorEventQueue* sensorEventQueue;
jobject g_object;
static JavaVM *jvm;
static int get_sensor_events(int fd, int events, void* data);

static void sendData(int64_t timestamp, float x, float y, float z);

static void sendData(JNIEnv *pEnv, jobject pJobject, jmethodID pID, int i, double d, double d1,
                     double d2);

bool sensorPrint=true;


extern "C"
jstring
Java_netlab_fakturk_ndksavefile_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
void Java_netlab_fakturk_ndksavefile_MainActivity_sensorValue(JNIEnv* env, jobject thiz)
{
    ASensorEvent event;
    int events, ident;
    ASensorManager* sensorManager;
    const ASensor* accSensor;
    void* sensor_data = malloc(1000);



// Identify the looper associated with the calling thread, or create one if it does not exist.
// A looper is a message loop for a thread and will handle the sensor event callbacks.

    ALooper* looper = ALooper_forThread();

    if(looper == NULL)
    {
        looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
    }

    sensorManager = ASensorManager_getInstance();

    accSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_ACCELEROMETER);

    int status = (*env).GetJavaVM(&jvm);

    //setting global object in order to use in callback function
    g_object = env->NewGlobalRef(thiz);


    jclass clazz = (*env).GetObjectClass(thiz);
    if (clazz == NULL) {
       LOGI("Failed to find class" );
    }
    jmethodID writeData = env->GetMethodID(clazz, "writeData","(JFFF)V");
    env->CallVoidMethod(thiz, writeData,1,2.0,3.0,4.0);
    sendData(env, thiz, writeData, 5,6.0,7.0,8.0);

    sensorEventQueue = ASensorManager_createEventQueue(sensorManager, looper, 3, get_sensor_events, sensor_data);
    ASensorEventQueue_enableSensor(sensorEventQueue, accSensor);
    int a = ASensor_getMinDelay(accSensor);
    LOGI("min-delay: %d",a);
    ASensorEventQueue_setEventRate(sensorEventQueue, accSensor, 10000);

}

static void sendData(JNIEnv *pEnv, jobject pJobject, jmethodID pID, int i, double d, double d1,
                     double d2) {
    pEnv->CallVoidMethod(pJobject,pID,i,d,d1,d2);

}

extern "C"
void Java_netlab_fakturk_ndksavefile_MainActivity_stopSensorPrint(JNIEnv* env, jobject thiz)
{
    sensorPrint=false;
}
extern "C"
void Java_netlab_fakturk_ndksavefile_MainActivity_startSensorPrint(JNIEnv* env, jobject thiz)
{
    sensorPrint=true;
}


static int get_sensor_events(int fd, int events, void* data)
{
    ASensorEvent event;
    JNIEnv *env;
    (*jvm).AttachCurrentThread(&env,NULL);
    jclass clazz = (*env).GetObjectClass(g_object);
    jmethodID writeData = env->GetMethodID(clazz, "writeData","(JFFF)V");



    while (ASensorEventQueue_getEvents(sensorEventQueue, &event, 1) > 0) {
        if (event.type == ASENSOR_TYPE_ACCELEROMETER) {
            if (sensorPrint)
            {
//                sendData(event.timestamp, event.acceleration.x, event.acceleration.y, event.acceleration.z);
                env->CallVoidMethod(g_object, writeData,event.timestamp, event.acceleration.x, event.acceleration.y, event.acceleration.z);
//                LOGI(" %lld %f %f %f ", event.timestamp, event.acceleration.x, event.acceleration.y, event.acceleration.z);
            }

        }
    }

    //should return 1 to continue receiving callbacks, or 0 to unregister

    return 1;
}

static void sendData(int64_t timestamp, float x, float y, float z) {
//    sendData(timestamp,x,y,z);

}

