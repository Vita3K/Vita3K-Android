package org.vita3k.emulator;


import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.res.AssetFileDescriptor;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.ParcelFileDescriptor;
import android.provider.OpenableColumns;
import android.view.ViewGroup;

import androidx.annotation.Keep;
import androidx.core.content.pm.ShortcutInfoCompat;
import androidx.core.content.pm.ShortcutManagerCompat;
import androidx.core.graphics.drawable.IconCompat;

import com.jakewharton.processphoenix.ProcessPhoenix;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.util.ArrayList;

import org.libsdl.app.SDLActivity;
import org.libsdl.app.SDLSurface;
import org.vita3k.emulator.overlay.InputOverlay;

public class Emulator extends SDLActivity
{
    private InputOverlay mOverlay;
    private String currentGameId = "";

    public InputOverlay getInputOverlay(){
        return mOverlay;
    }

    @Keep
    public void setCurrentGameId(String gameId){
        currentGameId = gameId;
    }

    /**
     * This method is called by SDL before loading the native shared libraries.
     * It can be overridden to provide names of shared libraries to be loaded.
     * The default implementation returns the defaults. It never returns null.
     * An array returned by a new implementation must at least contain "SDL2".
     * Also keep in mind that the order the libraries are loaded may matter.
     *
     * @return names of shared libraries to be loaded (e.g. "SDL2", "main").
     */
    @Override
    protected String[] getLibraries() {
        return new String[] {
                // "SDL2",
                // "SDL2_audio",
                // "SDL2_image",
                // "SDL2_mixer",
                // "SDL2_net",
                // "SDL2_ttf",
                "Vita3K"
        };
    }

    @Override
    protected SDLSurface createSDLSurface(Context context) {
        // Create the input overlay in the same time
        mOverlay = new InputOverlay(this);
        return new EmuSurface(context);
    }

    @Override
    protected void setupLayout(ViewGroup layout){
        super.setupLayout(layout);
        layout.addView(mOverlay);
    }

    static private String APP_RESTART_PARAMETERS = "AppStartParameters";

    @Override
    protected String[] getArguments() {
        Intent intent = getIntent();

        String[] args = intent.getStringArrayExtra(APP_RESTART_PARAMETERS);
        if(args == null)
            args = new String[]{};

        return args;
    }

    @Override
    protected void onNewIntent(Intent intent){
        super.onNewIntent(intent);

        // if we start the app from a shortcut and are in the main menu
        // or in a different game, start the new game
        if(intent.getAction().startsWith("LAUNCH_")){
            String game_id = intent.getAction().substring(7);
            if(!game_id.equals(currentGameId))
                ProcessPhoenix.triggerRebirth(getContext(), intent);
        }
    }

    @Keep
    public void restartApp(String app_path, String exec_path, String exec_args){
        ArrayList<String> args = new ArrayList<String>();

        // first build the args given to Vita3K when it restarts
        // this is similar to run_execv in main.cpp
        args.add("-a");
        args.add("true");
        if(!app_path.isEmpty()){
            args.add("-r");
            args.add(app_path);

            if(!exec_path.isEmpty()){
                args.add("--self");
                args.add(exec_path);

                if(!exec_args.isEmpty()){
                    args.add("--app-args");
                    args.add(exec_args);
                }
            }
        }

        Intent restart_intent = new Intent(getContext(), Emulator.class);
        restart_intent.putExtra(APP_RESTART_PARAMETERS, args.toArray(new String[]{}));
        ProcessPhoenix.triggerRebirth(getContext(), restart_intent);
    }

    static final int FILE_DIALOG_CODE = 545;

    @Keep
    public void showFileDialog(){
        Intent intent = new Intent()
                .setType("*/*")
                .setAction(Intent.ACTION_GET_CONTENT);

        intent = Intent.createChooser(intent, "Choose a file");
        startActivityForResult(intent, FILE_DIALOG_CODE);
    }

    private File getFileFromUri(Uri uri){
        try {
            InputStream inputStream = getContentResolver().openInputStream(uri);
            File tempFile = File.createTempFile("vita3ktemp", ".bin");
            tempFile.deleteOnExit();

            FileOutputStream outStream = new FileOutputStream(tempFile);
            byte[] buffer = new byte[1024 * 1024];
            int bytesRead;
            while ((bytesRead = inputStream.read(buffer)) != -1) {
                outStream.write(buffer, 0, bytesRead);
            }
            outStream.close();
            inputStream.close();

            return tempFile;
        } catch (Exception e) {
            return null;
        }
    }

    // from https://stackoverflow.com/questions/5568874/how-to-extract-the-file-name-from-uri-returned-from-intent-action-get-content
    private String getFileName(Uri uri){
        String result = null;
        if(uri.getScheme().equals(ContentResolver.SCHEME_CONTENT)){
            Cursor cursor = getContentResolver().query(uri, null, null, null, null);
            try {
                if(cursor != null && cursor.moveToFirst()){
                    int name_index = cursor.getColumnIndex(OpenableColumns.DISPLAY_NAME);
                    if(name_index >= 0)
                        result = cursor.getString(name_index);
                }
            } finally {
                cursor.close();
            }
        }

        if(result == null){
            result = uri.getLastPathSegment();
        }

        return result;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if(requestCode == FILE_DIALOG_CODE){
            if(resultCode == RESULT_OK){
                Uri result_uri = data.getData();
                String filename = getFileName(result_uri);
                String result_uri_string = result_uri.toString();
                int result_fd = -1;
                try {
                    AssetFileDescriptor asset_fd = getContentResolver().openAssetFileDescriptor(result_uri, "r");
                    // if the file is less than 64 KB, make a temporary copy
                    if(asset_fd.getLength() >= 64*1024) {
                        ParcelFileDescriptor file_descr = getContentResolver().openFileDescriptor(result_uri, "r");
                        result_fd = file_descr.detachFd();
                    } else {
                        File f = getFileFromUri(result_uri);
                        result_uri_string = f.getAbsolutePath();
                    }
                } catch (FileNotFoundException e) {
                }
                filedialogReturn(result_uri_string, result_fd, filename);
            } else if(resultCode == RESULT_CANCELED){
                filedialogReturn("", -1, "");
            }
        }
    }

    @Keep
    public void setControllerOverlayState(int overlay_mask, boolean edit, boolean reset){
        mOverlay.setState(overlay_mask);
        mOverlay.setIsInEditMode(edit);

        if(reset)
            mOverlay.resetButtonPlacement();
    }

    @Keep
    public void setControllerOverlayScale(float scale){
        mOverlay.setScale(scale);
    }

    @Keep
    public boolean createShortcut(String game_id, String game_name){
        if(!ShortcutManagerCompat.isRequestPinShortcutSupported(getContext()))
            return false;

        // first look at the icon, its location should always be the same
        File src_icon = new File(getExternalFilesDir(null), "vita/ux0/app/" + game_id + "/sce_sys/icon0.png");
        Bitmap icon;
        if(src_icon.exists())
            icon = BitmapFactory.decodeFile(src_icon.getPath());
        else
            icon = BitmapFactory.decodeResource(getResources(), R.mipmap.ic_launcher);

        // intent to directly start the game
        Intent game_intent = new Intent(getContext(), Emulator.class);
        ArrayList<String> args = new ArrayList<String>();
        args.add("-r");
        args.add(game_id);
        game_intent.putExtra(APP_RESTART_PARAMETERS, args.toArray(new String[]{}));
        game_intent.setAction("LAUNCH_" + game_id);

        // now create the pinned shortcut
        ShortcutInfoCompat shortcut = new ShortcutInfoCompat.Builder(getContext(), game_id)
                .setShortLabel(game_name)
                .setLongLabel(game_name)
                .setIcon(IconCompat.createWithBitmap(icon))
                .setIntent(game_intent)
                .build();
        ShortcutManagerCompat.requestPinShortcut(getContext(), shortcut, null);

        return true;
    }

    public native void filedialogReturn(String result_uri, int result_fd, String filename);
}