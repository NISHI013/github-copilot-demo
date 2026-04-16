---
name: cpp-qt-workspace-guidelines
description: "Workspace-level Copilot instructions for C++ + Qt projects. Covers const correctness, smart pointers, RAII, signal/slot patterns, and code organization best practices."
---

# C++ + Qt ワークスペースガイドライン

このファイルは、プロジェクト全体 Copilot の動作をガイドする基本指針です。

## 🎯 コア原則

1. **C++17 以上** — モダン C++ 機能を活用
2. **Qt 6.x フレームワーク** — 最新 Qt バージョン
3. **型安全第一** — const 正確性・強力な型
4. **メモリ安全** — スマートポインタ・RAII・生ポインタ所有権ゼロ
5. **意図明確** — 自己説明的なコード

---

## 💾 メモリ管理

### スマートポインタの使法

- **`std::unique_ptr`** — 排他的所有権。デフォルトで使用
- **`std::shared_ptr`** — 複数所有者が必要な場合のみ
- **生ポインタ** — 所有目的で絶対使用禁止。参照用のみ

### RAII 原則

```cpp
// ✅ 正しい: RAII パターン
class DataManager {
public:
    DataManager() : m_data(std::make_unique<QVector<float>>()) {}
    ~DataManager() = default;  // スマートポインタが自動クリーンアップ
    
private:
    std::unique_ptr<QVector<float>> m_data;
};

// ❌ 間違い: 手動メモリ管理
class BadDataManager {
public:
    BadDataManager() { m_data = new QVector<float>; }
    ~BadDataManager() { delete m_data; }
    
private:
    QVector<float> *m_data;
};
```

---

## 🔌 Qt シグナル/スロット パターン

### PMF（Pointer-to-Member-Function）シンタックス（必須）

```cpp
// ✅ 正しい: PMF シンタックス（コンパイル時型チェック）
connect(dataSource, &DataSource::valueChanged,
        this, &DataWidget::onValueChanged);

// ならって複数引数オーバーロード
connect(dataSource, QOverload<int, float>::of(&DataSource::dataUpdated),
        this, &DataWidget::onDataUpdated);
```

### 古い文字列シンタックス（禁止）

```cpp
// ❌ 絶対禁止: 文字列シンタックス（型チェックなし・実行時エラー可能）
connect(dataSource, SIGNAL(valueChanged()), this, SLOT(onValueChanged()));
```

### 接続のベストプラクティス

- 親-子所有権を活用（自動クリーンアップ）
- 接続タイプ明示（Direct、Queued など）
- 循環接続回避（A → B → A）
- ラムダは最小化（接続を明示的に保つ）

---

## 📁 コード組織

### ファイルサイズ制限

- **最大 800 行** — 単一ファイル
- **関数 50 行以下** — 平均
- **1 ファイル 1 クラス** — または密接関連クラス

### ディレクトリ構造

```
src/
├── MainWindow.cpp/h       # 主ウィンドウ
├── DataPanel.cpp/h        # データパネル
├── DataProcessor.cpp/h    # データ処理コア
└── CMakeLists.txt

include/
├── DataProcessor.h
└── ...

tests/
└── ...
```

---

## 📝 命名規則

### 規約

| 要素 | 形式 | 例 |
|------|------|-----|
| クラス | PascalCase | `DataProcessor`, `MainWindow` |
| 関数 | camelCase | `processData`, `setupConnections` |
| 定数 | UPPER_SNAKE_CASE | `MAX_BUFFER_SIZE`, `DEFAULT_PORT` |
| メンバ変数 | m_camelCase | `m_data`, `m_processCount` |
| ローカル変数 | camelCase | `tempBuffer`, `loopIndex` |
| プライベート関数 | camelCaseImpl または camelCase | `validateDataImpl`, `computeSum` |

---

## ✅ const 正確性

### 原則

- **デフォルト `const`** — 変更なしなら const
- **メンバ関数** — 状態変更なし → `const` マーク
- **パラメータ** — 変更しない → `const &` 使用
- **ポインタ** → `const T*` （変更不.vs ポインタ可変）

### 例

```cpp
class DataWidget {
public:
    // ❌ 間違い: const なし（だが変更しない）
    QVector<float> getData() {
        return m_data;
    }
    
    // ✅ 正しい: const メンバ・const ref 返却
    const QVector<float>& getData() const {
        return m_data;
    }
    
    // ❌ 間違い: 値コピー
    void loadFile(QString filename) { }
    
    // ✅ 正しい: const 参照
    void loadFile(const QString &filename) { }
    
private:
    QVector<float> m_data;
};
```

---

## 🔍 コード品質チェック

### 禁止事項

- ❌ 生ポインタ所有権（スマートポインタを使用）
- ❌ `printf` / C-style I/O（Qt ログ `qDebug`, `qWarning` 使用）
- ❌ `goto` または深いネスト（>4 レベル）
- ❌ グローバル可変状態
- ❌ 例外なしのエラー処理（例外またはチェック明示）

### 推奨パターン

- ✅ スマートポインタ + RAII
- ✅ const 正確性
- ✅ Range-for ループ
- ✅ 初期化リスト使用
- ✅ `auto` 型推論（可読性を損なわない範囲）

---

## 📖 参考

このガイドラインは以下に基づいています：
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/)
- [Qt Documentation](https://doc.qt.io/)
- Effective Modern C++ (Scott Meyers)
