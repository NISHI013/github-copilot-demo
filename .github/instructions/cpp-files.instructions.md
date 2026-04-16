---
name: cpp-files-guidelines
description: "C++ implementation files guidelines with focus on const correctness, RAII, memory safety, and modern C++ patterns."
applyTo: "**/*.cpp"
---

# C++ 実装ファイル（.cpp）ガイドライン

このファイルは `.cpp` ファイル専用のガイドラインです。`**/*.cpp` に適用されます。

## 📌 インクルード順序

1. **システムヘッダ** （`<vector>`, `<memory>`, `<fstream>`）
2. **Qt ヘッダ** （`<QWidget>`, `<QVector>`）
3. **プロジェクトヘッダ** （`"DataProcessor.h"`, `"MainWindow.h"`）

セクション間は空行で分離します。

```cpp
#include <vector>
#include <memory>
#include <fstream>

#include <QWidget>
#include <QVector>
#include <QPushButton>

#include "DataProcessor.h"
#include "MainWindow.h"
```

---

## 📏 関数サイズ

- **最大 50 行** — 典型的な関数
- **関連する 3+ ステートメント** → ヘルパー関数に抽出

```cpp
// ❌ 長い関数（50 行以上）
void MainWindow::loadAndProcessData(const QString &filename) {
    std::ifstream file(filename.toStdString());
    if (!file) { qWarning() << "Cannot open"; return; }
    
    QVector<float> data;
    float value;
    while (file >> value) {
        data.push_back(value);
    }
    file.close();
    
    // ... 処理（20+ 行）
    // ... さらに処理（15+ 行）
}

// ✅ ヘルパー関数に分割
bool MainWindow::loadDataFile(const QString &filename, QVector<float> &data) {
    std::ifstream file(filename.toStdString());
    if (!file) { qWarning() << "Cannot open"; return false; }
    
    float value;
    while (file >> value) {
        data.push_back(value);
    }
    return true;
}

void MainWindow::processLoadedData(const QVector<float> &data) {
    // 処理...
}
```

---

## 💾 RAII とリソース管理

### コンストラクタ: リソース取得

```cpp
class DataWidget {
public:
    DataWidget(QWidget *parent = nullptr)
        : QWidget(parent)
        , m_data(std::make_unique<QVector<float>>())
        , m_processor(std::make_unique<DataProcessor>())
    {
        setupUI();
    }
};
```

### デストラクタ: リソース解放

```cpp
// ❌ 手動削除（非 RAII）
~DataWidget() {
    delete m_data;
    delete m_processor;
}

// ✅ スマートポインタ自動管理
~DataWidget() = default;  // または明示的削除は不要
```

### リソース直接割り当て禁止

```cpp
// ❌ 禁止: new/delete の混在
QVector<float> *buffer = new QVector<float>;
// ... 使用...
delete buffer;

// ✅ 推奨: スマートポインタ
auto buffer = std::make_unique<QVector<float>>();
// ... 使用...
// 自動削除（スコープ終了時）
```

---

## 🔍 const 正確性

### メンバ関数

```cpp
class DataProcessor {
public:
    // ❌ const なし（だが状態を変更しない）
    QVector<float> getData() {
        return m_data;
    }
    
    // ✅ const メンバ + const 参照返却
    const QVector<float>& getData() const {
        return m_data;
    }
};
```

### パラメータ

```cpp
// ❌ 値コピー（高コスト + const なし）
void process(QVector<float> data) { }

// ✅ const 参照（無コスト + 意図明確）
void process(const QVector<float> &data) { }

// 修正する場合のみ参照（const なし）
void modify(QString &value) {
    value.append("_modified");
}
```

---

## 🔄 初期化リスト

```cpp
// ❌ 体内初期化（メンバ初期化遅延）
DataWidget::DataWidget(QWidget *parent) : QWidget(parent) {
    m_data = std::make_unique<QVector<float>>();
    m_count = 0;
}

// ✅ 初期化リスト（メンバ直接初期化）
DataWidget::DataWidget(QWidget *parent)
    : QWidget(parent)
    , m_data(std::make_unique<QVector<float>>())
    , m_count(0)
{
    // 本体（その他初期化）
}
```

---

## ✨ スマートポインタ パターン

```cpp
// unique_ptr: 排他的所有権
std::unique_ptr<DataProcessor> m_processor;

// shared_ptr: 複数所有者（稀）
std::shared_ptr<CacheManager> m_cache;

// 作成
m_processor = std::make_unique<DataProcessor>();
m_cache = std::make_shared<CacheManager>();

// 使用
m_processor->process(*m_data);
```

---

## ❌ アンチパターン

```cpp
// ❌ 禁止: 生ポインタ所有権
DataProcessor *m_processor = new DataProcessor();
delete m_processor;

// ❌ 禁止: printf（C-style ログ）
printf("Value: %f\n", value);

// ❌ 禁止: マジックナンバー
data[100] = value;  // 100 は何？

// ❌ 禁止: グローバル可変状態
int globalCounter = 0;  // 避けるべき

// ❌ 禁止: 深いネスト（>4 レベル）
if (condition1) {
    if (condition2) {
        if (condition3) {
            if (condition4) {
                // ネストが深すぎる！
            }
        }
    }
}
```

---

## ✅ ベストプラクティス

```cpp
// ✅ 推奨: early return（ネスト回避）
if (!data.isValid()) {
    qWarning() << "Invalid data";
    return;
}
// 有効なデータ処理...

// ✅ 推奨: range-for ループ
for (const auto &value : m_data) {
    process(value);
}

// ✅ 推奨: auto 型推論
auto processor = std::make_unique<DataProcessor>();

// ✅ 推奨: Qt ロギング
qDebug() << "Data loaded:" << m_data.size() << "items";
qWarning() << "Error processing data";
```
