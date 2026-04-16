# Rules 基礎: ワークスペース・ファイルレベルの指示

## 📝 Rules とは？

**Rules** (`.instructions.md` ファイル)は Copilot に次のように指示するテキストガイドラインです：
- このプロジェクトが守るコーディング標準
- ファイル組織の原則
- 言語特化のイディオムと制約
- トレードオフと設計決定

Copilot はこれらのガイドラインに従う提案を優先しますが、**強制されません** — 違反提案もあります（優先度は低い）。

---

## 🏢 スコープ: ワークスペース級 vs ファイル級

### ワークスペース級ルール

**ファイル**: `copilot-instructions.md`

**適用範囲**: ワークスペース内のすべてのファイル（グローバルデフォルト）

**例コンテンツ**:
```markdown
# C++ + Qt ワークスペースガイドライン

## コア原則
1. C++17 以降を使用
2. Qt 6.x フレームワーク
3. モダン C++ イディオムを優先（スマートポインタ、auto、range-for ループ）

## 命名規則
- クラス: PascalCase (DataProcessor, MainWindow)
- 関数: camelCase (processData, setupConnections)
- 定数: UPPER_SNAKE_CASE (MAX_BUFFER_SIZE)
- メンバ変数: m_camelCase (m_data, m_count)

## メモリ管理
- スマートポインタを使用（std::unique_ptr, std::shared_ptr）
- RAII 原則に従う
- 生ポインタ所有権ゼロ
```

### ファイル級ルール

**ファイル**: `.instructions.md` (同じディレクトリまたは `applyTo` パターン付き)

**適用範囲**: 特定ファイルパターン（`applyTo` glob 経由）

**例**:
```markdown
---
name: cpp-file-guidelines
description: "C++ ファイルガイドライン（const 正確性・RAII・メモリ管理）"
applyTo: "**/*.cpp"
---

# C++ 実装ファイルガイドライン

## ヘッダインクルード
1. システムヘッダ最初（<vector> など）
2. Qt ヘッダ次（<QWidget> など）
3. プロジェクトヘッダ最後
4. セクション間は空行で分離

## 関数サイズ
- 50 行以下に保つ
- 関連する 3+ ステートメントはヘルパー関数に抽出

## メモリ割り当て
- コンストラクタ: リソース取得
- デストラクタ: リソース解放
- ムーブセマンティクスに従う
```

---

## 🔧 実例: Before ＆ After

### **Before** (Rules なし)

```cpp
// パターン明確ではなく、Copilot は一般的な Q* 使用を提案
class DataWidget : public QWidget {
public:
    DataWidget(QWidget *parent = nullptr) : QWidget(parent) {
        data = new QVector<float>;
        processor = new DataProcessor;
    }
    
    ~DataWidget() {
        delete data;
        delete processor;
    }
    
private:
    QVector<float> *data;
    DataProcessor *processor;
};
```

**問題点**:
- 所有権セマンティクス明確ではない
- 生ポインタで手動 delete
- モダン C++ パターンなし

### **After** (Rules 適用)

```cpp
// Rules: Copilot がスマートポインタ + const 正確性を優先
class DataWidget : public QWidget {
Q_OBJECT

public:
    explicit DataWidget(QWidget *parent = nullptr);
    ~DataWidget() = default;  // RAII + スマートポインタ = 自動クリーンアップ

private:
    std::unique_ptr<QVector<float>> m_data;
    std::unique_ptr<DataProcessor> m_processor;
};
```

**なぜ改善**:
- Rules 明示: 「スマートポインタ使用、手動 delete は不可」
- Copilot は `delete` パターンを優先度下げ
- コンパイラが RAII ゼロコスト強制

---

## 📦 Rules を作るべき時

| 状況 | 作成? | 理由 |
|------|------|------|
| プロジェクトは Qt 6 のみ | ✅ はい | すべての開発者が知るべき |
| const 参照使用、コピーではない | ✅ はい | メモリ効率重要 |
| ファイルサイズ上限 800 行 | ✅ はい | 保守性の境界 |
| ベクター使用・リスト時々使用 | ❌ いいえ | コンテキスト依存、ルールではない |
| clang-format で整形 | ⚠️ 部分 | Rule より Hook が最適 |

---

## 🎯 効果的なルール記述

### ✅ 良いルール

```markdown
## Qt シグナル/スロット接続

常に PMF（ポインタ-トゥ-メンバ-ファンクション）シンタックス使用:

connect(sender, &Source::sig, receiver, &Receiver::slot);

古い文字列シンタックスは絶対ダメ:

connect(sender, SIGNAL(sig()), receiver, SLOT(slot()));
```

**なぜ**: 明確・実行可能・単一パターン。

### ❌ 悪いルール

```markdown
## Qt パターン

適切な時は Qt シグナル使用。シンプルに保つが高度にも。
Qt 規則に従うが、モダンに。接続時々カスタムオブジェクト...
```

**なぜ**: 曖昧・矛盾・複数解釈可能。

---

## 🗂️ Rules 用プロジェクト構造

```
github-copilot-demo/
├── .github/
│   ├── copilot-instructions.md       # ★ ワークスペース級
│   └── instructions/
│       ├── cpp-files.instructions.md        # C++ ファイル
│       ├── cmake-build.instructions.md      # CMakeLists.txt
│       └── signal-slot.instructions.md      # シグナル/スロット
```

各ファイルは YAML frontmatter を含み、`applyTo` で適用ファイルパターンを指定します。

---

## ✅ 次のステップ

→ `docs/02-skills-basics.md` で **Skills** の役割を学ぶ
