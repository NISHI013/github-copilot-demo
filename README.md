# GitHub Copilot カスタマイズ デモプロジェクト

GitHub Copilot のカスタマイズ要素を、実作業的な C++ + Qt プロジェクトで段階的に学習できる包括的なデモプロジェクトです。

## 📋 プロジェクト概要

このプロジェクトは、**データビジュアライザー** デスクトップアプリケーションを C++ と Qt で構築しながら、GitHub Copilot のカスタマイズ要素を段階的に適用します：

- **Rules（ルール）** — ワークスペース・ファイルレベルのガイドライン
- **Skills（スキル）** — ドメイン特化の専門知識とパターン
- **Agents（エージェント）** — 特化した審査・リファクタリングワークフロー
- **Hooks（フック）** — 自動化されたコード品質・フォーマッティング
- **Project CLAUDE.md** — プロジェクトコンテキスト

## 🎯 学習パス

各フェーズは **before/after** コード比較で、カスタマイズ要素の効果を明示します：

| フェーズ | フォーカス | ファイル | 対象 |
|---------|----------|--------|------|
| **Phase 1** | 基礎 | ドキュメント・構造 | 初心者 |
| **Phase 2** | Rules | ガイドライン | 初心者 |
| **Phase 3** | Skills | パターン | 中級 |
| **Phase 4** | Agents | 自動レビュー | 上級 |
| **Phase 5** | Hooks | 自動化 | 上級 |
| **Phase 6-8** | Advanced | 相互作用・リファレンス | 完全網羅 |

## 🏗️ プロジェクト構成

```
github-copilot-demo/
├── README.md
├── docs/
│   ├── 00-overview.md              # カスタマイズ要素の全体像
│   ├── 01-rules-basics.md          # Rules の基本
│   ├── 02-skills-basics.md         # Skills の役割
│   ├── 03-agents-basics.md         # Agents の使い方
│   ├── 04-hooks-basics.md          # Hooks の基本
│   ├── 05-project-claude.md        # Project CLAUDE.md
│   ├── 06-agents-usage.md          # Agents 使用法
│   ├── 07-hooks-automation.md      # Hooks 自動化
│   ├── 08-project-claude.md        # 詳細説明
│   ├── 09-advanced-combinations.md # 複雑な相互作用
│   ├── 10-complete-reference.md    # リファレンス
│   ├── COMPARISON-rules.md         # Rules 前後比較
│   ├── COMPARISON-skills.md        # Skills 前後比較
│   ├── COMPARISON-agents.md        # Agents 審査比較
│   └── WALKTHROUGH.md              # インタラクティブガイド
├── examples/
│   ├── before-customization/       # 非最適化コード
│   │   ├── before-main-window.cpp
│   │   └── before-data-processor.cpp
│   └── after-customization/        # 最適化コード
│       ├── after-main-window.cpp
│       └── after-data-processor.cpp
├── src/
│   └── CMakeLists.txt              # Qt 6 ビルド設定
├── include/
├── .github/
│   ├── copilot-instructions.md (Phase 2)
│   ├── instructions/ (Phase 2)
│   ├── skills/ (Phase 3)
│   ├── agents/ (Phase 4)
│   └── hooks.json (Phase 5)
└── .claude/
    └── CLAUDE.md (Phase 6)
```

## 🚀 クイックスタート

1. **ドキュメントを読む** → `docs/00-overview.md` から開始
2. **Before コード確認** → `examples/before-customization/` を確認
3. **段階的に適用** → Phase 2 以降のカスタマイズをフォロー

## 🎓 対象レベル

- **初心者** → Phase 1～2（基本的なガイドラインの理解）
- **中級者** → Phase 3～4（パターンとワークフロー）
- **上級者** → Phase 5～8（自動化と複雑な相互作用）

## 🛠️ 技術スタック

- **言語**: C++17+
- **GUI フレームワーク**: Qt 6.x
- **ビルドシステム**: CMake 3.22+
- **Copilot カスタマイズ**: `.instructions.md`, Skills, Agents, Hooks

## 📚 重要なファイル

| ファイル | 説明 |
|--------|------|
| `docs/00-overview.md` | **ここから開始** — カスタマイズ要素の全体像 |
| `docs/01-rules-basics.md` | Rules がどう Copilot の動作を導くのか |
| `examples/before-customization/` | 非最適化のベースラインコード |
| `.github/copilot-instructions.md` | ワークスペース級のルール |

---

*Phase 1 完了: プロジェクト基礎・ドキュメント枠組み・Before 状態の例を作成*
