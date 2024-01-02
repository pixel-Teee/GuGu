package com.example.gugu;

import android.content.Context;
import android.os.Bundle;
import android.view.View;

import com.google.androidgamesdk.GameActivity;

public class MainActivity extends GameActivity {
    static {
        System.loadLibrary("GuGu");
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
}