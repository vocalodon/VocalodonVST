Build setting guide

  1. open VocalodonVST.xcodeproj with Xcode.

  2. set the codesign for "VST", "standalone" target.

  3. Build with Xcode. ("VST", "standalone" target) 

  4. open　"dev_command" folder. It contains a command files.

  5. grant execute permission to the command files with terminal.app.
      ex)   chmod u+x del_build.command

  6. Execute the following 2 commands after executing "VST", "standalone" build.
     deploy_standalone.command
     deploy_vst.command

  7. Build files are in the following places.
     ./src/DonVST/Builds/MacOSX/build/VocalodonVST.app  (standalone)
     ~/Library/Audio/Plug-Ins/VST/VocalodonVST.vst   (VST)

  8. If you want to build again, please execute del_build.command　
     (delete build binary file)


experimenting with build in:
    MacOSX 10.14
    Xcode 10.0 (10A255)
