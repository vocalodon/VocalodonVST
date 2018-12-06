
#delete deployed VST

rm -rf ~/Library/Audio/Plug-Ins/VST/VocalodonVST.vst
rm -rf ~/Library/VocalodonVST
rm -rf ~/Library/"Application Support"/VocalodonVST
rm -f  ~/Library/"Application Support"/VocalodonVST.settings


#delete build files

cd `dirname $0`
cd ../

rm -rf ./src/DonVST/Builds/MacOSX/build/Release/VocalodonVST.app
rm -f  ./src/DonVST/Builds/MacOSX/build/Release/libVocalodonVST.a
rm -f  ./src/DonVST/Builds/MacOSX/build/Release/VocalodonVST.vst

rm -rf ./src/DonVST/Builds/MacOSX/build/Debug/VocalodonVST.app
rm -f  ./src/DonVST/Builds/MacOSX/build/Debug/libVocalodonVST.a
rm -f  ./src/DonVST/Builds/MacOSX/build/Debug/VocalodonVST.vst
