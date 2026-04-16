# Skills 基礎: ドメイン特化の専門知識

## 📚 Skills とは？

**Skills** は、特定のドメイン・パターン・技術に関する構造化されたナレッジパッケージです。

Rules が「守るべき規則」なら、Skills は「ベストプラクティスとパターンの地図」です。

---

## 🎯 Rules vs Skills

| 特性 | Rules | Skills |
|------|-------|--------|
| **役割** | 制約・ガイドライン | 知識・パターン参考 |
| **適用** | 自動・常時 | 参照型（明示的または検出） |
| **詳細度** | 簡潔 | 詳細（理論 + 複数例） |
| **内容** | 「何をすべきか」 | 「どうやるのか」+「なぜか」 |
| **使用例** | 「const 正確性を使う」| 「スマートポインタ所有権パターン」|

---

## 📦 Skill ファイル構造

```
.github/skills/qt-ui-patterns/
├── SKILL.md                    # メインスキル定義
└── examples/                   # オプション: コード例

.github/skills/cpp-memory-patterns/
├── SKILL.md
```

### SKILL.md テンプレート

```yaml
---
name: qt-ui-patterns
description: "Qt ウィジェット合成・レイアウトパターン・レスポンシブ UI デザイン"
origin: demo-project
---

# Qt UI パターン

## 使用時

- ウィジェット階層とレイアウト設計
- 異なる画面サイズへのレスポンシブ UI
- 複数セクションの複雑 UI 組織化

## パターン例

### ウィジェット合成パターン

[C++ コード例]

### レイアウトベストプラクティス

[詳細説明 + コード例]
```

---

## 🎓 デモプロジェクトの Skills

このプロジェクトは **4 つの核心 Skills** を提供します：

### 1. **qt-ui-patterns** 

ウィジェット組織化・レイアウト・レスポンシブデザイン

```cpp
// パターン例: メインウィンドウ構成
class MainWindow : public QMainWindow {
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    
    // セクション
    std::unique_ptr<DataPanel> m_dataPanel;
    std::unique_ptr<VisualizationPanel> m_vizPanel;
};
```

### 2. **cpp-memory-patterns**

スマートポインタ・RAII・所有権セマンティクス

```cpp
// unique_ptr で排他的所有権
std::unique_ptr<DataProcessor> m_processor;

// shared_ptr は必要時のみ
std::shared_ptr<CacheManager> m_cache;
```

### 3. **qt-signal-slot-design**

PMF シンタックス・効率的接続・メモリ安全性

```cpp
// ✅ PMF シンタックス（推奨）
connect(source, &Source::valueChanged, this, &Widget::onValueChanged);

// ❌ 文字列シンタックス（非推奨）
connect(source, SIGNAL(valueChanged()), ...);
```

### 4. **error-handling**

C++ 例外・Qt エラーパターン・リソースクリーンアップ

```cpp
try {
    processor.loadData("data.txt");
} catch (const std::exception &e) {
    qWarning() << "Error:" << QString::fromStdString(e.what());
}
```

---

## 💡 Skills を使う方法

### 方法 1: 自動参照
Copilot がパターン検出时自動で関連 Skill を参照

### 方法 2: 明示的参照
```
あなた: "qt-signal-slot-design skill を使ってシグナル接続を最適化してください"

Copilot: [Skill コンテンツを取得し、パターンを適用した実装を提案]
```

### 方法 3: スラッシュコマンド
VS Code チャットで `/qt-ui-patterns` でスキルアクセス

---

## ✅ 次のステップ

→ `docs/03-agents-basics.md` で **Agents** の役割を学ぶ
