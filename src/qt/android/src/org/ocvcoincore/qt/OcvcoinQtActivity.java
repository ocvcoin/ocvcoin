package org.ocvcoincore.qt;

import android.os.Bundle;
import android.system.ErrnoException;
import android.system.Os;

import org.qtproject.qt5.android.bindings.QtActivity;

import java.io.File;

public class OcvcoinQtActivity extends QtActivity
{
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        final File ocvcoinDir = new File(getFilesDir().getAbsolutePath() + "/.ocvcoin");
        if (!ocvcoinDir.exists()) {
            ocvcoinDir.mkdir();
        }

        try {
            Os.setenv("QT_QPA_PLATFORM", "android", true);
        } catch (ErrnoException e) {
            e.printStackTrace();
        }

        super.onCreate(savedInstanceState);
    }
}
