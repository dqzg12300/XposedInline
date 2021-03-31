//
// Created by Administrator on 2019/10/15.
//

#ifndef VMP_LOG_H
#define VMP_LOG_H


#include <android/log.h>

#define TAG "hide"


#define LOGG(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__);



#endif //VMP_LOG_H
