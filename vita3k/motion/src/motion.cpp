// Vita3K emulator project
// Copyright (C) 2024 Vita3K team
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

#include <motion/functions.h>
#include <motion/motion.h>
#include <motion/state.h>

#include <ctrl/state.h>
#include <util/log.h>

#include <SDL.h>
#include <SDL_gamecontroller.h>

static void init_device_sensors(MotionState& state){
    const int num_sensors = SDL_NumSensors();
    for(int idx = 0; idx < num_sensors; idx++){
        SDL_Sensor* sensor = SDL_SensorOpen(idx);
        bool sensor_used = true;
        switch (SDL_SensorGetType(sensor))
        {
        case SDL_SENSOR_ACCEL:
            state.device_accel = sensor;
            break;

        case SDL_SENSOR_GYRO:
            state.device_gyro = sensor;
            break;
        
        default:
            sensor_used = false;
            break;
        }
        if(!sensor_used)
            SDL_SensorClose(sensor);
    }
    state.has_device_motion_support = (state.device_accel && state.device_gyro);
}

void MotionState::init(){
    init_device_sensors(*this);

    if(has_device_motion_support)
        LOG_INFO("Device has builtin accelerometer and gyroscope.");

    // close them as having them opened uses battery
    if(device_accel){
        SDL_SensorClose(device_accel);
        device_accel = nullptr;
    }
    if(device_gyro){
        SDL_SensorClose(device_gyro);
        device_gyro = nullptr;
    }
}

SceFVector3 get_acceleration(const MotionState &state) {
    Util::Vec3f accelerometer = state.motion_data.GetAcceleration();
    return {
        accelerometer.x,
        accelerometer.y,
        accelerometer.z,
    };
}

SceFVector3 get_gyroscope(const MotionState &state) {
    Util::Vec3f gyroscope = state.motion_data.GetGyroscope() * static_cast<float>(2.f * M_PI);
    return {
        gyroscope.x,
        gyroscope.y,
        gyroscope.z,
    };
}

Util::Quaternion<SceFloat> get_orientation(const MotionState &state) {
    auto quat = state.motion_data.GetOrientation();
    return {
        { quat.xyz[1], quat.xyz[0], -quat.w },
        -quat.xyz[2],
    };
}

SceBool get_gyro_bias_correction(const MotionState &state) {
    return state.motion_data.IsGyroBiasEnabled();
}

void set_gyro_bias_correction(MotionState &state, SceBool setValue) {
    state.motion_data.EnableGyroBias(setValue);
}

void refresh_motion(MotionState &state, CtrlState &ctrl_state) {
    if (!state.is_sampling){
        // the check is done here so that everything sensor related is done on the same thread
        if(state.device_accel){
            SDL_SensorClose(state.device_accel);
            state.device_accel = nullptr;
        }
        if(state.device_gyro){
            SDL_SensorClose(state.device_gyro);
            state.device_gyro = nullptr;
        }
        return;
    }

    if (!ctrl_state.has_motion_support && !state.has_device_motion_support)
        return;

    // make sure to use the data from only one accelerometer and gyroscope
    bool found_gyro = false;
    bool found_accel = false;
    Util::Vec3f gyro;
    uint64_t gyro_timestamp = 0;
    Util::Vec3f accel;
    uint64_t accel_timestamp = 0;

    {
        // SDL_GameControllerGetSensorDataWithTimestamp is only supported on 2.26+
        // we need to check it because we are linking dynamically with SDL
        // TODO: put this in an init function
        SDL_version sdl_version;
        SDL_GetVersion(&sdl_version);
        const bool can_use_timestamp_fn = sdl_version.minor >= 26;

        std::lock_guard<std::mutex> guard(ctrl_state.mutex);
        for (auto controller : ctrl_state.controllers) {
            if (!found_gyro && controller.second.has_gyro) {
                if (can_use_timestamp_fn && SDL_GameControllerGetSensorDataWithTimestamp(controller.second.controller.get(), SDL_SENSOR_GYRO, &gyro_timestamp, reinterpret_cast<float *>(&gyro), 3) == 0)
                    found_gyro = true;
                else if (!can_use_timestamp_fn && SDL_GameControllerGetSensorData(controller.second.controller.get(), SDL_SENSOR_GYRO, reinterpret_cast<float *>(&gyro), 3) == 0)
                    found_gyro = true;
            }

            if (!found_accel && controller.second.has_accel) {
                if (can_use_timestamp_fn && SDL_GameControllerGetSensorDataWithTimestamp(controller.second.controller.get(), SDL_SENSOR_ACCEL, &accel_timestamp, reinterpret_cast<float *>(&accel), 3) == 0)
                    found_accel = true;
                else if (!can_use_timestamp_fn && SDL_GameControllerGetSensorData(controller.second.controller.get(), SDL_SENSOR_ACCEL, reinterpret_cast<float *>(&accel), 3) == 0)
                    found_accel = true;
            }
        }
    }

    const bool no_controller_sensor = (!found_gyro || !found_accel);
    const bool device_sensor_uninitialized = (state.device_accel == nullptr && state.device_gyro == nullptr);
    if(no_controller_sensor && state.has_device_motion_support && device_sensor_uninitialized){
        init_device_sensors(state);
    }

    bool gyro_from_device = false;
    if(!found_gyro && state.device_gyro){
        if (SDL_SensorGetDataWithTimestamp(state.device_gyro, &gyro_timestamp, reinterpret_cast<float *>(&gyro), 3) == 0) {
            found_gyro = true;
            gyro_from_device = true;
        }
    }

    bool accel_from_device = false;
    if(!found_accel && state.device_accel){
        if (SDL_SensorGetDataWithTimestamp(state.device_accel, &accel_timestamp, reinterpret_cast<float *>(&accel), 3) == 0) {
            found_accel = true;
            accel_from_device = true;
        }
    }

    if (!found_accel && !found_gyro)
        return;

    // if timestamp is not available, use the current time instead
    if (gyro_timestamp == 0 || accel_timestamp == 0) {
        std::chrono::time_point<std::chrono::steady_clock> ts = std::chrono::steady_clock::now();
        uint64_t timestamp = std::chrono::duration_cast<std::chrono::microseconds>(ts.time_since_epoch()).count();

        if (gyro_timestamp == 0)
            gyro_timestamp = timestamp;
        if (accel_timestamp == 0)
            accel_timestamp = timestamp;
    }

    gyro /= static_cast<float>(2.0 * M_PI);
    if(gyro_from_device)
        std::tie(gyro.x, gyro.y, gyro.z) = std::make_tuple(-gyro.y, gyro.z, -gyro.x);
    std::swap(gyro.y, gyro.z);
    gyro.y *= -1;

    accel /= -SDL_STANDARD_GRAVITY;
    if(accel_from_device)
        std::tie(accel.x, accel.y, accel.z) = std::make_tuple(-accel.y, accel.z, -accel.x);
    std::swap(accel.y, accel.z);
    accel.y *= -1;

    std::lock_guard<std::mutex> guard(state.mutex);

    state.motion_data.SetGyroscope(gyro);
    state.motion_data.SetAcceleration(accel);

    state.motion_data.UpdateRotation(gyro_timestamp - state.last_gyro_timestamp);
    state.motion_data.UpdateOrientation(accel_timestamp - state.last_accel_timestamp);

    state.last_gyro_timestamp = gyro_timestamp;
    state.last_accel_timestamp = accel_timestamp;
    state.last_counter++;
}
