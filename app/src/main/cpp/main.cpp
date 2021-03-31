

#include "main.h"
#include <SubstrateHook.h>
#include "utils/hide.h"
#include <android/log.h>
#include <fcntl.h>
#include <sys/vfs.h>
#include <dobby.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sstream>
#include "fix.h"
#define BYTE_POINT sizeof(void*)
#define LOGME(...) __android_log_print(ANDROID_LOG_DEBUG, "hide", __VA_ARGS__);
#define LOGERROR(...) __android_log_print(ANDROID_LOG_ERROR, "hide", __VA_ARGS__);
char* outTmpPath = "/data/data/com.example.ceair/out.tmp.so";
char* outPath = "/data/data/com.example.ceair/out.so";
char* solibname = "vmpso";
const  char* so_names[2];
int SDK_INT = 0;
static unsigned long get_rx_address(const char *name,uint32_t pid){
    char *lpCh = NULL;
    char bufMaps[256] = "/proc/self/maps";
    char szLines[1024] = {0};
    unsigned long start = 0;
    unsigned  long tmp=0;


    if (pid != 0) {
        sprintf(bufMaps, "/proc/%d/maps", pid);
    }
    FILE *fp = fopen(bufMaps, "r");
    if (fp != NULL)
    {
        while (fgets(szLines, sizeof(szLines), fp))
        {
            if (strstr(szLines,name)&&strstr(szLines,"r-xp"))
            {

                LOGME("Find=%s",szLines);
                lpCh = strtok(szLines, "-");
                tmp=strtoul(lpCh, NULL, 16);
                if((start> tmp|| start ==0 )){
                    start=strtoul(lpCh, NULL, 16);
                }

            }
        }
        fclose(fp);
        return start;
    }
    else
    {
        LOGME("fopen error\r\n");
    }
    return -1;

}
static unsigned long get_rwx_address(const char *name,uint32_t pid){
    char *lpCh = NULL;
    char *lpCh1 = NULL;
    char szLines[1024] = {0};
    unsigned long start = 0;
    unsigned  long tmp=0;
    char bufMaps[256] = "/proc/self/maps";
    if (pid != 0) {
        sprintf(bufMaps, "/proc/%d/maps", pid);
    }

    FILE *fp = fopen(bufMaps, "r");

    if (fp != NULL)
    {

        while (fgets(szLines, sizeof(szLines), fp))
        {
            if (strstr(szLines,name)&&strstr(szLines,"rw-p"))
            {
                lpCh1 =  strtok(szLines, " ");
                LOGME("Find=lpCh1 %s",lpCh1);
                lpCh = strtok(lpCh1, "-");
                LOGME("Find=lpCh %s",lpCh);
                lpCh = strtok(NULL, "-");
                LOGME("Find=lpCh  aa  %s",lpCh);
                tmp=strtoul(lpCh, NULL, 16);
            }
        }
        fclose(fp);
        return tmp;
    }
    else
    {
        LOGME("fopen error\r\n");
    }
    return -1;

}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_artxposedcpphook_CppHookMain_dumpso(JNIEnv *env, jobject thiz, jint pid) {
    // TODO: implement <init>()
    size_t begin = get_rx_address(solibname,pid);
    size_t end = get_rwx_address(solibname,pid);
    unsigned char *begin_1 = (unsigned char *)begin;

    size_t sz = (size_t)(end - begin);
    unsigned char *mem = (unsigned char*)malloc(sz);
    if (!mem) {
        return;
    }
    memset(mem, 0, sz);
    for(uint32_t i=0;i<end-begin;i++){
        mem[i] = *begin_1;
        begin_1++;

    }

    int fOut = open(outTmpPath, O_RDONLY|O_WRONLY|O_CREAT, 0666);
    if (fOut < 0) {
        printf("open %s error:%s\n", outTmpPath, strerror(errno));
    }
    ssize_t szW = write(fOut, mem, sz);
    printf("%d writed\n", (unsigned)szW);
    close(fOut);
    free(mem);
    int fOut1 = open(outPath, O_RDONLY|O_WRONLY|O_CREAT, 0666);
    if (fOut1 < 0) {
        return;
    }
    close(fOut1);

    fix_so(outTmpPath,outPath,begin);

}

static void printData(const char* tag,const char* data, int cnt) {
    stringstream fmt;
    for (int i = 0; i < cnt; i++) {
        char targetString[3];
        snprintf(targetString,3,"%02x ",data[i]);
        fmt<<targetString;
    }
    LOGME("%s:%s", tag,fmt.str().c_str());
}

typedef int (*zlibfunc)(void*,int);
zlibfunc zlibfunc_old;

int zlibfunc_replace(int64_t * buf03ptr,int flag){
    LOGME("hide zlibfunc_replace\n");
    int size=*(int *)(*buf03ptr + 8);
    const void* buf03data = *(const void **)*buf03ptr;
    printData("hide zlibfunc buf03",(const char*)buf03data,size);
    LOGME("hide zlibfunc_replace size:%d",size );
    return zlibfunc_old(buf03ptr,flag);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_artxposedcpphook_CppHookMain_hooknative(JNIEnv *env, jobject thiz, jint pid, jint address){

    size_t begin = get_rx_address("libwechatnormsg.so",pid);
    LOGME("hide begin:%x\n", (unsigned)begin);
    DobbyHook((void*)(begin+address), (void*)zlibfunc_replace, (void **)&zlibfunc_old);
}

jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {

    JNIEnv *env = nullptr;
    LOGME("fuck inject")

    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) == JNI_OK) {

        so_names[0] = "/system/lib/libriru_edxp.so";
        so_names[1] = "/system/lib/libsandhook.so";
        riru_hide(so_names,2);
        return JNI_VERSION_1_6;
    }

    return 0;
}







