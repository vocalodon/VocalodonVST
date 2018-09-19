## VocalodonVST

Vocalodon VST is a Mastodon client for VST.

Copyright (c) 2017 TOMOKI++/Vocalodon.net
http://vst.vocalodon.net/

## For end users

All documents and binaries are on the official site.
http://vst.vocalodon.net/

## For developers

using

- JUCE : https://www.juce.com/
- libcurl : https://curl.haxx.se/libcurl/
- The LUFS Meter : https://github.com/klangfreund/LUFSMeter
- picojson : https://github.com/kazuho/picojson


## Builds

Require Visual Studio 2015

### Get Vocalodon VST source code

```
git clone https://github.com/vocalodon/VocalodonVST
```

### Build libcurl

Download latest curl-7.x.x.zip from https://curl.haxx.se/download.html
unzip it

#### Compile 64bit version

Open VS2015 x64 Native Tools Command prompt and change directory to your unzipped curl-7.x.x\winbuild folder.

```
nmake /f Makefile.vc mode=dll VC=14 ENABLE_SSPI=yes ENABLE_IDN=no ENABLE_WINSSL=yes DEBUG=no MACHINE=x64 GEN_PDB=no ENABLE_IPV6=yes
```

Copy curl-7.x.x\builds\libcurl-vc14-x64-release-dll-ipv6-sspi-winssl\include\curl folder into VocalodonVST\src\include

Copy bin folder and lib folder in the curl-7.x.x\builds\libcurl-vc14-x64-release-dll-ipv6-sspi-winssl into VocalodonVST\src\x64

#### Compile 32bit version (if you need)

Open VS2015 x86 Native Tools Command prompt and change directory to your unzipped curl-7.x.x\winbuild folder.

```
nmake /f Makefile.vc mode=dll VC=14 ENABLE_SSPI=yes ENABLE_IDN=no ENABLE_WINSSL=yes DEBUG=no MACHINE=x86 GEN_PDB=no ENABLE_IPV6=yes
```

Copy bin folder and lib folder in the curl-7.x.x\builds\libcurl-vc14-x86-release-dll-ipv6-sspi-winssl into VocalodonVST\src\x64

### Build Vocalodon VST

Open VocalodonVST\src\Builds\VisualStudio2015\DonVST.sln with Visual Studio 2015.

Change Solution Configuration to "Debug x64" or "Debug win32" and Rebuild "DonVST_VST" project. 

You can find VocalodonVST.dll file into VocalodonVST\src\Builds\VisualStudio2015\x64\Debug\VST or VocalodonVST\src\Builds\VisualStudio2015\win32\Debug\VST

To execute it, put libcurl.dll into same folder with VocalodonVST.dll and copy VocalodonVST\src\assets folder into same folder and rename to "VocalodonVST_files".


## License

GPLv3
