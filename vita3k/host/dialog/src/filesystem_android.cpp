// Vita3K emulator project
// Copyright (C) 2022 Vita3K team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

/**
 * @file filesystem.cpp
 * @brief Filesystem-related dialogs
 *
 * This file contains all the source code related to the implementation and
 * abstraction of user interface dialogs from the host operating system
 * related to filesystem interaction such as file or folder opening dialogs.
 *
 * The implementation is using the android native file dialog.
 */

#include <host/dialog/filesystem.h>

#include <map>

#include <SDL.h>
#include <jni.h>

static bool file_dialog_running = false;

// the result from the dialog, this is an UTF-8 string
static std::string dialog_result_uri = "";
// the resulting file descriptor from the dialog
static int dialog_result_fd = -1;
static std::string dialog_result_filename = "";

static std::map<fs::path, std::pair<int, std::string>> path_mapping;

extern "C" JNIEXPORT void JNICALL
Java_org_vita3k_emulator_Emulator_filedialogReturn(JNIEnv *env, jobject thiz, jstring result_uri, jint result_fd, jstring filename) {
    const char *result_ptr = env->GetStringUTFChars(result_uri, nullptr);
    dialog_result_uri = std::string(result_ptr);
    env->ReleaseStringUTFChars(result_uri, result_ptr);

    result_ptr = env->GetStringUTFChars(filename, nullptr);
    dialog_result_filename = std::string(result_ptr);
    env->ReleaseStringUTFChars(filename, result_ptr);

    dialog_result_fd = result_fd;
    file_dialog_running = false;
}

/**
 * @brief Format the file extension list of a certain file filter to match the
 * format expected by the underlying file browser dialog implementation
 *
 * @param file_extensions_list File extensions list
 * @return std::string A string containing the properly formatted file extension list
 */
std::string format_file_filter_extension_list(std::vector<std::string> &file_extensions_list) {
    // Formatted string containing the properly formatted file extension list
    //
    // In the case of nativefiledialog, the expected file extension is a single
    // string containing comma-separated list of file extensions
    //
    // Example: "cpp,cc,txt,..."
    std::string formatted_string = "";

    // For every file extension in the filter list, append it to the formatted string
    for (size_t index = 0; index < file_extensions_list.size(); index++) {
        // Don't add comma before the first file extension
        if (index == 0) {
            formatted_string += file_extensions_list.at(index);
        } else {
            formatted_string += "," + file_extensions_list.at(index);
        }
    }

    return formatted_string;
};

namespace host {
namespace dialog {
namespace filesystem {
Result open_file(fs::path &resulting_path, std::vector<FileFilter> file_filters, fs::path default_path) {
    SDL_AndroidRequestPermission("android.permission.READ_EXTERNAL_STORAGE");

    // retrieve the JNI environment.
    JNIEnv *env = reinterpret_cast<JNIEnv *>(SDL_AndroidGetJNIEnv());

    // retrieve the Java instance of the SDLActivity
    jobject activity = reinterpret_cast<jobject>(SDL_AndroidGetActivity());

    // find the Java class of the activity. It should be SDLActivity or a subclass of it.
    jclass clazz(env->GetObjectClass(activity));

    // find the identifier of the method to call
    jmethodID method_id = env->GetMethodID(clazz, "showFileDialog", "()V");

    file_dialog_running = true;
    // effectively call the Java method
    env->CallVoidMethod(activity, method_id);

    // clean up the local references.
    env->DeleteLocalRef(activity);
    env->DeleteLocalRef(clazz);

    while (file_dialog_running)
        SDL_Delay(10);

    if (dialog_result_uri.empty())
        return Result::CANCEL;

    resulting_path = fs::path(dialog_result_uri);
    // if dialog_result_fd is -1, it means the file was copied and we can open it with the usual io functions
    if (dialog_result_fd != -1)
        path_mapping[resulting_path] = {dialog_result_fd, dialog_result_filename};

    return Result::SUCCESS;
};

Result pick_folder(fs::path &resulting_path, fs::path default_path) {
    SDL_AndroidRequestPermission("android.permission.MANAGE_EXTERNAL_STORAGE");
    
    // retrieve the JNI environment.
    JNIEnv *env = reinterpret_cast<JNIEnv *>(SDL_AndroidGetJNIEnv());

    // retrieve the Java instance of the SDLActivity
    jobject activity = reinterpret_cast<jobject>(SDL_AndroidGetActivity());

    // find the Java class of the activity. It should be SDLActivity or a subclass of it.
    jclass clazz(env->GetObjectClass(activity));

    // find the identifier of the method to call
    jmethodID method_id = env->GetMethodID(clazz, "changeDir", "()V");

    file_dialog_running = true;
    // effectively call the Java method
    env->CallVoidMethod(activity, method_id);

    // clean up the local references.
    env->DeleteLocalRef(activity);
    env->DeleteLocalRef(clazz);

    while (file_dialog_running)
        SDL_Delay(10);

    if (dialog_result_uri.empty())
        return Result::CANCEL;
            
    resulting_path = fs::path(dialog_result_uri);
    return Result::SUCCESS;
    
    // return Result::ERROR;
};

std::string get_error() {
    std::string error = "";

#ifndef __ANDROID__
    // Retrieve error char array from nativefiledialog and turn it into a C++ string
    error.assign(NFD::GetError());
#endif

    return error;
};

FILE *resolve_host_handle(const fs::path &path) {
    auto it = path_mapping.find(path);
    if (it != path_mapping.end() && it->second.first != -1) {
        int fd = it->second.first;
        return fdopen(fd, "rb");
    } else {
        return fopen(path.c_str(), "rb");
    }
}

std::string resolve_path_string(const fs::path &path){
    auto it = path_mapping.find(path);
    if (it != path_mapping.end()) {
        // this is only the filename but that's still better than giving the Uri
        return it->second.second;
    } else {
        return std::string(path.c_str());
    }
}

std::string resolve_filename(const fs::path &path){
    auto it = path_mapping.find(path);
    if (it != path_mapping.end()) {
        return it->second.second;
    } else {
        return std::string(path.filename().c_str());
    }
}

} // namespace filesystem
} // namespace dialog
} // namespace host
