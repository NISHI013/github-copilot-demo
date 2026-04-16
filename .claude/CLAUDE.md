# CLAUDE.md - プロジェクトコンテキスト

このファイルは、Claude Code（GitHub Copilot など）がプロジェクト全体を理解するための**統一されたコンテキスト**です。

---

## 📋 プロジェクト概要

**プロジェクト名**: GitHub Copilot カスタマイズサンプル — C++ + Qt  
**目的**: GitHub Copilot の 5 つのカスタマイズ要素（Rules、Skills、Agents、Hooks、CLAUDE.md）の実装デモ  
**対象技術**: C++17+、Qt 6.x、CMake 3.22+  
**学習対象**: 初心者～上級者

---

## 🎯 5 つのカスタマイズ要素

### 1. Rules（原則・ガイドライン）
**ファイル**: `.github/copilot-instructions.md` + `.github/instructions/*.md`  
**役割**: コードスタイル・原則を定義  
**例**:
- メモリ管理: `unique_ptr` が基本
- const 正確性: パラメータ・戻り値・メンバ関数に適用
- シグナル: PMF シンタックスのみ

→ 詳細は [`docs/01-rules-basics.md`](../docs/01-rules-basics.md)

---

### 2. Skills（ドメイン知識パッケージ）
**ファイル**: `.github/skills/*/SKILL.md`  
**役割**: パターン・テンプレート・ベストプラクティス  
**このプロジェクトのスキル**:
- `qt-ui-patterns` — Qt ウィジェット構成
- `cpp-memory-patterns` — スマートポインタ・RAII
- `qt-signal-slot-design` — PMF 接続パターン
- `error-handling` — 例外・ロギング・リカバリー

→ 詳細は [`docs/02-skills-basics.md`](../docs/02-skills-basics.md)

---

### 3. Agents（特化した自動ワークフロー）
**ファイル**: `.github/agents/*.agent.md`  
**役割**: 自動検出・修正提案  
**このプロジェクトのエージェント**:
- `qt-architecture-reviewer` — Qt 設計・メモリレビュー
- `cpp-refactorer` — C++ 効率化・リファクタリング

→ 詳細は [`docs/03-agents-basics.md`](../docs/03-agents-basics.md)

---

### 4. Hooks（自動化ゲート）
**ファイル**: `.github/hooks/hooks.json`  
**役割**: ツール実行前後・セッション終了時の自動処理  
**このプロジェクトのフック**:
- PreToolUse: ファイルサイズ制限チェック
- PostToolUse: clang-format、cppcheck 自動実行
- Stop: セッション終了時のビルド確認

→ 詳細は [`docs/04-hooks-basics.md`](../docs/04-hooks-basics.md)

---

### 5. Project CLAUDE.md（プロジェクトコンテキスト）
**ファイル**: `.claude/CLAUDE.md` （ **このファイル** ）  
**役割**: プロジェクト全体の状況・設定を統一  
**効果**: Copilot が各セッション・各ファイルで一貫したコンテキスト保持

---

## 📁 ディレクトリ構造

```
github-copilot-demo/
├── .claude/
│   └── CLAUDE.md                      # ← このファイル
├── .github/
│   ├── copilot-instructions.md        # Rules（ワークスペース）
│   ├── instructions/                  # Rules（ファイル特化）
│   │   ├── cpp-files.instructions.md
│   │   ├── cmake-build.instructions.md
│   │   └── signal-slot.instructions.md
│   ├── skills/                        # Skills（ドメイン知識）
│   │   ├── qt-ui-patterns/SKILL.md
│   │   ├── cpp-memory-patterns/SKILL.md
│   │   ├── qt-signal-slot-design/SKILL.md
│   │   └── error-handling/SKILL.md
│   ├── agents/                        # Agents（自動ワークフロー）
│   │   ├── qt-architecture-reviewer.agent.md
│   │   └── cpp-refactorer.agent.md
│   └── hooks/
│       └── hooks.json                 # Hooks（自動化ゲート）
├── docs/
│   ├── 00-overview.md                 # 全要素の概要
│   ├── 01-rules-basics.md             # Rules 説明
│   ├── 02-skills-basics.md            # Skills 説明
│   ├── 03-agents-basics.md            # Agents 説明
│   ├── 04-hooks-basics.md             # Hooks 説明
│   ├── 05-project-claude.md           # CLAUDE.md 説明
│   ├── COMPARISON-rules.md            # Before/After: Rules
│   ├── COMPARISON-skills.md           # Before/After: Skills
│   ├── COMPARISON-agents.md           # Before/After: Agents
│   ├── COMPARISON-hooks.md            # Before/After: Hooks
│   ├── 06-agents-usage.md             # Agents 使用ガイド
│   ├── WALKTHROUGH.md                 # 実践ガイド
│   └── 10-complete-reference.md       # リファレンス
├── examples/
│   ├── before-customization/          # ❌ 非最適コード
│   │   ├── before-main-window.cpp
│   │   └── before-data-processor.cpp
│   └── after-customization/           # ✅ 最適化コード
│       ├── after-main-window.cpp
│       └── after-data-processor.cpp
├── src/                               # （スケルトン）
├── include/                           # （スケルトン）
├── tests/                             # （スケルトン）
├── README.md                          # プロジェクト入口
└── CMakeLists.txt                     # CMake 設定
```

---

## 🔄 推奨される学習進行

### 初心者向け（開発経験あり、Copilot 初心者）

1. [`README.md`](../README.md) — プロジェクト概要
2. [`docs/00-overview.md`](../docs/00-overview.md) — 5 要素の鳥瞰図
3. [`docs/01-rules-basics.md`](../docs/01-rules-basics.md) — Rules（原則）
4. [`examples/before-customization/`](../examples/before-customization/) — コード比較開始
5. [`docs/02-skills-basics.md`](../docs/02-skills-basics.md) — Skills（パターン）
6. [`examples/after-customization/`](../examples/after-customization/) — 最適化コード確認

### 中級者向け（Copilot 経験あり）

1. [`docs/COMPARISON-rules.md`](../docs/COMPARISON-rules.md) — Rules 効果実感
2. [`docs/COMPARISON-skills.md`](../docs/COMPARISON-skills.md) — Skills 活用例
3. `.github/skills/*/` — 各スキルを読破
4. [`docs/03-agents-basics.md`](../docs/03-agents-basics.md) — Agents 機能
5. [`docs/COMPARISON-agents.md`](../docs/COMPARISON-agents.md) — Agents の自動化実感

### 上級者向け（全機能カスタマイズ）

1. `.github/copilot-instructions.md` — Rules 全体像
2. `.github/instructions/` — ファイル特化 Rules
3. `.github/agents/` — Agent 定義カスタマイズ
4. `.github/hooks/hooks.json` — Hook カスタマイズ
5. [`docs/WALKTHROUGH.md`](../docs/WALKTHROUGH.md) — エンドツーエンド実装

---

## 🛠 Copilot との統合フロー

### シナリオ 1: 新規 Qt ウィジェット開発

```
1. Developer: "Qt ウィジェットを追加したい"
              ↓
2. Copilot が Rules を確認
   → メモリ: unique_ptr
   → const: 参照パラメータ
   → シグナル: PMF
              ↓
3. Copilot が Skills を参照
   → qt-ui-patterns
   → cpp-memory-patterns
   → qt-signal-slot-design
              ↓
4. Copilot: コード提案生成
   ✅ スマートポインタ
   ✅ PMF シンタックス
   ✅ const パラメータ
              ↓
5. Hooks 自動実行
   ✅ clang-format
   ✅ cppcheck
              ↓
6. Agent 実行（オプション）
   ✅ qt-architecture-reviewer → 設計チェック
   ✅ cpp-refactorer → 効率化提案
```

---

### シナリオ 2: バグ修正時

```
1. Developer: "メモリリークが報告された"
              ↓
2. Agent (qt-architecture-reviewer) 実行
   → 生ポインタ検出
   → 手動 delete 検出
              ↓
3. Copilot が Skill 提案
   → cpp-memory-patterns
   → error-handling
              ↓
4.修正提案
   ✅ unique_ptr に置き換え
   ✅ RAII パターン適用
              ↓
5. Hooks 実行 → ビルド確認
```

---

## 📚 ドキュメント一覧

| ファイル | 対象 | 目的 |
|---------|------|------|
| `00-overview.md` | 全員 | 5 要素の概観 |
| `01-rules-basics.md` | 初心者 | Rules 理解 |
| `02-skills-basics.md` | 初心者 | Skills 理解 |
| `03-agents-basics.md` | 中級者 | Agents 理解 |
| `04-hooks-basics.md` | 中級者 | Hooks 理解 |
| `05-project-claude.md` | 全員 | CLAUDE.md 理解 |
| `COMPARISON-rules.md` | 初心者 | Before/After: Rules |
| `COMPARISON-skills.md` | 初心者 | Before/After: Skills |
| `COMPARISON-agents.md` | 中級者 | Agents 効果 |
| `COMPARISON-hooks.md` | 中級者 | Hooks 効果 |
| `06-agents-usage.md` | 上級者 | Agent 実装 |
| `WALKTHROUGH.md` | 上級者 | エンドツーエンド |
| `10-complete-reference.md` | 全員 | リファレンス |

---

## ⚙️ 設定項目

### C++ 標準
- **要求**: C++17 以上
- **推奨**: C++20
- **CMake**: `set(CMAKE_CXX_STANDARD 17)`

### Qt バージョン
- **要求**: Qt 6.2 以上
- **コンポーネント**: Core、Gui、Widgets、Sql

### ビルドシステム
- **CMake**: 3.22 以上
- **コンパイラ**: GCC 10+、Clang 12+、MSVC 2022+

### 開発環境
- **OS**: Linux、macOS、Windows（WSL 推奨）
- **IDE**: VS Code（推奨）、Qt Creator、CLion

---

## 🔐 コード品質ガイドライン

### 強制事項

- ✅ メモリ: `unique_ptr` が基本
- ✅ シグナル: PMF シンタックスのみ
- ✅ ログ: Qt ロギングシステム（printf 禁止）
- ✅ const: 参照・メンバ関数に適用
- ✅ ファイル: 800 行以下

### 推奨事項

- ✅ Range-for ループ（C スタイルループ廃止）
- ✅ std::make_unique/make_shared
- ✅ RAII（リソース管理）
- ✅ 例外ベースエラーハンドリング

### テスト

- ✅ 最小カバレッジ: 80%
- ✅ Unit + Integration + E2E
- ✅ TDD 推奨（テスト先行開発）

---

## 📞 サポート

### ドキュメント不足時

→ `docs/WALKTHROUGH.md` で実装例を検索  
→ `docs/10-complete-reference.md` でリファレンス確認  
→ `.github/skills/*/SKILL.md` で Pattern 検索

### バグ・改善提案

→ プロジェクト README.md の「貢献」セクション確認

---

## ✅ チェックリスト

### セットアップ時

- [ ] `.github/` ディレクトリが存在
- [ ] `copilot-instructions.md` が読み込まれた
- [ ] `.github/skills/*/SKILL.md` が認識された
- [ ] `.github/agents/` が認識された
- [ ] `.github/hooks/hooks.json` が有効

### コード作成時

- [ ] Rules に沿った命名・スタイル
- [ ] Skills でパターン確認
- [ ] Agents で自動チェック
- [ ] Hooks で自動フォーマット

### セッション終了時

- [ ] Stop Hook でビルド確認
- [ ] テスト実行（カバレッジ 80%+）

---

**最終更新**: 2025-01-09  
**バージョン**: 1.0  
**対象**: Copilot、Claude、LLM ベース統合
