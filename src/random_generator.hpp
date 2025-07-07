#pragma once
#include <random>

class RandomGenerator {
public:
    static int getInt(int min, int max) {
        static std::mt19937 gen(std::random_device{});
        std::uniform_int_distribution<int> dist(min, max);
        return dist(gen);
    }
};