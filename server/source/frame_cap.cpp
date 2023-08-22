#include "frame_cap.hpp"

#include <stdint.h>
#include <unistd.h>
#include <time.h>

#define DELAY 60000000

uint64_t now = 0;
uint64_t lastFrame = 0;

void frameCapGetTime(void) {
}

void frameCap_DelayNextFrame(void) {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	uint64_t delta = now - lastFrame;
	lastFrame = now;
	now = ts.tv_sec*1000000000 + ts.tv_nsec;
	
	if (delta < DELAY) {
		usleep((DELAY - delta)/1000);
	}
}