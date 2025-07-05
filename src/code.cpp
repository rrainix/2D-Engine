#include "keygen.h"
#include <random>
#include <stdexcept>
#include <chrono>
#include <array>

namespace {
	constexpr int ipow(int base, int exp) {
		int result = 1;
		while (exp--) result *= base;
		return result;
	}
}

namespace keygen {
	const std::string k_numericChars =
		"0123456789";
	const std::string k_alphaNumericChars =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	const char* ULID_CHARS = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";

	static thread_local std::mt19937_64 gen{
		std::random_device{}()
	};

	// 2) Verteilungen als thread_local
	static thread_local std::uniform_int_distribution<unsigned int> distByte{ 0, 255 };
	static thread_local std::uniform_int_distribution<std::size_t>
		distNumChar{ 0, k_numericChars.size() - 1 };
	static thread_local std::uniform_int_distribution<std::size_t>
		distAlphaNumChar{ 0, k_alphaNumericChars.size() - 1 };


	int generateNumeric(uint8_t length) {
		if (length <= 0 || length > 9)
			throw std::invalid_argument("length must be between 1 and 9");

		int lower = ipow(10, length - 1);
		int upper = ipow(10, length) - 1;

		// Statt: std::uniform_int_distribution<> dist(lower, upper);
		// nutzen wir parametric version:
		static thread_local std::uniform_int_distribution<> distNum;
		return distNum(gen, decltype(distNum)::param_type{ lower, upper });
	}
	std::string generateNumeric(uint16_t length) {
		std::string key;
		key.reserve(length);

		for (int i = 0; i < length; ++i) {
			key += k_numericChars[distNumChar(gen)];
		}

		return key;
	}
	std::string generateAlphanumeric(std::size_t length) {
		static thread_local std::uniform_int_distribution<std::size_t>
			distAlphaNum{ 0, k_alphaNumericChars.size() - 1 };

		std::string key;
		key.resize(length);                    // resize statt reserve+push_back
		for (std::size_t i = 0; i < length; ++i) {
			key[i] = k_alphaNumericChars[distAlphaNum(gen)];
		}
		return key;
	}
	std::string generateUniqueCode(std::size_t length) {
		static thread_local std::mt19937_64 rng{
			std::random_device{}()
			^ (std::mt19937_64::result_type)
			  std::chrono::steady_clock::now().time_since_epoch().count()
		};

		std::uniform_int_distribution<> dist(0, allowed_chars.size() - 1);

		std::string result;
		result.reserve(length);
		for (std::size_t i = 0; i < length; ++i) {
			result += allowed_chars[dist(rng)];
		}

		return result;
	}
	std::string encodeBase32(const std::array<uint8_t, 16>& data) {
		std::string result(26, '\0');
		uint64_t buffer = 0;
		int bits = 0, pos = 0;

		for (uint8_t byte : data) {
			buffer = (buffer << 8) | byte;
			bits += 8;
			while (bits >= 5) {
				result[pos++] = ULID_CHARS[(buffer >> (bits - 5)) & 0x1F];
				bits -= 5;
			}
		}
		if (bits > 0) {
			result[pos++] = ULID_CHARS[(buffer << (5 - bits)) & 0x1F];
		}
		// pos sollte jetzt 26 sein
		return result;
	}
	std::string generateUUID() {
		static thread_local std::uniform_int_distribution<uint32_t> dist32(0, 0xFFFFFFFF);
		uint32_t d[4] = {
			dist32(gen), dist32(gen), dist32(gen), dist32(gen)
		};

		std::ostringstream ss;
		ss << std::hex << std::setfill('0')
			<< std::setw(8) << d[0] << '-'
			<< std::setw(4) << ((d[1] >> 16) & 0xFFFF) << '-'
			<< std::setw(4) << ((d[1] & 0x0FFF) | 0x4000) << '-'
			<< std::setw(4) << (((d[2] >> 16) & 0x3FFF) | 0x8000) << '-'
			<< std::setw(4) << (d[2] & 0xFFFF)
			<< std::setw(8) << d[3];

		return ss.str();
	}
	std::string generateULID() {
		using namespace std::chrono;
		uint64_t t = duration_cast<milliseconds>(
			system_clock::now().time_since_epoch()).count();
		std::array<uint8_t, 16> b{};
		for (int i = 5; i >= 0; --i) { b[i] = t & 0xFF; t >>= 8; }
		for (int i = 6; i < 16; ++i) { b[i] = distByte(gen); }
		return encodeBase32(b);
	}
}
