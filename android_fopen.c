#include <stdio.h>

#include "android_fopen.h"
#include <errno.h>
#include <android/asset_manager.h>
#include <android/log.h>
#include <jni.h>


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,"android_fopen", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "android_fopen", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"android_fopen", __VA_ARGS__))


#define EXPORT_ME __attribute__ ((visibility("default")))

void EXPORT_ME Java_com_beloko_AssetFileAccess_setAssetManager(JNIEnv *env, jobject obj,
        jobject assetManager)
{
   AAssetManager *mgr = (AAssetManager *)AAssetManager_fromJava(env, assetManager);
   android_fopen_set_asset_manager(mgr);
}

static int android_read(void* cookie, char* buf, int size) {
// LOGI("android_read %p %d",cookie,size);

  return AAsset_read((AAsset*)cookie, buf, size);
}

static int android_write(void* cookie, const char* buf, int size) {
  return EACCES; // can't provide write access to the apk
}

static fpos_t android_seek(void* cookie, fpos_t offset, int whence) {
  return AAsset_seek((AAsset*)cookie, offset, whence);
}

static int android_close(void* cookie) {
  AAsset_close((AAsset*)cookie);
  return 0;
}

// must be established by someone else...
AAssetManager* android_asset_manager;
void android_fopen_set_asset_manager(AAssetManager* manager) {
  android_asset_manager = manager;
}

FILE* android_fopen(const char* fname, const char* mode) {

  LOGI("android_fopen - %s",fname);

  if(mode[0] == 'w') return NULL;

  AAsset* asset = AAssetManager_open(android_asset_manager, fname, 0);
  if(!asset)
  {
    LOGE("android_fopen error opening: %s",fname);
    return NULL;
  }

  return funopen(asset, android_read, android_write, android_seek, android_close);
}