cd `dirname $0`
cd ../

#deploy Release Build
cp -r VocalodonVST_files ./src/DonVST/Builds/MacOSX/build/Release/VocalodonVST.app/Contents/MacOS/VocalodonVST_files/

#deploy Debug Build
cp -r VocalodonVST_files ./src/DonVST/Builds/MacOSX/build/Debug/VocalodonVST.app/Contents/MacOS/VocalodonVST_files/
