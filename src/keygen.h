#pragma once
#include <random>
#include <cassert>
#include <string>
#include <chrono>
#include <random>
#include <array>
#include <sstream>
#include <iomanip>
#include <cstdint>

namespace keygen {
	const std::string allowed_chars =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"-_+=";

	int generateNumeric(uint8_t length);
	std::string generateNumeric(uint16_t length);
	std::string generateAlphanumeric(std::size_t length);
	std::string generateUniqueCode(std::size_t length);
	std::string generateUUID();
	std::string generateULID();
	std::string encodeBase32(const std::array<uint8_t, 16>& data);
}