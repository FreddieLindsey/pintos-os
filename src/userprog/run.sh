[ -f "filesys.dsk" ] && rm "filesys.dsk"

make

echo -e "\n\n\n\n\n\nSTARTING NOW\n\n\n\n\n\n\n\n"

pintos-mkdisk filesys.dsk --filesys-size=2
pintos -f -q
pintos -p ../examples/echo -a echo -- -q

echo -e "\n\n\n\nPROGRAM RUNNING!\n\n\n\n"

pintos -q run 'echo x' > ./outfile 2>&1

bob="$(cat ./outfile | grep Call\ stack)"
echo "\n$bob\n"

atom ./outfile
