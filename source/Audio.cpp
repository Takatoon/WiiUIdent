#include "Audio.hpp"

#include <sndcore2/core.h>
#include <sndcore2/voice.h>
#include <coreinit/cache.h>
#include <vector>
#include <span>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <iostream> 

// Store audio data in global variables
static std::vector<uint16_t> navigateSoundData;
static std::vector<uint16_t> selectSoundData;
static std::vector<uint16_t> backSoundData;

static AXVoice* navigateVoice;
static AXVoice* selectVoice;
static AXVoice* backVoice;

// Volume
const  static uint16_t VOLUME_LEVEL = 0x4CCC;

// Used to identify fixed headers
const static char RIFF_ID[4] = { 'R', 'I', 'F', 'F' };
const static char WAVE_ID[4] = { 'W', 'A', 'V', 'E' };

static void read_wav(const char* path, std::vector<uint16_t>& samples) {
    std::cout << "Opening file: " << path << std::endl;

    //Open file in binary mode
    FILE* fd = fopen(path, "rb"); 
    if (!fd) {
        std::cout << "Failed to open file: " << path << std::endl;
        return;
    }

    // Get file stats
    struct stat stats; 
    if (fstat(fileno(fd), &stats)) { 
        std::cout << "Failed to get file stats for: " << path << std::endl;
        fclose(fd);
        return;
    }
    
    // Get file size
    auto size = stats.st_size; 
    if (size < 12 + 8) {  // RIFF header(12) + chunk header(8)
        std::cout << "File too small to be a valid WAV: " << path << std::endl;
        fclose(fd);
        return;
    }

    // Create a buffer to store the file data
    std::vector<unsigned char> buffer(size); /
    auto res = fread(buffer.data(), size, 1, fd); // Read all bytes from the file into the buffer
    fclose(fd);
    if (res != 1) {
        std::cout << "Failed to read file: " << path << std::endl;
        return;
    }

    // Check for "RIFF" and "WAVE" identifiers
    if (memcmp(buffer.data(), RIFF_ID, 4) != 0 || // Compare the first 4 bytes of the buffer with RIFF_ID
        memcmp(buffer.data() + 8, WAVE_ID, 4) != 0) { // Compare the 9th to 12th bytes of the buffer with WAVE_ID
        std::cout << "Invalid WAV file: " << path << std::endl;
        return;
    }

    std::cout << "Valid WAV file: " << path << std::endl;

    size_t offset = 12; // Skip RIFF header
    
    // Read chunks
    while (offset + 8 <= size) { // Read 8 bytes (chunk ID + chunk size)
        // Read chunk ID
        unsigned char chunk_id[4]; // Create a 4 byte array to store the chunk ID
        memcpy(chunk_id, buffer.data() + offset, 4); // Copy the chunk ID from the buffer
        
        // Get chunk size (next 4 bytes)
        uint32_t chunk_size = buffer[offset + 4] 
                            | (buffer[offset + 5] << 8)
                            | (buffer[offset + 6] << 16)
                            | (buffer[offset + 7] << 24);
        offset += 8;

        // Process "fmt " chunk
        if (memcmp(chunk_id, "fmt ", 4) == 0) { // Compare the chunk ID with "fmt "
            if (chunk_size < 16) {
                std::cout << "Invalid 'fmt ' chunk size in file: " << path << std::endl;
                return;
            }
            offset += chunk_size;  // Skip "fmt " chunk
        } 
        // Process "data" chunk
        else if (memcmp(chunk_id, "data", 4) == 0) {
            if (offset + chunk_size > size) {
                std::cout << "Invalid 'data' chunk size in file: " << path << std::endl;
                return;
            }

            // Read sample data
            std::span<unsigned char> sample_data { buffer.begin() + offset, chunk_size };
            size_t sampleCount = sample_data.size() / 2; // 2 bytes per sample
            samples.resize(sampleCount); // Resize the samples vector to store the sample data
            for (size_t i = 0; i < sampleCount; i++) {
                // Combine 2 bytes into a 16-bit sample
                samples[i] = sample_data[i * 2] | (sample_data[i * 2 + 1] << 8);
            }
            DCFlushRange(samples.data(), samples.size() * sizeof(samples[0])); // Flush the data cache
            std::cout << "Successfully read 'data' chunk from file: " << path << std::endl;
            return;
        } 
        else {
            // Skip other chunks
            offset += chunk_size;
        }
    }
}

static void setup_voice(AXVoice* voice, const std::vector<uint16_t>& samples) {
    AXVoiceVeData vol = {
        .volume = 0x4CCC, 
    };

    AXVoiceDeviceMixData mono_mix[6] = {
        { .bus = { {.volume = 0x4CCC}, } }, // TV Volume
        { .bus = { {.volume = 0x4CCC}, } }, // DRC Volume
    };

    AXVoiceBegin(voice);
    AXSetVoiceType(voice, 0);
    AXSetVoiceVe(voice, &vol);
    AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_DRC, 0, &mono_mix[0]);
    AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_TV, 0, &mono_mix[1]);
    AXSetVoiceSrcType(voice, AX_VOICE_SRC_TYPE_NONE);

    // Configure voice offsets
    AXVoiceOffsets offs = {
        .dataType = AX_VOICE_FORMAT_LPCM16,
        .loopingEnabled = AX_VOICE_LOOP_DISABLED,
        .endOffset = samples.size(),
        .data = samples.data(),
    };
    AXSetVoiceOffsets(voice, &offs);

    AXVoiceEnd(voice);
}

void Audio::Init() {
    read_wav("fs:/vol/content/click.wav", navigateSoundData);
    read_wav("fs:/vol/content/enter.wav", selectSoundData);
    read_wav("fs:/vol/content/back.wav", backSoundData);

    // Initialize the audio system (AX)
    AXInitParams params = {
        .renderer = AX_INIT_RENDERER_32KHZ,
        .pipeline = AX_INIT_PIPELINE_SINGLE,
    };
    AXInitWithParams(&params);

    navigateVoice = AXAcquireVoice(31, nullptr, nullptr);
    setup_voice(navigateVoice, navigateSoundData);
    selectVoice = AXAcquireVoice(31, nullptr, nullptr);
    setup_voice(selectVoice, selectSoundData);
    backVoice = AXAcquireVoice(31, nullptr, nullptr);
    setup_voice(backVoice, backSoundData);
}

void Audio::Shutdown() {
    AXFreeVoice(navigateVoice);
    AXFreeVoice(selectVoice);
    AXFreeVoice(backVoice);
    AXQuit();
}

void Audio::PlaySound(SoundID sound) {
    AXVoice* voice = nullptr;
    switch (sound) {
        case NAVIGATE:
            voice = navigateVoice;
            break;
        case SELECT:
            voice = selectVoice;
            break;
        case BACK:
            voice = backVoice;
            break;
    }

    if (voice) {
        AXSetVoiceCurrentOffset(voice, 0);
        AXSetVoiceState(voice, AX_VOICE_STATE_PLAYING);
    }
}
