# Project CLAUDE.md: プロジェクトコンテキスト統合

## 📖 Project CLAUDE.md とは？

**Project CLAUDE.md** は、プロジェクト全体を Copilot に「ブリーフ」する単一・統合ファイルです。

Rules/Skills/Agents では提供できない、**深い文脈情報**を一元管理します。

---

## 📍 配置と読み込み

**ファイル位置**: `.claude/CLAUDE.md`

**読み込みタイミング**: セッション開始時（1 回）

**スコープ**: プロジェクト全体（常に有効）

---

## 📋 CLAUDE.md の典型的内容

### 1. **プロジェクト概要**

```markdown
# Data Visualization Desktop App

- Type: Qt 6 Desktop Application
- Language: C++17+
- Purpose: Real-time sensor data visualization
- Maintainer: Team X
```

### 2. **アーキテクチャ図**（ASCII または Mermaid）

```
┌──────────────────────────────────────┐
│        Main Window                   │
├──────────────────────────────────────┤
│  Data Panel  │  Visualization Panel  │
├──────────────────────────────────────┤
│        Control Panel                 │
└──────────────────────────────────────┘
           │
           ▼
    ┌────────────────────┐
    │  DataProcessor     │
    │  (Core Logic)      │
    └────────────────────┘
           │
           ├─ File I/O
           ├─ Data Transform
           └─ Caching
```

### 3. **ディレクトリ構造**

```
project/
├── src/
│   ├── MainWindow.cpp/h
│   ├── DataPanel.cpp/h
│   └── DataProcessor.cpp/h
├── include/
├── tests/
├── docs/
└── CMakeLists.txt
```

### 4. **ビルド・デプロイ手順**

```bash
# ビルド
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

# テスト実行
ctest --output-on-failure

# デプロイ
make install
```

### 5. **キー依存関係・バージョン**

```
Dependency      Version    Purpose
Qt              6.5+       GUI Framework
CMake           3.22+      Build System
C++ Standard    C++17      Language
pkg-config      1.8+       Library Discovery
```

### 6. **一般的開発タスク**

```markdown
## 新機能追加

1. Feature branch 作成: `git checkout -b feature/xxx`
2. src/ に .cpp/.h ファイル追加
3. CMakeLists.txt にソース追加
4. ビルド検証: `cmake --build .`
5. PR 作成

## バグ修正

1. Bugfix branch 作成
2. src/ で該当ファイル編集
3. テスト追加: tests/ に .cpp ファイル
4. `ctest` で検証
5. PR 作成

## メモリリーク検査

valgrind ./build/app-binary
```

### 7. **トラブルシューティング FAQ**

```markdown
## Q: ビルドが CMake エラーで失敗

A: `rm -rf build && cmake -B build` で再生成

## Q: Qt 6 が見つからない

A: `export Qt6_DIR=/path/to/qt6` で環境指定

## Q: リンクエラー（未解決シンボル）

A: CMakeLists.txt 内 `target_link_libraries()` を確認
```

---

## 💡 Rules/Skills と CLAUDE.md の違い

| 要素 | Rules | Skills | CLAUDE.md |
|------|-------|--------|-----------|
| **内容** | コーディング規則 | パターン・知識 | プロジェクト文脈 |
| **更新頻度** | 低（ガイドライン） | 低（知識） | 中（プロジェクト変更時） |
| **対象** | コード生成時 | 参照時 | セッション開始時 |
| **例** | 「const 使用」| 「スマートポインタパターン」| 「CMakeLists.txt ビルドコマンド」|

---

## 🎯 効果的な CLAUDE.md の要点

### ✅ 含めるべき

- アーキテクチャ図（概要だけ）
- ビルド・テスト・デプロイ手順
- キー依存関係
- 一般的な開発ワークフロー
- 既知の制限事項
- FAQ（よくある問題）

### ❌ 含めるべきではない

- 詳細なコーディング規則（→ Rules）
- 設計パターン詳細（→ Skills）
- 個別ファイルの詳細（→ コメント）
- 大規模ドキュメント（→ wiki）

---

## 📊 CLAUDE.md サイズ目安

- **最小**: 1 ページ（概要 + ビルド手順）
- **推奨**: 2-3 ページ（+ アーキテクチャ + FAQ）
- **最大**: 5 ページ（詳細シナリオ可）

**理由**: Copilot のコンテキストウィンドウを効率的に使用

---

## ✅ 次のステップ

→ `docs/COMPARISON-rules.md` で **Rules 適用の効果** を実邦例で確認
