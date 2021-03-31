package com.example.artxposedcpphook;


import android.app.Application;
import android.content.Context;
import android.util.Log;

import java.io.File;
import java.util.Collections;
import java.util.List;

import de.robv.android.xposed.IXposedHookLoadPackage;
import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedHelpers;
import de.robv.android.xposed.callbacks.XC_LoadPackage;

public class Main implements IXposedHookLoadPackage {
    public static final String TAG = "hide";
    public static final String thirdPackageName = "com.tencent.mm";
    public static final String selfPackageName = "com.example.artxposedcpphook";
    private List<Integer> pids;
    @Override
    public void handleLoadPackage(XC_LoadPackage.LoadPackageParam param) throws Throwable {
        if (!param.packageName.equals(thirdPackageName))
            return;
        Log.i(TAG, param.packageName);
        XposedHelpers.findAndHookMethod(Application.class, "onCreate",
                new XC_MethodHook() {
                    @Override
                    protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                        super.afterHookedMethod(param);
                        CppHookMain.loadCustomSo((Context) param.thisObject,thirdPackageName,selfPackageName);
                        pids=   Utils.queryAllRunningAppInfo((Context) param.thisObject,thirdPackageName);
                    }
                });
        new Thread(new Runnable() {
            @Override
            public void run() {
                while (true){
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    if(pids!=null&&pids.size()>0){
                        int min = (int) Collections.min(pids);
                        if(min>0){
                            Log.i(TAG, param.packageName+" start hook");
                            new CppHookMain().hooknative(min,0x417220);
                            break;
                        }
                    }

                }
            }
        }).start();
    }

}
