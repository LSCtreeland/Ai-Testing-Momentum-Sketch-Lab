#include <iostream>
#include <cassert>
#include <vector>
#include <cmath>
#include <random>
#include "../Momentum-Sketch/CPU/Common/hash.h"

void test_hash_determinism() {
    std::cout << "[Test 1.1] Hash Determinism... ";
    uint32_t key = 0x12345678;
    uint32_t hash1 = Hash::MurmurHash32(key, 42);
    uint32_t hash2 = Hash::MurmurHash32(key, 42);
    assert(hash1 == hash2 && "Identical inputs with same seed must yield identical hash");
    std::cout << "PASSED" << std::endl;
}

void test_hash_seed_variance() {
    std::cout << "[Test 1.2] Hash Seed Variance... ";
    uint32_t key = 0x87654321;
    uint32_t h1 = Hash::MurmurHash32(key, 1);
    uint32_t h2 = Hash::MurmurHash32(key, 2);
    assert(h1 != h2 && "Different seeds should produce different hash values");
    std::cout << "PASSED" << std::endl;
}

void test_hash128_consistency() {
    std::cout << "[Test 1.3] Hash128 Consistency... ";
    uint64_t key[2] = {0xDEADBEEF12345678ULL, 0xCAFEBABE98765432ULL};
    uint32_t out[4] = {0};
    Hash::MurmurHash128(key, out, 0);
    assert((out[0] != 0 || out[1] != 0 || out[2] != 0 || out[3] != 0) && "Hash128 output should not be all zero");
    std::cout << "PASSED" << std::endl;
}

void test_hash_uniformity_chi_square() {
    std::cout << "[Test 1.4] Hash Uniformity (Chi-Square Test)... ";
    const int NUM_KEYS = 1000000;
    const int NUM_BUCKETS = 1000;
    std::vector<int> bucket_counts(NUM_BUCKETS, 0);

    for (int i = 0; i < NUM_KEYS; ++i) {
        uint32_t h = hash(i, 10);
        int b = h % NUM_BUCKETS;
        bucket_counts[b]++;
    }

    double expected = static_cast<double>(NUM_KEYS) / NUM_BUCKETS;
    double chi_square = 0.0;

    for (int b = 0; b < NUM_BUCKETS; ++b) {
        double diff = bucket_counts[b] - expected;
        chi_square += (diff * diff) / expected;
    }

    // Reduced Chi-Square should be close to 1.0 (for 999 deg of freedom, chi^2 / df < 1.3 is very good uniformity)
    double reduced_chi_square = chi_square / (NUM_BUCKETS - 1);
    std::cout << "(Chi-Square / df = " << reduced_chi_square << ") ";
    assert(reduced_chi_square < 1.5 && "Hash distribution failed Chi-Square uniformity test");
    std::cout << "PASSED" << std::endl;
}

int main() {
    std::cout << "=== Running Hash Uniformity & Consistency Tests ===" << std::endl;
    test_hash_determinism();
    test_hash_seed_variance();
    test_hash128_consistency();
    test_hash_uniformity_chi_square();
    std::cout << ">>> ALL HASH TESTS PASSED SUCCESSFULLY! <<<\n" << std::endl;
    return 0;
}
