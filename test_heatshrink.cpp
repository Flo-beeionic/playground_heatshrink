// test_heatshrink.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include <array>
#include "heatshrink_encoder.h"
#include "heatshrink_decoder.h"
#include <chrono>

/*
* 
	25 00 8f 0d 00 38											=> 008f = 143 values
	25 00 d6ca661329acea7f44a3d2a9b50a7d0a5914854120f02807fb	=> 25 -> id de trame, 00 -> id de chunk, d6ca661329 -> 0xd6, 0xca, 0x66, 0x13, 0x29
	25 01 f9fdfeff8001ff802200080e4031800300004c00c808044000
	25 02 81d1fd0197f202a0843f6f9fd0002f97bbdff2fafdbedf7000	
	25 03 ff1f8fc8007fa008bf5fbfd7f05820f0605011b0bfdfb00c7e
	25 04 9dffa7f3f5fdfc76b99c1e5777d1e9f57afdbeff87c7e3f2f9
	25 05 8883f5fa872819086624a8ff7f9da307c00500000000000000
*/

int main()
{
	static heatshrink_decoder decoder;
	int8_t data[] = { 0xd6, 0xca, 0x66, 0x13, 0x29, 0xac, 0xea, 0x7f, 0x44, 
		0xa3, 0xd2, 0xa9, 0xb5, 0x0a, 0x7d, 0x0a, 0x59, 0x14, 0x85, 0x41, 
		0x20, 0xf0, 0x28, 0x07, 0xfb, 0xf9, 0xfd, 0xfe, 0xff, 0x80, 0x01, 
		0xff, 0x80, 0x22, 0x00, 0x08, 0x0e, 0x40, 0x31, 0x80, 0x03, 0x00, 
		0x00, 0x4c, 0x00, 0xc8, 0x08, 0x04, 0x40, 0x00, 0x81, 0xd1, 0xfd, 
		0x01, 0x97, 0xf2, 0x02, 0xa0, 0x84, 0x3f, 0x6f, 0x9f, 0xd0, 0x00, 
		0x2f, 0x97, 0xbb, 0xdf, 0xf2, 0xfa, 0xfd, 0xbe, 0xdf, 0x70, 0x00, 
		0xff, 0x1f, 0x8f, 0xc8, 0x00, 0x7f, 0xa0, 0x08, 0xbf, 0x5f, 0xbf, 
		0xd7, 0xf0, 0x58, 0x20, 0xf0, 0x60, 0x50, 0x11, 0xb0, 0xbf, 0xdf, 
		0xb0, 0x0c, 0x7e, 0x9d, 0xff, 0xa7, 0xf3, 0xf5, 0xfd, 0xfc, 0x76, 
		0xb9, 0x9c, 0x1e, 0x57, 0x77, 0xd1, 0xe9, 0xf5, 0x7a, 0xfd, 0xbe, 
		0xff, 0x87, 0xc7, 0xe3, 0xf2, 0xf9, 0x88, 0x83, 0xf5, 0xfa, 0x87, 
		0x28, 0x19, 0x08, 0x66, 0x24, 0xa8, 0xff, 0x7f, 0x9d, 0xa3, 0x07, 0xc0, 0x05
	 };


	uint32_t polled = 0;
	uint32_t sunk = 0;
	uint64_t count = 0;

	std::array<int8_t, 2000> deflate;
	heatshrink_decoder_reset(&decoder);
	HSD_poll_res decoderPollResult;
	HSD_sink_res decoderSinkResult;
	auto start = std::chrono::system_clock::now();
	auto i = 0U;
	sunk = 0;
	polled = 0;
	count = 0;
	uint32_t samplesCount = sizeof(data);
	while (sunk < samplesCount) {
		heatshrink_decoder_sink(&decoder, reinterpret_cast<uint8_t*>(&data) + sunk, samplesCount - sunk, &count);
		sunk += count;
		do {
			decoderPollResult = heatshrink_decoder_poll(&decoder, reinterpret_cast<uint8_t*>(& deflate)+ polled, deflate.max_size() - polled, &count);
			polled += count;
		} while (decoderPollResult == HSDR_POLL_MORE);
		auto decodeFinishResult = heatshrink_decoder_finish(&decoder);
	}
	std::cout << "deltas" << std::endl;
	for (auto i = 0; i < samplesCount; i++) {
		float value = deflate[i];
		std::cout << value << ";";
	}
	std::cout << std::endl;
	std::cout << "values" << std::endl;
	float previousValue = 1013.f;
	for (auto i = 0; i < samplesCount; i++) {
		// pressure - previous = delta
		// pressure = delta + previous
		auto value = static_cast<float>(deflate[i]) + previousValue;
		std::cout << value << ";";
		previousValue = value;
	}


}
