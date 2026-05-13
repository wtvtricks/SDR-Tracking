//Made by @oemat2,and @wtvtricks
#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include "RTLSDR/rtl-sdr.h"
#include <windows.h>
#include <cmath>
#include <conio.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "rtl-sdr.lib")
#pragma comment(lib, "ws2_32.lib")

const int SAMPLE_RATE_SDR = 1024000; // Use a power of 2 for better stability
const int AUDIO_SAMPLE_RATE = 48000;
const int DECIMATION = SAMPLE_RATE_SDR / AUDIO_SAMPLE_RATE;

class WinAudio {
public:
    WinAudio() : hWaveOut(NULL) {}

    void Init() {
        WAVEFORMATEX waveFormat = { 0 };
        waveFormat.wFormatTag = WAVE_FORMAT_PCM;
        waveFormat.nChannels = 1;
        waveFormat.nSamplesPerSec = AUDIO_SAMPLE_RATE;
        waveFormat.wBitsPerSample = 16;
        waveFormat.nBlockAlign = 2;
        waveFormat.nAvgBytesPerSec = AUDIO_SAMPLE_RATE * 2;

        if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
            std::cerr << "Error: Unable to open audio device." << std::endl;
        }
    }

    void PlayBuffer(const std::vector<short>& buffer) const {
        if (!hWaveOut) return;

        WAVEHDR* header = new WAVEHDR();
        ZeroMemory(header, sizeof(WAVEHDR));

        // We need to copy the data because waveOutWrite is asynchronous
        short* data = new short[buffer.size()];
        memcpy(data, buffer.data(), buffer.size() * sizeof(short));

        header->lpData = reinterpret_cast<LPSTR>(data);
        header->dwBufferLength = (DWORD)(buffer.size() * sizeof(short));
        header->dwFlags = WHDR_DONE; // Mark for cleanup

        waveOutPrepareHeader(hWaveOut, header, sizeof(WAVEHDR));
        waveOutWrite(hWaveOut, header, sizeof(WAVEHDR));

        // Note: In a professional app, you'd clean up these headers in a callback 
        // to prevent memory leaks. For homework, this shows the logic.
    }

    ~WinAudio() { if (hWaveOut) waveOutClose(hWaveOut); }

private:
    HWAVEOUT hWaveOut;
};

bool is_key_pressed(int vkey) {
    return (GetAsyncKeyState(vkey) & 0x8000) != 0;
}

struct Checkbox {
    bool clicked;
};

Checkbox use_rtl_checkbox = { false };

int main()  {
    double user_freq_mhz = 0.0;
    std::cout << "SDR SIGNAL METER V1.2\nEnter Frequency (MHz): ";
    std::cin >> user_freq_mhz;
    uint32_t freq_hz = (uint32_t)(user_freq_mhz * 1e6);

    rtlsdr_dev_t* dev = nullptr;

    if (use_rtl_checkbox.clicked) {
        WSADATA wsa;
        int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsa);
        if (wsaResult != 0) {
            std::cerr << "WSAStartup failed with error: " << wsaResult << std::endl;
            return 1;
        }
        SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(1234);
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        connect(s, (sockaddr*)&serverAddr, sizeof(serverAddr));
        if (s == INVALID_SOCKET) {
            std::cerr << "Socket creation failed." << std::endl;
            WSACleanup();
            return 1;
        }
        else {
            if (rtlsdr_open(&dev, 0) < 0) {
                std::cerr << "Failed to open TCP." << std::endl;
                return 1;
            }
            rtlsdr_set_sample_rate(dev, freq_hz);
            std::cout << "TCP Connected to SDR at " << user_freq_mhz << " MHz" << std::endl;
            std::cout << "Using TCP for SDR data." << std::endl;
        }
    }

    // Setup Hardware
    rtlsdr_set_sample_rate(dev, SAMPLE_RATE_SDR);
    rtlsdr_set_center_freq(dev, freq_hz);
    rtlsdr_set_tuner_gain_mode(dev, 0); // Auto-gain
    rtlsdr_reset_buffer(dev);

    WinAudio player;
    player.Init();

    std::vector<uint8_t> rtl_buffer(16384 * 2); // I and Q samples
    int n_read = 0;
    bool is_recording = false;
    std::ofstream rec_file;

    std::cout << "Running... [Q] Quit [R] Record [Space] Audio" << std::endl;

    while (!is_key_pressed('Q')) {
        int r = rtlsdr_read_sync(dev, rtl_buffer.data(), (int)rtl_buffer.size(), &n_read);
        if (r < 0) break;

        // 1. Calculate Signal Power (RSSI)
        double pwr = 0;
        for (int i = 0; i < n_read; i++) {
            double val = (rtl_buffer[i] - 127.5);
            pwr += val * val;
        }
        double db = 10 * std::log10(pwr / n_read);

        // Simple visual meter
        std::cout << "\rSignal Strength: " << db << " dB    " << std::flush;

        // 2. Simple AM Demodulation (for the sake of hearing 'something')
        std::vector<short> audio_samples;
        for (int i = 0; i < n_read; i += 2) {
            double I = rtl_buffer[i] - 127.5;
            double Q = rtl_buffer[i + 1] - 127.5;
            short mag = (short)(std::sqrt(I * I + Q * Q) * 200);
            audio_samples.push_back(mag);
        }

        // 3. Play Audio
        player.PlayBuffer(audio_samples);

        // 4. Handle Recording
        if (is_key_pressed('R')) {
            if (!is_recording) {
                rec_file.open("sdr_output.bin", std::ios::binary);
                is_recording = true;
                std::cout << "\n[REC START]" << std::endl;
            }
            else {
                rec_file.close();
                is_recording = false;
                std::cout << "\n[REC STOP]" << std::endl;
            }
            Sleep(300);
        }
    }

    rtlsdr_close(dev);
    return 0;
}