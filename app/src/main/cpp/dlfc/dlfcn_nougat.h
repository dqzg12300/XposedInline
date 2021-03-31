#ifndef DLFCN_NOUGAT_H
#define DLFCN_NOUGAT_H
#define ANDROID_K 19
#define ANDROID_L 21
#define ANDROID_L2 22
#define ANDROID_M 23
#define ANDROID_N 24
#define ANDROID_N2 25
#define ANDROID_O 26
#define ANDROID_O2 27
#define ANDROID_P 28
#define ANDROID_Q 29
#define ANDROID_R 30
//see implementation in https://tech.meituan.com/2017/07/20/android-remote-debug.html
extern "C" {
int fake_dlclose(void *handle);

void *fake_dlopen(const char *filename, int flags);

void *fake_dlsym(void *handle, const char *name);

const char *fake_dlerror();

void *getSymCompat(const char *filename, const char *name);
}

#endif //DLFCN_NOUGAT_H
