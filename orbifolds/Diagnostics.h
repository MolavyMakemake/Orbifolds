#pragma once

#include <ctime>
#include <iostream>

template <int N>
class Diagnostics {
public:
	int ticks_hz[N]{};
	
	Diagnostics(int samplePeriod_ms = 1000) 
		: samplePeriod_ms(samplePeriod_ms) {
		lastSample_clock = std::clock();
	}

	int& operator [] (int i) { return tick_counter[i]; }

	void tick() {
		clock_t dClock = std::clock() - lastSample_clock;
		bool flush = dClock * 1000 >= CLOCKS_PER_SEC * samplePeriod_ms;

		for (int i = 0; i < N; i++) {
			ticks_hz[i] = flush ? tick_counter[i] * 1000 / samplePeriod_ms : ticks_hz[i];
			tick_counter[i] *= !flush;
		}

		lastSample_clock += flush * dClock;
	}

private:
	int samplePeriod_ms;

	clock_t lastSample_clock;
	int tick_counter[N]{};
};