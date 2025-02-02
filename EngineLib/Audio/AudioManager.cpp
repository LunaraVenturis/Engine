/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section LICENSE
 * MIT License
 * 
 * Copyright (c) 2025 Krusto
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * @section DESCRIPTION
 * 
 * Audio Manager definitions
 */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/

#include "AudioManager.hpp"
#include <vector>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio/miniaudio.h>
#pragma GCC diagnostic pop

/***********************************************************************************************************************
Typedefs
***********************************************************************************************************************/

typedef struct {
    std::string_view name;
    std::string_view path;
} AudioView;

typedef struct {
    ma_sound* sound;
} Audio;

typedef struct {
    ma_fence done_fence;
    ma_engine engine;
    std::vector<AudioView> audio_views;
    std::vector<Audio> audios;
} AudioManagerData;

/***********************************************************************************************************************
Global Variables
***********************************************************************************************************************/
inline static AudioManagerData g_AudioManagerData;

/***********************************************************************************************************************
Function Definitions
***********************************************************************************************************************/
namespace LunaraEngine
{

    AudioManagerResult AudioManager::Init()
    {
        auto result = ma_engine_init(NULL, &g_AudioManagerData.engine);
        if (result != MA_SUCCESS) { return AudioManager_Result_Fail; }
        result = ma_fence_init(&g_AudioManagerData.done_fence);
        if (result != MA_SUCCESS) { return AudioManager_Result_Fail; }

        return AudioManager_Result_Success;
    }

    AudioManagerResult AudioManager::LoadAudio(std::string_view name, std::string_view path)
    {
        Audio audio;

        ma_uint32 soundFlags = 0;
        soundFlags |= MA_SOUND_FLAG_ASYNC;
        soundFlags |= MA_SOUND_FLAG_DECODE;// Decode in the beginning

        audio.sound = (ma_sound*) malloc(sizeof(ma_sound));
        auto result = ma_sound_init_from_file(&g_AudioManagerData.engine, path.data(), soundFlags, NULL,
                                              &g_AudioManagerData.done_fence, audio.sound);
        if (result != MA_SUCCESS) { return AudioManager_Result_Fail; }

        //Wait for the sound to load
        ma_fence_wait(&g_AudioManagerData.done_fence);

        g_AudioManagerData.audio_views.push_back({name, path});
        g_AudioManagerData.audios.push_back(audio);

        return AudioManager_Result_Success;
    }

    void AudioManager::PlayAudio(std::string_view name)
    {

        for (size_t i = 0; i < g_AudioManagerData.audio_views.size(); i++)
        {
            if (g_AudioManagerData.audio_views[i].name != name) { continue; }

            auto& audio = g_AudioManagerData.audios[i];
            AudioManager::SoundPlay(audio.sound);
            break;
        }
    }

    void AudioManager::Destroy()
    {
        for (size_t i = 0; i < g_AudioManagerData.audio_views.size(); i++)
        {
            ma_sound_uninit(g_AudioManagerData.audios[i].sound);
            free(g_AudioManagerData.audios[i].sound);
        }
        g_AudioManagerData.audios.clear();
        g_AudioManagerData.audio_views.clear();

        ma_fence_uninit(&g_AudioManagerData.done_fence);
        ma_engine_uninit(&g_AudioManagerData.engine);
    }

    bool AudioManager::IsAudioPlaying(std::string_view name)
    {
        for (size_t i = 0; i < g_AudioManagerData.audio_views.size(); i++)
        {
            if (g_AudioManagerData.audio_views[i].name != name) { continue; }

            return ma_sound_is_playing(g_AudioManagerData.audios[i].sound);
        }
        return false;
    }

    void AudioManager::SoundRewindToBegin(void* sound) { ma_sound_seek_to_pcm_frame((ma_sound*) sound, 0); }

    void AudioManager::SoundPlay(void* sound)
    {
        auto result = ma_sound_start((ma_sound*) sound);
        if (result != MA_SUCCESS) { ma_atomic_exchange_32(&((ma_sound*) sound)->atEnd, MA_TRUE); }
    }
}// namespace LunaraEngine
