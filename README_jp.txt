ビルド手順

  1. VocalodonVST.xcodeproj をXcodeから開きます。

  2. VST, standaloneのターゲットごとのコード署名の設定をして下さい。

  3. VST, standalone　の各ターゲットをビルドします。 

  4. dev_command　フォルダを開きます。

  5. コマンドファイルをターミナルで実行できるように各コマンドファイルに実行権限を与えて下さい。
      ex)   chmod u+x del_build.command

  6. ビルド後に以下の2つのコマンドファイルを実行して下さい。
     ビルドしたファイルに必要なファイルを埋め込む処理です。
     deploy_standalone.command
     deploy_vst.command

  7. ビルドしたファイルは以下のパスに作成されます。
     ./src/DonVST/Builds/MacOSX/build/VocalodonVST.app  (standalone)
     ~/Library/Audio/Plug-Ins/VST/VocalodonVST.vst   (VST)

  8. 再度ビルドする場合あらかじめ、del_build.command　を実行して下さい。
     (ビルドしたファイルを削除します)
     
ビルドを試した環境:
    MacOSX 10.14
    Xcode 10.0 (10A255)
