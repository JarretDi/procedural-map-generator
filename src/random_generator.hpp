#pragma once
#include <random>
#include <vector>

class RandomGenerator {
    private:
        inline static std::random_device rd;
        inline static std::mt19937 gen{rd()};
    public:
        static int getInt(int min, int max) {
            std::uniform_int_distribution<int> dist(min, max);
            return dist(gen);
        }

        static int getIntDistribution(std::vector<int> weights) {
            std::discrete_distribution<int> dist(weights.begin(), weights.end());
            return dist(gen);
        }
};