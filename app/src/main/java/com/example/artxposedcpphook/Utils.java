package com.example.artxposedcpphook;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.util.Log;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.Closeable;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Utils {
    public static final String TAG = "hide";
    public static void copyFileFromJar(String inJarPath, String distPath) {
        BufferedInputStream in = null;
        BufferedOutputStream out = null;
        try {
            in = new BufferedInputStream(new FileInputStream(inJarPath));
            out = new BufferedOutputStream(new FileOutputStream(distPath));

            int len;
            byte[] b = new byte[1024];
            while ((len = in.read(b)) != -1) {
                out.write(b, 0, len);
            }
            out.flush();
        } catch (IOException e) {
            e.printStackTrace();
            Log.i(TAG,"copyFileFromJar  "+e.getMessage());
        } finally {
            close(out);
            close(in);
        }
    }
    public static void close(Closeable closeable) {
        try {
            if (closeable != null) {
                closeable.close();
            }
        } catch (IOException io) {
            io.printStackTrace();
        }
    }
    public static List<Integer> queryAllRunningAppInfo(Context context,String thirdPackageName) {
        PackageManager pm = context.getPackageManager();
        // 查询所有已经安装的应用程序
        List<ApplicationInfo> listAppcations = pm.getInstalledApplications(PackageManager.GET_UNINSTALLED_PACKAGES);
        Collections.sort(listAppcations,new ApplicationInfo.DisplayNameComparator(pm));// 排序

        // 保存所有正在运行的包名 以及它所在的进程信息
        Map<String, ActivityManager.RunningAppProcessInfo> pgkProcessAppMap = new HashMap<>();

        ActivityManager mActivityManager = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        // 通过调用ActivityManager的getRunningAppProcesses()方法获得系统里所有正在运行的进程
        List<ActivityManager.RunningAppProcessInfo> appProcessList = mActivityManager
                .getRunningAppProcesses();

        for (ActivityManager.RunningAppProcessInfo appProcess : appProcessList) {
            int pid = appProcess.pid; // pid
            String processName = appProcess.processName; // 进程名
            Log.i(TAG, "processName: " + processName + "  pid: " + pid);

            String[] pkgNameList = appProcess.pkgList; // 获得运行在该进程里的所有应用程序包

            // 输出所有应用程序的包名
            for (int i = 0; i < pkgNameList.length; i++) {
                String pkgName = pkgNameList[i];
                Log.i(TAG, "packageName " + pkgName + " at index " + i+ " in process " + pid);
                // 加入至map对象里
                pgkProcessAppMap.put(pkgName, appProcess);
            }
        }
        // 保存所有正在运行的应用程序信息
        List<Integer> runningAppInfos = new ArrayList<>(); // 保存过滤查到的AppInfo

        for (ApplicationInfo app : listAppcations) {
            // 如果该包名存在 则构造一个RunningAppInfo对象
            if (pgkProcessAppMap.containsKey(thirdPackageName)) {
                // 获得该packageName的 pid 和 processName
                int pid = pgkProcessAppMap.get(thirdPackageName).pid;
                runningAppInfos.add(pid);
            }
        }

        return runningAppInfos;

    }
    /**
     * 获取模块的So文件路径
     */
    public static String getInjecSoPath(Context mContext,String selfPackageName,String selfSoName) {
        PackageManager pm = mContext.getPackageManager();
        List<PackageInfo> pkgList = pm.getInstalledPackages(0);
        if (pkgList.size() > 0) {
            for (PackageInfo pi : pkgList) {
                Log.i(TAG,"publicSourceDir  "+pi.applicationInfo.publicSourceDir);
//               android11  /data/app/~~JPZU7djIModJoqsZCrcIqA==/com.example.artxposedcpphook-BcQej18Aq0dXzn3jTRlp-Q==/base.apk
                if (pi.applicationInfo.publicSourceDir.contains(selfPackageName)) {
                    String path = pi.applicationInfo.publicSourceDir.
                            replace("base.apk", "lib/arm64/" + selfSoName);
                    return path;
                }
            }
        }
        return null;
    }

}
