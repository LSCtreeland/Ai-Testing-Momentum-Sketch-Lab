# Momentum-Sketch 單元測試套件 (Unit Test Suite)

本目錄包含 `Ai-Testing-Momentum-Sketch-Lab` 的獨立 C++ 單元測試套件，使用 **CMake** 與 **CTest** 進行自動化建構與驗證。每個測試皆編譯為獨立的執行檔，確保測試間無狀態共用與相互污染。

---

## 🧪 測試檔案介紹

### 1. [`test_hash_uniformity.cpp`](test_hash_uniformity.cpp)
**測試主題**：Hash 計算穩定度與卡方均勻度檢定 (Chi-Square Uniformity Test)

- **`Test 1.1: Hash Determinism`**：驗證相同的鍵值 (Key) 與種子 (Seed) 計算出的 32-bit Hash 值完全一致。
- **`Test 1.2: Hash Seed Variance`**：驗證相同的鍵值在不同種子下產生相異 Hash 值，確保多重 Hash 桶的獨立性。
- **`Test 1.3: Hash128 Consistency`**：驗證 128-bit MurmurHash3 輸出 4 個 32-bit 陣列之正確性與穩定度。
- **`Test 1.4: Hash Uniformity (Chi-Square Test)`**：
  - 生成 1,000,000 個連續 Key 映射至 1,000 個 Hash 桶。
  - 計算卡方統計量 $\chi^2 = \sum \frac{(O_i - E)^2}{E}$。
  - 實測卡方標準化值 $\chi^2 / \text{df} = 0.9421$（極接近理想值 1.0），證明 MurmurHash3 分佈均勻無極端熱點碰撞。

---

### 2. [`test_tuples_operators.cpp`](test_tuples_operators.cpp)
**測試主題**：網路 5-Tuple 資料結構與運算子驗證

- **`Test 2.1: TUPLES Size & Pack Alignment`**：驗證 `sizeof(TUPLES) == 13` Bytes（檢查 `#pragma pack(1)` 記憶體對齊，避免編譯器填補 Padding 破壞二進制封包讀取）。
- **`Test 2.2: TUPLES Field Accessors`**：寫入已知 IP (`srcIP`, `dstIP`)、Port (`srcPort`, `dstPort`) 及 Protocol (`proto`)，驗證欄位讀取與轉換結果正確。
- **`Test 2.3: TUPLES high64 & low40`**：驗證 SIMD 向量化運算所依賴的前 8 Bytes (`high64`) 與後 5 Bytes (`low40`) 之位元遮罩與位移正確性。
- **`Test 2.4: TUPLES Operator== & std::hash`**：驗證 `operator==` 判定 logic，以及在 `std::unordered_set<TUPLES>` 中的 Hash Table 插入與搜尋相容性。

---

### 3. [`test_momentum_sketch_boundary.cpp`](test_momentum_sketch_boundary.cpp)
**測試主題**：MomentumSketch 核心演算法命中、衰減、歸零替換與 32-bit/64-bit 溢位處理

- **`Test 3.1: Hit & Counter Accumulation`**：測試對相同 Flow ID 連續插入時，Counter 與 Momentum 的正確累加與查詢。
- **`Test 3.2: Eviction & Momentum Decay`**：測試高碰撞環境下，選出最小 Counter 桶進行動量衰減 (`momentum /= DECAY_FACTOR`) 的邏輯。
- **`Test 3.3: Zero Counter Bucket Replacement`**：測試當 Counter 扣減至 0 時，桶內 Flow ID 替換為新流量，且 `counter` 與 `momentum` 同步重置為 1。
- **`Test 3.4: Momentum Addition Overflow Saturation`**：測試當 `momentum` 接近 `UINT32_MAX` 時，`momentum += counter` 觸發飽和保護，精確截斷在 `UINT32_MAX` 而不會繞回歸零。
- **`Test 3.5: Counter * Momentum 64-bit Multiplication Safety`**：測試當 `counter` 與 `momentum` 均接近 $3 \times 10^9$ 時，`(uint64_t)counter * momentum + 1` 安全進行 64-bit 乘法與取模 (`%`) 運算，防止 32-bit 整數溢位。

---

## 🛠️ 建構與執行測試 (Build & Run)

在 WSL 環境中執行以下指令進行編譯與自動化測試：

```bash
# 1. 進入 tests 目錄
cd tests

# 2. 建立 build 目錄並配置 CMake
cmake -B build

# 3. 編譯所有測試執行檔
cmake --build build

# 4. 使用 CTest 執行全套單元測試
ctest --test-dir build --output-on-failure
```

或是直接單獨執行編譯出的測試二進制檔：
```bash
./build/test_hash_uniformity
./build/test_tuples_operators
./build/test_momentum_sketch_boundary
```
