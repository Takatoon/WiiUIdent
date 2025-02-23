#pragma once

namespace Audio {
    enum SoundID {
        NAVIGATE,
        SELECT,
        BACK
    };

    void Init();
    void Shutdown();
    void PlaySound(SoundID sound);
}