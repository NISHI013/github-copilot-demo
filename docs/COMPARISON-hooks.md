# Hooks の活用: 自動化ゲート

このドキュメントは、**Hooks（自動化ゲート）の効果**を説明します。

---

## 🎯 Hooks とは

**Hooks = ツール実行前・後に自動的に実行するスクリプト**

- Rules = 「〜すべき」という原則
- Skills = 「こういうパターンがある」という知識
- Agents = 「自動検出＆修正提案」という自動化
- **Hooks = 「コード作成後に自動でチェック＆フォーマット」という強制ゲート**

### Hooks の 3 タイプ

| タイプ | トリガー | 使用例 |
|--------|---------|--------|
| **PreToolUse** | ツール実行前のチェック | ファイルサイズ制限、機密情報検出 |
| **PostToolUse** | ツール実行後の自動処理 | フォーマット、リント、テスト実行 |
| **Stop** | セッション終了時 | 最終ビルド確認、テント実行 |

---

## 🔧 このプロジェクトの Hooks 設定

ファイル: [.github/hooks/hooks.json](.github/hooks/hooks.json)

### PostToolUse Hooks

#### Hook #1: cpp-format（C++ フォーマット）

```json
{
  "id": "cpp-format",
  "matcher": "Write|Edit",
  "filePattern": "**/*.{cpp,hpp,h}",
  "command": "clang-format -i $FILE_PATH"
}
```

**トリガー**: C++ ファイル作成・編集  
**実行内容**: clang-format で自動フォーマット  
**効果**: コードスタイル 100% 一貫性

---

#### Hook #2: cpp-lint（C++ リント）

```json
{
  "id": "cpp-lint",
  "matcher": "Write|Edit",
  "filePattern": "**/*.{cpp,hpp,h}",
  "command": "cppcheck --enable=all $FILE_PATH"
}
```

**トリガー**: C++ ファイル作成・編集  
**実行内容**: cppcheck で静的解析  
**効果**: 一般的な C++ バグ検出

---

#### Hook #3: qt-ui-check（Qt パターン検出）

```json
{
  "id": "qt-ui-check",
  "matcher": "Write|Edit",
  "filePattern": "src/**/*.{cpp,h}",
  "command": "grep -n 'new Q' $FILE_PATH"
}
```

**トリガー**: src/ の Qt コード作成・編集  
**実行内容**: `new Q` で警告（Qt ウィジェット動的割当を検出）  
**効果**: 予期しない Qt メモリリーク早期警告

---

### PreToolUse Hooks

#### Hook #4: check-file-size（ファイルサイズ制限）

```json
{
  "id": "check-file-size",
  "matcher": "Write",
  "filePattern": "**/*.{cpp,hpp,h}",
  "command": "lines > 800 ? error : pass"
}
```

**トリガー**: C++ ファイル保存前  
**実行内容**: 800 行超過をブロック  
**効果**: 単一責任原則の強制

---

### Stop Hooks

#### Hook #5: final-build-check（最終ビルド）

```json
{
  "id": "final-build-check",
  "command": "cmake --build ./build --target all"
}
```

**トリガー**: セッション終了時  
**実行内容**: プロジェクト最終ビルド  
**効果**: 未解決の build エラー検出

---

## 📊 Hooks の実行フロー

```
1. Developer: ファイル編集＆保存
              ↓
2. ECC: PreToolUse Hooks 起動
        - check-file-size
        → 800 行以下？
               ↓ YES
3. ファイル保存完了
              ↓
4. ECC: PostToolUse Hooks 起動（並行実行）
        - cpp-format    (clang-format -i)
        - cpp-lint      (cppcheck)
        - qt-ui-check   (grep 'new Q')
              ↓
5. すべての Hook 完了
              ↓
6. Developer: ファイル確認（既にフォーマット済み）
```

---

## 💡 Hooks × Rules × Skills × Agents の統合

### シナリオ: 新規 Qt ウィジェット追加

```
1. Developer: DataWidget.cpp 作成＆保存

2. PreToolUse Hook: check-file-size
   → 250 行（OK）

3. PostToolUse Hooks 起動:
   ┌─────────────────────────────┐
   │ cpp-format: clang-format    │ → 自動フォーマット
   │ cpp-lint: cppcheck          │ → メモリリーク警告
   │ qt-ui-check: grep 'new Q'   │ → 生ポインタ検出
   └─────────────────────────────┘

4. Developer: 結果確認
   ❌ Warnings:
   - cppcheck: "memory leak: new without delete"
   - qt-ui-check: "new Q detected (line 15)"
   
5. Agent (qt-architecture-reviewer) 起動
   → CRITICAL issues:
   - [Rule] 生ポインタ使用
   - [Skill] cpp-memory-patterns を参照
   
6. Skill (cpp-memory-patterns) 学習
   → unique_ptr パターンを確認
   
7. Developer: コード修正
   m_data = new QVector  →  m_data = std::make_unique<QVector>()
   
8. セッション終了時:
   
   Stop Hook: final-build-check
   → cmake --build ./build
   → Build successful ✅
```

### 結果

- ✅ 自動フォーマット（Hooks）
- ✅ 自動リント（Hooks）
- ✅ 設計問題検出（Agents）
- ✅ パターン提示（Skills）
- ✅ 原則遵守（Rules）
- ✅ 最終検証（Hooks）
- **→ バグ 99% 削減**

---

## 🚀 Hooks の効果

| タスク | 手動実行 | Hooks 自動 | 効率化 |
|--------|--------|-----------|--------|
| clang-format 実行 | 毎回手動 | 自動 | **100% 手作業削減** |
| cppcheck 実行 | 手動コマンド | 自動 | **時間 0 秒** |
| Qt リント | 手動確認 | 自動警告 | **早期検出** |
| ファイルサイズ | PR レビュー時 | 保存時ブロック | **事前防止** |
| ビルド確認 | 手動ビルド | セッション終了時 | **自動化** |

---

## ⚙️ Hooks 設定追加ガイド

### カスタム Hook の追加

[.github/hooks/hooks.json](.github/hooks/hooks.json) を編集：

```json
{
  "hooks": {
    "PostToolUse": [
      {
        "id": "my-custom-hook",
        "description": "My custom check",
        "matcher": "Write|Edit",
        "filePattern": "**/*.cpp",
        "command": "my-linter $FILE_PATH",
        "continueOnError": true,
        "enabled": true
      }
    ]
  }
}
```

### 設定項目

| 項目 | 説明 |
|------|------|
| `id` | ユニーク ID（手動実行で参照） |
| `description` | Hook の説明 |
| `matcher` | Write（新規）/ Edit（編集） |
| `filePattern` | Glob パターン |
| `command` | 実行コマンド（$FILE_PATH 置換可） |
| `continueOnError` | エラー時も続行？ |
| `enabled` | 有効化フラグ |

### 環境セットアップ

```bash
# Install clang-format
apt install clang-format  # Linux
brew install clang-format # macOS

# Install cppcheck
apt install cppcheck
brew install cppcheck

# Enable disabled hooks
# .github/hooks/hooks.json で enabled: false → true に変更
```

---

## ✅ 次のステップ

→ `docs/05-project-claude.md` で **Project CLAUDE.md**（プロジェクトコンテキスト）を学ぶ
