#pragma once
#include <vector>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace engine {
	class PerlinNoise {
	public:
		PerlinNoise(unsigned int seed = 1337);

		float Perlin2D(float x, float y);

	private:
		std::vector<int> perm;
		float Fade(float t);
		float Grad(int hash, float x, float y);
		glm::vec2 RandomGradient(int ix, int iy);
		float Lerp(float a, float b, float t);
	};
}
