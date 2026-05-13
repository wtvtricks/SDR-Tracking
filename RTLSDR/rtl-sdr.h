#ifndef __RTL_SDR_H
#define __RTL_SDR_H
	
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) 
	#ifdef RTLSDR_EXPORTS
	#define RTLSDR_API __declspec(dllexport)
	#else
	#define RTLSDR_API __declspec(dllimport)
    #endif
#else
	#define RTLSDR_API
#endif
	typedef struct rtlsdr_dev rtlsdr_dev_t;

	RTLSDR_API int rtlsdr_open(rtlsdr_dev_t** dev, uint32_t index);
	RTLSDR_API int rtlsdr_close(rtlsdr_dev_t* dev);
	RTLSDR_API int rtlsdr_set_sample_rate(rtlsdr_dev_t* dev, uint32_t rate);
	RTLSDR_API int rtlsdr_set_center_freq(rtlsdr_dev_t* dev, uint32_t freq);
	RTLSDR_API int rtlsdr_set_tuner_gain_mode(rtlsdr_dev_t* dev, int manual);
	RTLSDR_API int rtlsdr_reset_buffer(rtlsdr_dev_t* dev);
	RTLSDR_API int rtlsdr_read_sync(rtlsdr_dev_t* dev, void* buf, int len, int* n_read);

#ifdef __cplusplus
}
#endif

#endif // __RTL_SDR_H