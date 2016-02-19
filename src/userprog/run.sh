[ -f "filesys.dsk" ] && rm "filesys.dsk"

make

[ $? != 0 ] && exit $?

pintos-mkdisk filesys.dsk --filesys-size=2 >/dev/null 2>&1
pintos -f -q >/dev/null 2>&1
pintos -p ../examples/echo -a echo -- -q >/dev/null 2>&1

pintos -q run 'echo x' > ./outfile 2>&1

bob="$(cat ./outfile | grep Call\ stack)"
echo "\n$bob\n"

atom ./outfile
