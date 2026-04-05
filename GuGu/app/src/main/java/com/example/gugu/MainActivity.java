package com.example.gugu;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.view.View;

import com.google.androidgamesdk.GameActivity;

import java.io.IOException;

public class MainActivity extends GameActivity {
    static {
        System.loadLibrary("Game");
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        if (hasFocus) {
            hideSystemUi();
        }
    }
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        Context androidContext = getApplicationContext();
        String dataDirStr = androidContext.getDataDir().getAbsolutePath();
        String fileDirStr = androidContext.getFilesDir().getAbsolutePath();
        passInternalStorageDataPathAndFilePath(dataDirStr, fileDirStr);
        setDpiScale(androidContext.getResources().getDisplayMetrics().densityDpi / 160.0f);
    }

    private void hideSystemUi() {
        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                        | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_FULLSCREEN
        );
    }

    private native void passInternalStorageDataPathAndFilePath(String dataPath, String filePath);

    private native void setDpiScale(float dpiScale);

    public static boolean isAssetFileExists(AssetManager assetManager, String fileName)
    {
        String[] files = new String[0];
        try {
            files = assetManager.list("");
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        if (files != null) {
            for (String file : files) {
                if (file.equals(fileName)) {
                    return true;
                }
            }
        }
        return false;
    }

    public static boolean isAssetDirExists(AssetManager assetManager, String path) {
        try {
            String[] files = assetManager.list(path);
            return files != null && files.length > 0;
        } catch (IOException e) {
            return false;
        }
    }
}