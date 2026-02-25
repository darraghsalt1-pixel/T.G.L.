#ifndef TGL_AUDIO_AUDIO_H
#define TGL_AUDIO_AUDIO_H

#include "../core/types.h"
#include <string>

namespace tgl {

using SoundID = u32;
constexpr SoundID INVALID_SOUND = 0;

class Audio {
public:
    Audio() = default;
    ~Audio();

    Result init();
    void shutdown();

    // Load a WAV file, returns handle
    SoundID load(const std::string& path);
    void unload(SoundID id);

    // Playback
    void play(SoundID id, float volume = 1.0f, bool loop = false);
    void stop(SoundID id);
    void pause(SoundID id);
    void resume(SoundID id);

    // Global controls
    void set_master_volume(float volume);
    float master_volume() const { return master_volume_; }
    void stop_all();
    void pause_all();
    void resume_all();

private:
    float master_volume_ = 1.0f;
    struct AudioData;
    AudioData* data_ = nullptr;
};

// Simple music player (streams from disk)
class Music {
public:
    Music() = default;
    ~Music();

    Result load(const std::string& path);
    void play(bool loop = true);
    void stop();
    void pause();
    void resume();
    void set_volume(float volume);
    bool is_playing() const;

private:
    struct MusicData;
    MusicData* data_ = nullptr;
};

} // namespace tgl

#endif // TGL_AUDIO_AUDIO_H
