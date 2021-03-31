package com.example.artxposedcpphook;

import android.content.Context;
import android.util.Log;

import java.io.File;

public class CppHookMain {
    public static final String TAG = "hide";
    public static final String selfSoName = "libnative-libs.so";
    public  native void dumpso(int pid);
    public native void hooknative(int pid,int address);
    public static void loadCustomSo(Context mContext, String thirdPackageName, String selfPackageName) {
        try {
            File file = new File("/data/data/" + thirdPackageName + "/" + selfSoName);
            if (!file.exists()) {
                file.createNewFile();
            }else{
                file.delete();
            }
            String sourcePath =  Utils.getInjecSoPath(mContext, selfPackageName, selfSoName);
            Log.i(TAG,"source path "+sourcePath+",dest path " +file.getAbsolutePath());
            if(sourcePath!=null&&file.getAbsolutePath()!=null){
                Log.i(TAG," sourcePath "+new File(sourcePath).getTotalSpace());
                Utils.copyFileFromJar(sourcePath, file.getAbsolutePath());
                Log.i(TAG," destPath "+file.getTotalSpace());
                Log.i(TAG,"System.load path "+file.getAbsolutePath());
                System.load(file.getAbsolutePath());
                Log.i(TAG,"System.loaded ");
            }

        } catch (Exception e) {
            e.printStackTrace();
            Log.i(TAG,"loadCustomSo  "+e.getMessage());
        }
    }
}
