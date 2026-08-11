# AI Testing Momentum Sketch Lab

`Ai-Testing-Momentum-Sketch-Lab` 是一個針對 **Momentum-Sketch** 演算法與 **Two-Stage** 巨大流量檢測 (Heavy Hitter Detection) 框架進行 AI 測試、單元驗證 (Unit Testing) 與合成數據分析的實驗室儲存庫。

## 📌 儲存庫主要功能

1. **核心演算法庫 (`Momentum-Sketch/`)**：
   - 包含 **Momentum-Sketch**（結合到達強度與動量指標的機率衰減 Sketch）與 **Two-Stage 框架**（Stage 1 CU-Sketch 預過濾器 + Stage 2 Momentum-Sketch）。
   - 支援 C++ 基準測試 (Benchmarking) 與效能指標計算 (Insert/Query Throughput, Recall, Precision, F1-Score, AAE, ARE)。
   - 提供 50MB 重尾分佈 (Zipfian Heavy-Tail) 二進制資料集生成器 (`generate_dataset.py`)。

2. **單元測試套件 (`tests/`)**：
   - 包含獨立編譯的 C++ 單元測試套件，涵蓋 MurmurHash3 均勻度與卡方檢定、`TUPLES` 資料結構記憶體對齊與運算子、以及 MomentumSketch 命中、衰減、歸零替換與 32-bit/64-bit 溢位截斷與保護機制。

## 🚀 快速開始 (Quick Start)

### 執行單元測試 (Unit Tests)
```bash
cd tests
cmake -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

### 生成 50MB 重尾分佈資料集 (Synthetic Heavy-Tail Dataset)
```bash
cd Momentum-Sketch/CPU
python3 generate_dataset.py
```

詳細單元測試說明請參考 [`tests/README.md`](tests/README.md)。
