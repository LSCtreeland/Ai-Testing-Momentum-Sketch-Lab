#include <iostream>
#include <cassert>
#include <cstring>
#include <unordered_set>
#include "../Momentum-Sketch/CPU/Common/Util.h"

void test_tuples_size_and_packing() {
    std::cout << "[Test 2.1] TUPLES Size & Pack Alignment... ";
    assert(sizeof(TUPLES) == 13 && "sizeof(TUPLES) must be strictly 13 bytes under #pragma pack(1)");
    assert(TUPLES_LEN == 13 && "TUPLES_LEN macro must be 13");
    std::cout << "PASSED" << std::endl;
}

void test_tuples_field_accessors() {
    std::cout << "[Test 2.2] TUPLES Field Accessors... ";
    TUPLES t;
    // srcIP: 1.2.3.4 (0x04030201 in little-endian)
    t.data[0] = 1; t.data[1] = 2; t.data[2] = 3; t.data[3] = 4;
    // dstIP: 5.6.7.8
    t.data[4] = 5; t.data[5] = 6; t.data[6] = 7; t.data[7] = 8;
    // srcPort: 8080 (0x1F90)
    t.data[8] = 0x90; t.data[9] = 0x1F;
    // dstPort: 443 (0x01BB)
    t.data[10] = 0xBB; t.data[11] = 0x01;
    // proto: 6 (TCP)
    t.data[12] = 6;

    assert(t.srcIP() == 0x04030201 && "srcIP mismatch");
    assert(t.dstIP() == 0x08070605 && "dstIP mismatch");
    assert(t.srcPort() == 8080 && "srcPort mismatch");
    assert(t.dstPort() == 443 && "dstPort mismatch");
    assert(t.proto() == 6 && "proto mismatch");
    assert(t[12] == 6 && "operator[] index mismatch");
    std::cout << "PASSED" << std::endl;
}

void test_tuples_high64_low40() {
    std::cout << "[Test 2.3] TUPLES high64 & low40... ";
    TUPLES t;
    std::memset(t.data, 0, sizeof(t.data));
    t.data[0] = 0xAA;
    t.data[7] = 0xBB;
    t.data[12] = 0xCC; // proto

    uint64_t h64 = t.high64();
    assert((h64 & 0xFF) == 0xAA && "high64 byte 0 mismatch");
    assert(((h64 >> 56) & 0xFF) == 0xBB && "high64 byte 7 mismatch");

    uint64_t l40 = t.low40();
    assert(((l40 >> 24) & 0xFF) == 0xCC && "low40 proto byte mismatch");
    std::cout << "PASSED" << std::endl;
}

void test_tuples_equality_and_std_hash() {
    std::cout << "[Test 2.4] TUPLES Operator== & std::hash... ";
    TUPLES t1, t2, t3;
    std::memset(t1.data, 0x11, sizeof(t1.data));
    std::memset(t2.data, 0x11, sizeof(t2.data));
    std::memset(t3.data, 0x22, sizeof(t3.data));

    assert(t1 == t2 && "Identical TUPLES must compare equal");
    assert(!(t1 == t3) && "Different TUPLES must not compare equal");

    std::hash<TUPLES> hasher;
    assert(hasher(t1) == hasher(t2) && "Hashes of identical TUPLES must match");

    std::unordered_set<TUPLES> set;
    set.insert(t1);
    set.insert(t3);
    assert(set.size() == 2 && "Unordered set size must be 2");
    assert(set.find(t2) != set.end() && "Set lookup of t2 (equal to t1) must succeed");
    std::cout << "PASSED" << std::endl;
}

int main() {
    std::cout << "=== Running TUPLES Structure & Operator Tests ===" << std::endl;
    test_tuples_size_and_packing();
    test_tuples_field_accessors();
    test_tuples_high64_low40();
    test_tuples_equality_and_std_hash();
    std::cout << ">>> ALL TUPLES TESTS PASSED SUCCESSFULLY! <<<\n" << std::endl;
    return 0;
}
