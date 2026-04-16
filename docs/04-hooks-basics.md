# Hooks 基礎: 自動化されたアクション

## 🪝 Hooks とは？

**Hooks** は、ファイル書き込み・編集・ビルド時に自動的に実行されるアクションです。

品質ゲートを「手動チェック」から「自動実行」に変換します。

---

## ⚙️ Hook のタイプ

### 1. **PreToolUse** — ツール実行前

```json
{
  "PreToolUse": [
    {
      "matcher": "Write",
      "command": "wc -l \"$FILE_PATH\" | awk '{if ($1 > 800) exit 1}'",
      "description": "Validate file doesn't exceed 800 lines"
    }
  ]
}
```

**用途**:
- ファイルサイズ制限検証
- CMakeLists.txt の構文チェック
- 禁止キーワードの検出（例：`delete`, `malloc`）

### 2. **PostToolUse** — ツール実行後

```json
{
  "PostToolUse": [
    {
      "matcher": "Write|Edit",
      "command": "clang-format -i \"$FILE_PATH\"",
      "description": "Auto-format C++ code"
    },
    {
      "matcher": "Write|Edit",
      "command": "cppcheck --quiet \"$FILE_PATH\"",
      "description": "Run static analysis"
    }
  ]
}
```

**用途**:
- 自動コード整形（clang-format）
- 静的解析実行（cppcheck）
- リント检查（clang-tidy）
- 形式検証（JSON、YAML）

### 3. **Stop** — セッション終了時

```json
{
  "Stop": [
    {
      "command": "cd $PROJECT_DIR && cmake --build . 2>&1 | tail -20",
      "description": "Verify final build success"
    }
  ]
}
```

**用途**:
- 最終ビルド検証
- デプロイ前チェック
- ログ集約

---

## 🔄 Hook の実行フロー

```
編集 → PreToolUse Hooks
         ├─ 検証 OK？
         │   ├─ Yes → 編集実行
         │   └─ No  → 中止・エラーメッセージ
         └─ Error → ロールバック

編集実行 → PostToolUse Hooks
           ├─ clang-format
           ├─ cppcheck
           └─ Custom check

セッション終了 → Stop Hook
                 └─ 最終検証
```

---

## 📊 手動 vs 自動化

### Before: 手動プロセス

```
1. ファイル編集
2. 「あ、フォーマット忘れた」と気づく
3. clang-format を手動実行
4. 「cppcheck でチェックしておこう」
5. cppcheck 手動実行
6. ビルド試す
7. エラー発見…
```

⏱️ **時間**: 数分 / 編集

❌ **問題**: 忘れるリスク・非一貫性

### After: Hook 自動化

```
1. ファイル編集
2. PostToolUse Hooks 自動実行:
   - clang-format ✓
   - cppcheck ✓
3. セッション終了
4. Stop Hook 実行: 最終ビルド ✓
```

⏡ **時間**: 自動（編集中に完了）

✅ **利点**: 確実・一貫性・クリーン

---

## 🛠️ よくある Hook コマンド

### C++ フォーマット

```json
{
  "matcher": "Write|Edit",
  "command": "which clang-format >/dev/null && clang-format -i \"$FILE_PATH\" || true",
  "description": "Auto-format with clang-format"
}
```

### CMake フォーマット

```json
{
  "matcher": "Write|Edit",
  "command": "which cmake-format >/dev/null && cmake-format -i \"$FILE_PATH\" || true",
  "description": "Auto-format CMakeLists.txt"
}
```

### 静的解析（cppcheck）

```json
{
  "matcher": "Write|Edit",
  "command": "which cppcheck >/dev/null && cppcheck --quiet --suppress=missingIncludeSystem \"$FILE_PATH\" || true",
  "description": "Static analysis with cppcheck"
}
```

### 最終ビルド検証

```json
{
  "matcher": "Stop",
  "command": "cd /path/to/project && mkdir -p build && cd build && cmake .. && cmake --build . 2>&1 | tail -5",
  "description": "Verify build succeeds"
}
```

---

## ✅ Hooks のベストプラクティス

### ✅ 良い Hook

- **高速** — 数秒以内に実行
- **決定的** — 同じ入力で常に同じ出力
- **失敗時のメッセージ明確** — ユーザーが何を修正すべきか分かる
- **オプショナルツール安全** — `which` チェック + `|| true` で失敗しない

### ❌ 悪い Hook

- **遅い** — 編集ごとに数分ブロック
- **非決定的** — ネットワークテストなど
- **エラーメッセージなし** — ユーザーが原因不明
- **ハード要件** — ツール未インストール時に失敗

---

## ✅ 次のステップ

→ `docs/05-project-claude.md` で **Project CLAUDE.md** を学ぶ
