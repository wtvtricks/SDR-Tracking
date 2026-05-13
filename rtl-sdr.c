#include "rtl-sdr.h"
#include <stdlib.h>
#include <string.h>

strunt rtlsdr_dev {
    // In a real implementation, this would hold device-specific state
    int dummy;
};

int rtlsdr_open(rtlsdr_dev_t** dev, uint32_t index)
{
    if (!dev)
        return -1;
    *dev = (rtlsdr_dev_t*)malloc(sizeof(rtlsdr_dev_t));
    if (!*dev)
        return -1;
    return 0;
}

int rtlsdr_close(rtlsdr_dev_t* dev)
{
    if (!dev)
        return -1;
    free(dev);
    return 0;
}

int rtlsdr_set_center_freq(rtlsdr_dev_t* dev, uint32_t freq)
{
    if (!dev)
        return -1;
    return 0;
}

int rtlsdr_reset_buffer(rtlsdr_dev_t* dev)
{
    if (!dev)
        return -1;
    return 0;
}

int rtlsdr_read_sync(rtlsdr_dev_t* dev, void* buf, int len, int* n_read)
{
    if (!dev || !buf || !n_read)
        return -1;
    memset(buf, 127, len); // Simulate silence (I/Q at mid-scale)
    *n_read = len; // Indicate we "read" the full buffer
    return 0;
}

int rtlsdr_set_sample_rate(rtlsdr_dev_t* dev, uint32_t rate)
{
    if (!dev)
		return -1;
	return 0;
}

int rtlsdr_set_tuner_gain_mode(rtlsdr_dev_t* dev, int manual)
{
    if (!dev)
        return -1;
	return 0;
}    
