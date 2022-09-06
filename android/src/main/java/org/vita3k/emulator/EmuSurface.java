package org.vita3k.emulator;

import android.content.Context;
import android.view.SurfaceHolder;

import org.libsdl.app.SDLSurface;

public class EmuSurface extends SDLSurface {

    public EmuSurface(Context context){
        super(context);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        setSurfaceStatus(true);
        super.surfaceCreated(holder);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        setSurfaceStatus(false);
        super.surfaceDestroyed(holder);
    }

    public native void setSurfaceStatus(boolean surface_present);

}
