CD=$PWD

cd /usr/local/bin

rm my_tools.exe
rm my_tools

ln -s $CD/my_tools.exe my_tools.exe

echo "my_tools.exe lua-apps \$@">my_tools
chmod 777 my_tools

