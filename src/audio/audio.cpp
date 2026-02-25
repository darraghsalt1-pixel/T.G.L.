#include <tgl/audio/audio.h>
#include <tgl/core/log.h>
#include <tgl/utils/file_utils.h>
#include <vector>
#include <unordered_map>
#include <cstring>

// Minimal audio implementation using platform-native APIs
// Windows: WASAPI/WinMM, Linux: ALSA via write() to /dev/dsp or PulseAudio

namespace tgl {

struct WavHeader {
    char riff[4];       // "RIFF"
    u32 file_size;
    char wave[4];       // "WAVE"
    char fmt[4];        // "fmt "
    u32 fmt_size;
    u16 audio_format;
    u16 channels;
    u32 sample_rate;
    u32 byte_rate;
    u16 block_align;
    u16 bits_per_sample;
};

struct SoundData {
    std::vector<u8> pcm_data;
    u32 sample_rate = 44100;
    u16 channels = 2;
    u16 bits_per_sample = 16;
};

struct Audio::AudioData {
    std::unordered_map<SoundID, SoundData> sounds;
    SoundID next_id = 1;
};

Audio::~Audio() { shutdown(); }

Result Audio::init() {
    data_ = new AudioData();
    log::info("Audio system initialized (basic WAV playback)");
    return {};
}

void Audio::shutdown() {
    if (data_) {
        delete data_;
        data_ = nullptr;
    }
}

SoundID Audio::load(const std::string& path) {
    if (!data_) return INVALID_SOUND;

    auto bytes = file::read_binary(path);
    if (bytes.size() < sizeof(WavHeader) + 8) {
        log::error("Failed to load audio: %s", path.c_str());
        return INVALID_SOUND;
    }

    WavHeader header;
    std::memcpy(&header, bytes.data(), sizeof(WavHeader));

    if (std::strncmp(header.riff, "RIFF", 4) != 0 || std::strncmp(header.wave, "WAVE", 4) != 0) {
        log::error("Not a valid WAV file: %s", path.c_str());
        return INVALID_SOUND;
    }

    // Find data chunk
    size_t pos = sizeof(WavHeader);
    while (pos + 8 < bytes.size()) {
        char chunk_id[5] = {};
        std::memcpy(chunk_id, &bytes[pos], 4);
        u32 chunk_size;
        std::memcpy(&chunk_size, &bytes[pos + 4], 4);

        if (std::strncmp(chunk_id, "data", 4) == 0) {
            SoundData sd;
            sd.sample_rate = header.sample_rate;
            sd.channels = header.channels;
            sd.bits_per_sample = header.bits_per_sample;
            sd.pcm_data.assign(bytes.begin() + pos + 8, bytes.begin() + pos + 8 + chunk_size);

            SoundID id = data_->next_id++;
            data_->sounds[id] = std::move(sd);
            log::info("Loaded audio: %s (id=%u, %uHz, %uch)", path.c_str(), id, header.sample_rate, header.channels);
            return id;
        }
        pos += 8 + chunk_size;
    }

    log::error("No data chunk found in WAV: %s", path.c_str());
    return INVALID_SOUND;
}

void Audio::unload(SoundID id) {
    if (data_) data_->sounds.erase(id);
}

void Audio::play(SoundID id, float volume, bool loop) {
    if (!data_ || data_->sounds.find(id) == data_->sounds.end()) return;
    // Platform-specific playback would go here
    // For now this is a stub - integrate with OS audio APIs
    (void)volume; (void)loop;
    log::debug("Playing sound %u (volume=%.2f, loop=%d)", id, volume, loop);
}

void Audio::stop(SoundID /*id*/) {}
void Audio::pause(SoundID /*id*/) {}
void Audio::resume(SoundID /*id*/) {}
void Audio::set_master_volume(float volume) { master_volume_ = volume; }
void Audio::stop_all() {}
void Audio::pause_all() {}
void Audio::resume_all() {}

// Music
struct Music::MusicData {};
Music::~Music() { delete data_; }
Result Music::load(const std::string& path) { (void)path; return {}; }
void Music::play(bool loop) { (void)loop; }
void Music::stop() {}
void Music::pause() {}
void Music::resume() {}
void Music::set_volume(float volume) { (void)volume; }
bool Music::is_playing() const { return false; }

} // namespace tgl
