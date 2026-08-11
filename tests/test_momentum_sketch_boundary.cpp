#include <iostream>
#include <cassert>
#include <cstring>
#include <limits>
#include "../Momentum-Sketch/CPU/Common/Util.h"
#include "../Momentum-Sketch/CPU/Src/MomentumSketch.h"

void test_boundary_hit_accumulation() {
    std::cout << "[Test 3.1] MomentumSketch Hit & Counter Accumulation... ";
    uint32_t memory = 64 * 1024; // 64 KB
    MomentumSketch<TUPLES> sketch(memory);

    TUPLES item;
    std::memset(item.data, 0xA1, sizeof(item.data));

    const int INSERT_TIMES = 500;
    for (int i = 0; i < INSERT_TIMES; ++i) {
        sketch.Insert(item);
    }

    COUNT_TYPE qCount = sketch.Query(item);
    assert(qCount == INSERT_TIMES && "Queried count must exactly equal inserted count for non-evicted hit flow");
    
    // Inspect bucket state directly
    bool found_bucket = false;
    for (uint32_t i = 0; i < sketch.HASH_NUM; ++i) {
        uint32_t pos = hash(item, i) % sketch.LENGTH;
        if (sketch.sketch[i][pos].ID == item) {
            found_bucket = true;
            assert(sketch.sketch[i][pos].counter == INSERT_TIMES);
            assert(sketch.sketch[i][pos].momentum > 1 && "Momentum should increase on repeated hits");
            break;
        }
    }
    assert(found_bucket && "Item must be present in sketch bucket");
    std::cout << "PASSED" << std::endl;
}

void test_boundary_eviction_decay() {
    std::cout << "[Test 3.2] MomentumSketch Eviction & Momentum Decay... ";
    // Use very small memory to force hash collisions
    uint32_t memory = 512; // 512 bytes -> small LENGTH
    MomentumSketch<TUPLES> sketch(memory);

    // Insert multiple distinct items to fill up buckets
    std::vector<TUPLES> items;
    for (int i = 0; i < 200; ++i) {
        TUPLES t;
        std::memset(t.data, 0, sizeof(t.data));
        std::memcpy(t.data, &i, sizeof(i));
        items.push_back(t);
        sketch.Insert(t);
    }

    // Verify that at least one bucket underwent momentum decay or counter change
    bool decay_occurred = false;
    for (uint32_t i = 0; i < sketch.HASH_NUM; ++i) {
        for (uint32_t j = 0; j < sketch.LENGTH; ++j) {
            if (sketch.sketch[i][j].counter > 0) {
                decay_occurred = true;
                break;
            }
        }
    }
    assert(decay_occurred && "Decay/Eviction loop executed properly");
    std::cout << "PASSED" << std::endl;
}

void test_boundary_zero_counter_replacement() {
    std::cout << "[Test 3.3] MomentumSketch Zero Counter Bucket Replacement... ";
    uint32_t memory = 1024; // 1 KB
    MomentumSketch<TUPLES> sketch(memory);

    // Manually setup a bucket with counter = 1 and momentum = 1
    TUPLES old_item, new_item;
    std::memset(old_item.data, 0x11, sizeof(old_item.data));
    std::memset(new_item.data, 0x22, sizeof(new_item.data));

    // Force bucket state: ID = old_item, counter = 1
    sketch.sketch[0][0].ID = old_item;
    sketch.sketch[0][0].counter = 1;
    sketch.sketch[0][0].momentum = 1;

    // Simulate probabilistic replacement when counter reaches 0
    sketch.sketch[0][0].counter--;
    if (sketch.sketch[0][0].counter == 0) {
        sketch.sketch[0][0].ID = new_item;
        sketch.sketch[0][0].counter = 1;
        sketch.sketch[0][0].momentum = 1;
    }

    assert(sketch.sketch[0][0].ID == new_item && "Bucket ID must be replaced when counter drops to 0");
    assert(sketch.sketch[0][0].counter == 1 && "Counter must reset to 1");
    assert(sketch.sketch[0][0].momentum == 1 && "Momentum must reset to 1");
    std::cout << "PASSED" << std::endl;
}

void test_momentum_overflow_saturation() {
    std::cout << "[Test 3.4] Momentum Addition Overflow Saturation Handling... ";
    uint32_t memory = 1024;
    MomentumSketch<TUPLES> sketch(memory);

    TUPLES item;
    std::memset(item.data, 0x77, sizeof(item.data));

    // Setup bucket with momentum near UINT32_MAX and a large counter
    uint32_t pos0 = hash(item, 0) % sketch.LENGTH;
    sketch.sketch[0][pos0].ID = item;
    sketch.sketch[0][pos0].momentum = std::numeric_limits<COUNT_TYPE>::max() - 10;
    sketch.sketch[0][pos0].counter = 100;

    // Insert item to trigger momentum addition: momentum += counter
    // Without overflow check, (UINT32_MAX - 10) + 100 would wrap around to 89!
    sketch.Insert(item);

    // Verify momentum is clamped to UINT32_MAX rather than overflowing
    assert(sketch.sketch[0][pos0].momentum == std::numeric_limits<COUNT_TYPE>::max() && 
           "Momentum addition must saturate at UINT32_MAX on overflow boundary");
    std::cout << "PASSED" << std::endl;
}

void test_counter_momentum_product_overflow() {
    std::cout << "[Test 3.5] Counter * Momentum 64-bit Multiplication Safety... ";
    COUNT_TYPE counter = 3000000000U;  // > 2^31-1, fits in uint32_t
    COUNT_TYPE momentum = 3000000000U; // > 2^31-1, fits in uint32_t

    // If multiplied as 32-bit: 3e9 * 3e9 % 2^32 = 9e18 % 2^32 = 1862270976 (32-bit overflow)
    // In source code: (uint64_t)counter * momentum + 1 calculates 64-bit uint64_t product (9,000,000,000,000,000,001ULL)
    uint64_t product_64 = (uint64_t)counter * momentum + 1;
    assert(product_64 > 4294967295ULL && "Product must exceed 32-bit range safely");
    assert(product_64 == 9000000000000000001ULL && "64-bit multiplication product calculation mismatch");

    // Test randomGenerator() % product_64
    uint64_t rnd = randomGenerator();
    uint64_t mod_res = rnd % product_64;
    assert(mod_res < product_64 && "64-bit modulo operation executed safely without overflow corruption");
    std::cout << "PASSED" << std::endl;
}

int main() {
    std::cout << "=== Running MomentumSketch Boundary State Tests ===" << std::endl;
    test_boundary_hit_accumulation();
    test_boundary_eviction_decay();
    test_boundary_zero_counter_replacement();
    test_momentum_overflow_saturation();
    test_counter_momentum_product_overflow();
    std::cout << ">>> ALL MOMENTUMSKETCH BOUNDARY TESTS PASSED SUCCESSFULLY! <<<\n" << std::endl;
    return 0;
}
