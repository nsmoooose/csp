#!/bin/bash
rm -rf dist
mkdir -p dist
cp csplib/.bin/*.dll dist
cp csplib/.bin/*.pyd dist
cp cspsim/.bin/*.dll dist
cp cspsim/.bin/*.pyd dist
cp cspwf/.bin/*.dll dist
cp cspwf/.bin/*.pyd dist

copy_dependencies () {
    src_file=$1
    dst_dir=$2
    mapfile -t files < <(ldd $src_file | grep -vi WINDOWS | grep -oP "(?<=\=\> ).*(?= \()")
    for f in "${files[@]}"; do
        if [[ "$f" =~ "$(pwd)/$dst_dir" ]]; then
            # Dependency is already present in the target directory.
            continue
        fi
        cp -v "$f" "$dst_dir"
    done
}

copy_dependencies dist/csplib.dll dist
copy_dependencies dist/cspsim.dll dist
copy_dependencies dist/cspwf.dll dist

export PATH=$(pwd)/dist:$PATH
export PYTHONPATH=$(pwd)/dist:$PYTHONPATH
pyinstaller bin/sim.py

mv dist/sim/* dist
rm -rf dist/sim
cp bin/sim.ini dist
mkdir dist/_internal/csp/
cp -r data dist/_internal/csp/
