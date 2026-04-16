---
name: cmake-guidelines
description: "CMakeLists.txt best practices for Qt 6 projects, including project setup, find_package, target configuration, and link libraries."
applyTo: "**/CMakeLists.txt"
---

# CMakeLists.txt ガイドライン

このファイルは `CMakeLists.txt` 用のガイドラインです。

## 🚀 Qt 6 プロジェクト構造（最小限）

```cmake
cmake_minimum_required(VERSION 3.22)
project(DataVisualizer)

# C++17 標準
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 6 必須コンポーネント
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
)

# メイン実行ファイル
add_executable(data-vizualizer
    src/main.cpp
    src/MainWindow.cpp
    src/DataProcessor.cpp
)

# インクルードディレクトリ
target_include_directories(data-visualizer PRIVATE include)

# リンクライブラリ
target_link_libraries(data-visualizer PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
```

---

## 📋 重要な設定

### C++ 標準

```cmake
# ✅ 推奨: C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ❌ 結論 C++11/14（古い）
# ❌ 非推奨: ...CXX_STANDARD なし（コンパイラ依存）
```

### Qt コンポーネント検出

```cmake
# ✅ explicit に必要なコンポーネント列挙
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
)

# ❌ 曖昧: バージョン未指定
find_package(Qt6 REQUIRED)

# ❌ 古い: Qt5（Qt 6 推奨）
find_package(Qt5 REQUIRED)
```

### ターゲット設定

```cmake
# ✅ target_include_directories（推奨）
target_include_directories(my-app PRIVATE include)

# ✅ target_link_libraries（推奨）
target_link_libraries(my-app PRIVATE Qt6::Core Qt6::Widgets)

# ❌ include_directories（古い）
include_directories(include)

# ❌ link_libraries（古い）
link_libraries(Qt6::Core)
```

---

## 📁 ファイル構成パターン

### コンパイルソースリスト

```cmake
# ✅ 推奨: 明示的なソースリスト
add_executable(app
    src/main.cpp
    src/MainWindow.cpp
    src/DataPanel.cpp
    src/DataProcessor.cpp
)

# ⚠️ 注意: glob（初期化時のみ更新）
file(GLOB SOURCES "src/*.cpp")
add_executable(app ${SOURCES})
```

### マルチ·ディレクトリ構成

```cmake
# メイン CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(DataVisualizer)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# サブディレクトリのビルド
add_subdirectory(src)

# または直接インクルード
include_directories(include)
```

### src/CMakeLists.txt

```cmake
# ソース収集（サブディレクトリぐる）
set(SOURCES
    main.cpp
    MainWindow.cpp
    DataProcessor.cpp
)

# 実行ファイル定義（親スコープで）
add_executable(data-visualizer ${SOURCES})

target_link_libraries(data-visualizer PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
```

---

## 🔗 ライブラリリンク

```cmake
# Qt ライブラリ（必須）
target_link_libraries(my-app PRIVATE
    Qt6::Core           # コア機能
    Qt6::Gui            # GUI フレームワーク
    Qt6::Widgets        # ウィジェット
    Qt6::Network        # ネットワーク（必要な場合）
)

# システムライブラリ
target_link_libraries(my-app PRIVATE
    pthread             # スレッド（Unix/Linux）
)
```

---

## ✅ ビルドタイプ

```cmake
# ビルドタイプ明示
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# または外部指定
# cmake .. -DCMAKE_BUILD_TYPE=Debug
```

---

## 🧪 テスト設定（オプション）

```cmake
# テスト有効化
enable_testing()

# テスト実行ファイル
add_executable(test-data-processor
    tests/test_data_processor.cpp
)

target_link_libraries(test-data-processor PRIVATE
    Qt6::Core
    Qt6::Test
)

# テスト登録
add_test(NAME DataProcessor COMMAND test-data-processor)
```

---

## ❌ アンチパターン

```cmake
# ❌ C++ 標準未設定（プラットフォーム依存）
# set(CMAKE_CXX_STANDARD 17) コメント出し

# ❌ 曖昧な find_package
find_package(Qt6)  # コンポーネント未指定

# ❌ 古い インターフェース
include_directories(include)
link_libraries(Qt6::Widgets)

# ❌ 絶対パス（ポ ータビリティ損失）
link_directories(/usr/lib/qt6)
```

---

## ✨ ベストプラクティス

```cmake
# ✅ 明示的・読み易い構成

cmake_minimum_required(VERSION 3.22)
project(DataVisualizer VERSION 1.0.0)

# 標準設定
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Qt 検出
find_package(Qt6 REQUIRED COMPONENTS
    Core Gui Widgets
)

# ターゲット
add_executable(data-visualizer
    src/main.cpp
    src/MainWindow.cpp
    src/DataProcessor.cpp
)

# インクルード・リンク（target-specific）
target_include_directories(data-visualizer PRIVATE include)
target_link_libraries(data-visualizer PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

# ビルド完了メッセージ
message(STATUS "Project configured successfully")
```
