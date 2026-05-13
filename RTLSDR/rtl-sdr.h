#ifndef __RTL_SDR_H
#define __RTL_SDR_H
	
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif


#if defined(_WIN32)
#define RTLSDR_API __declspec(dllimport)
#else
#define RTLSDR_API
#endif

	typedef struct rtlsdr_dev rtlsdr_dev_t;

	RTLSDR_API uint32_t rtlsdr_get_device_count(void);

	RTLSDR_API const char* rtlsdr_get_device_name(uint32_t index);


	RTLSDR_API int rtlsdr_get_device_usb_strings(uint32_t index,
		char* vendor,
		char* product,
		char* serial);

	RTLSDR_API int rtlsdr_get_index_by_serial(const char* serial);

	RTLSDR_API int rtlsdr_open(rtlsdr_dev_t** dev, uint32_t index);

	RTLSDR_API int rtlsdr_close(rtlsdr_dev_t* dev);
	RTLSDR_API int rtlsdr_set_sample_rate(rtlsdr_dev_t* dev, uint32_t rate);
	RTLSDR_API int rtlsdr_set_center_freq(rtlsdr_dev_t* dev, uint32_t freq);
	RTLSDR_API uint32_t rtlsdr_get_center_freq(rtlsdr_dev_t* dev);

	RTLSDR_API int rtlsdr_set_freq_correction(rtlsdr_dev_t* dev, int ppm);
	RTLSDR_API int rtlsdr_get_freq_correction(rtlsdr_dev_t* dev);

	RTLSDR_API enum rtlsdr_tuner rtlsdr_get_tuner_type(rtlsdr_dev_t* dev);

	/* gains */
	RTLSDR_API int rtlsdr_set_tuner_gain(rtlsdr_dev_t* dev, int gain);
	RTLSDR_API int rtlsdr_get_tuner_gain(rtlsdr_dev_t* dev);
	RTLSDR_API int rtlsdr_set_tuner_gain_mode(rtlsdr_dev_t* dev, int manual);

	/* bandwidth */
	RTLSDR_API int rtlsdr_set_tuner_bandwidth(rtlsdr_dev_t* dev, uint32_t bw);

	/* sampling */
	RTLSDR_API int rtlsdr_set_sample_rate(rtlsdr_dev_t* dev, uint32_t rate);
	RTLSDR_API uint32_t rtlsdr_get_sample_rate(rtlsdr_dev_t* dev);

	/* streaming */
	RTLSDR_API int rtlsdr_reset_buffer(rtlsdr_dev_t* dev);
	RTLSDR_API int rtlsdr_read_sync(rtlsdr_dev_t* dev, void* buf, int len, int* n_read);

	typedef void(*rtlsdr_read_async_cb_t)(unsigned char* buf, uint32_t len, void* ctx);
	RTLSDR_API int rtlsdr_read_async(rtlsdr_dev_t* dev,
		rtlsdr_read_async_cb_t cb,
		void* ctx,
		uint32_t buf_num,
		uint32_t buf_len);

	RTLSDR_API int rtlsdr_cancel_async(rtlsdr_dev_t* dev);

#ifdef __cplusplus
}
#endif

#endif // __RTL_SDR_H