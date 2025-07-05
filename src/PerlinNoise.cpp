#include "PerlinNoise.h"
#include <algorithm>
#include <cmath>
#include <numeric>


namespace engine {

	std::vector<int> perm;
	std::vector<glm::vec2> gradients;

	PerlinNoise::PerlinNoise(unsigned int seed) {
		perm.resize(512);
		gradients.resize(256);

		std::vector<int> p(256);
		std::iota(p.begin(), p.end(), 0);

		std::default_random_engine engine(seed);
		std::shuffle(p.begin(), p.end(), engine);

		// Fill the permutation array as before
		for (int i = 0; i < 256; i++) {
			perm[i] = perm[i + 256] = p[i];
		}

		std::uniform_real_distribution<float> distribution(0.0f, 2.0f * glm::pi<float>());
		for (int i = 0; i < 256; i++) {
			float angle = distribution(engine); // use the seeded engine here
			gradients[i] = glm::vec2(cos(angle), sin(angle));
		}
	}

	float PerlinNoise::Fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }


	glm::vec2 PerlinNoise::RandomGradient(int ix, int iy) {
		unsigned int hash = ix * 374761393 + iy * 668265263;
		hash = (hash ^ (hash >> 13)) * 1274126177;
		hash = hash ^ (hash >> 16);
		float random = (hash % 1000) / 1000.0f;
		float angle = random * 2.0f * glm::pi<float>();
		return glm::vec2(cos(angle), sin(angle));
	}

	float PerlinNoise::Lerp(float a, float b, float t) { return a + t * (b - a); }

	float PerlinNoise::Grad(int hash, float x, float y) {
		int index = hash & 255;
		return gradients[index].x * x + gradients[index].y * y;
	}

	float PerlinNoise::Perlin2D(float x, float y) {
		int X = (int)floor(x) & 255;
		int Y = (int)floor(y) & 255;

		x -= floor(x);
		y -= floor(y);

		float u = Fade(x);
		float v = Fade(y);

		assert(!perm.empty() && "Permutation table is empty!");
		assert(X >= 0 && X < 256 && "X is out of range!");
		assert(Y >= 0 && Y < 256 && "Y is out of range!");

		int aa = perm[X] + Y;     // effectively perm[X] + Y
		int ab = perm[X] + Y + 1; // effectively perm[X] + (Y+1)
		int ba = perm[X + 1] + Y;
		int bb = perm[X + 1] + Y + 1;

		float gradAA = Grad(perm[aa], x, y);
		float gradBA = Grad(perm[ba], x - 1, y);
		float gradAB = Grad(perm[ab], x, y - 1);
		float gradBB = Grad(perm[bb], x - 1, y - 1);

		float lerpX1 = Lerp(gradAA, gradBA, u);
		float lerpX2 = Lerp(gradAB, gradBB, u);
		float result = Lerp(lerpX1, lerpX2, v);

		return result * 0.5f + 0.5f;
	}
}