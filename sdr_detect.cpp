//Made by @oemat2,and @wtvtricks
#include <iostream>
#include <fstream>
#include <vector>
#include "RTLSDR/rtl-sdr.h"
#include <cmath>
#include <unistd.h>
#include <termios.h>
#include <thread>
const int SAMPLE_RATE_SDR = 1000000; // 1 MGHz
const int AUDIO_SAMPLE_RATE = 48000; // 48 KHz
const int DECIMATION = SAMPLE_RATE_SDR / AUDIO_SAMPLE_RATE;
const float PI = 3.14159265f;
class LinuxAudio {
public:
	LinuxAudio() {}
	void Init() {}
	void playTone(int frequency, int duration_ms) {
		// Placeholder for Linux audio implementation
		// You can integrate PulseAudio or ALSA later
	}
};

bool is_key_pressed(char key) {
	return false; // Placeholder - would require termios for non-blocking input
}
int main() {
	bool is_recording = false;
	bool audio_enabled = false;
	std::ofstream recording_file;
    double user_freq = 0.0;
	std::cout << "===============================" << std::endl;
	std::cout << "     SDR SIGNAL METER V1.1     " << std::endl;
	std::cout << "===============================" << std::endl;
	std::cout << "Enter Fequency in MHz(e.g., 162.4 for NOAA Weather Radio) : ";
    std::cin >> user_freq;
	uint32_t center_freq = static_cast<uint32_t>(user_freq * 1000000);
	int device_count = rtlsdr_get_device_count();
	if (device_count < 0) {
		std::cerr << "Error: Unable to get device count." << std::endl;
		return 0;
	}
	int r = 0;
	std::cout << "Number of RTL-SDR devices found: " << device_count << std::endl;
	if (device_count == 0) {
		std::cout << "No RTL-SDR devices connected." << std::endl;
		return 0;
	}
	for (int i = 0; i < device_count; ++i) {
		char vendor[256]{}, product[256], serial[256] = { 0 };
		rtlsdr_get_device_usb_strings(i, vendor, product, serial);
		std::cout << "Device " << i << ": " << vendor << " " << product
			<< " (Serial: " << serial << ")" << std::endl;
	}
	std::cout << "Attempting to open device 0" << std::endl;
	rtlsdr_dev_t* dev = nullptr;
	if (rtlsdr_open(&dev, 0) < 0) {
		std::cerr << "Error: Unable to open device 0." << std::endl;
		return 1;
	}
	std::cout << "Device 0 opened successfully." << std::endl;
	r = rtlsdr_set_sample_rate(dev, 2048000);
	if (r < 0) {
		std::cerr << "Error: Unable to set sample rate." << std::endl;
		rtlsdr_close(dev);
		return 1;
	}
	if (r < 0) {
		std::cout << "Started buffer reset Reading signal" << "MHz." << std::endl;
		rtlsdr_close(dev);
		return 1;
	}
	else {
		std::cout << "Tuned to." << std::endl;
	}
	rtlsdr_set_sample_rate(dev, SAMPLE_RATE_SDR);
	rtlsdr_set_center_freq(dev, center_freq);
	rtlsdr_set_tuner_gain_mode(dev, 0); // Auto gain
	rtlsdr_reset_buffer(dev); // Clear buffer before starting
	LinuxAudio player;
	player.Init(); 
	std::cout << "Reading signal (Press Ctrl+C to stop)" << std::endl;
	std::cout << "Recording will start after first signal detection" << std::endl;
	uint8_t buffer[16384];
	int n_read = 0;
	while (true) {
		int r = rtlsdr_read_sync(dev, buffer, sizeof(buffer), &n_read);
		if (r < 0) break;
		if (is_recording && recording_file.is_open()) {
			recording_file.write(reinterpret_cast<char*>(buffer), n_read);
		}
		double current_buffer_power = 0.0;
		for (int i = 0; i < n_read; ++i) {
			current_buffer_power += (static_cast<double>(buffer[i]) - 127.5) * (static_cast<double>(buffer[i]) - 127.5);
		}
		double average_power = current_buffer_power / n_read;
		double db = 10 * std::log10(average_power);
		std::cout << "Signal Power: " << db << " dB\r";
		std::cout.flush();
	}
	if (is_recording && recording_file.is_open()) recording_file.close();
 	rtlsdr_close(dev);
	return 0;
}