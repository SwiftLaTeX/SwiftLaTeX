mkdir win32 -p
rm win32/ -r
mkdir win32/upload/manifest -p
mkdir win32/upload/asset -p
cp public/* win32/ -R
cp dist win32/ -R