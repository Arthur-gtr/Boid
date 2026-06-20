#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>
#include <atomic>
#include <cmath>
#include "simulate/VulkanEngine.hpp"

struct AudioState {
    std::atomic<float> bassLevel{0.0f};
    std::atomic<float> trebleLevel{0.0f};
};

void audioCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    (void)pOutput;
    AudioState* state = static_cast<AudioState*>(pDevice->pUserData);
    if (pInput == nullptr)
        return;

    const float* input = static_cast<const float*>(pInput);
    
    float bassSum = 0.0f;
    float trebleSum = 0.0f;
    ma_uint32 totalSamples = frameCount * pDevice->capture.channels;

    static float prev_lp = 0.0f;
    static float prev_hp = 0.0f;
    static float prev_sample = 0.0f;

    const float alpha_lp = 0.02f;
    const float alpha_hp = 0.85f;

    for (ma_uint32 i = 0; i < totalSamples; ++i) {
        float sample = input[i];

        prev_lp = alpha_lp * sample + (1.0f - alpha_lp) * prev_lp;
        bassSum += prev_lp * prev_lp;

        prev_hp = alpha_hp * (prev_hp + sample - prev_sample);
        prev_sample = sample;
        trebleSum += prev_hp * prev_hp;
    }

    state->bassLevel.store(std::sqrt(bassSum / totalSamples), std::memory_order_relaxed);
    state->trebleLevel.store(std::sqrt(trebleSum / totalSamples), std::memory_order_relaxed);
}

int main() {
    try {
        AudioState audioState;
        ma_device_config config = ma_device_config_init(ma_device_type_capture);
        config.capture.format = ma_format_f32;
        config.capture.channels = 2;
        config.dataCallback = audioCallback;
        config.pUserData = &audioState;

        ma_device device;
        if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
            throw std::runtime_error("Failed to load miniaudio");
        }
        ma_device_start(&device);

        sf::WindowBase window(sf::VideoMode({1920, 1080}), "Boids Audio-Reactifs", sf::State::Fullscreen);
        VulkanEngine engine(window, 8196);

        while (window.isOpen()) {
            while (const std::optional<sf::Event> event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) window.close();
            }
            float bass = audioState.bassLevel.load(std::memory_order_relaxed) * 15;
            float treble = audioState.trebleLevel.load(std::memory_order_relaxed) * 15;

            engine.drawFrame(bass, treble);
        }

        ma_device_uninit(&device);

    } catch (const std::exception& e) {
        std::cerr << "CRASH : " << e.what() << std::endl;
        return 1;
    }
    return 0;
}