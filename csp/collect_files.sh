#!/bin/bash
set -e

rm -rf build dist
mkdir -p dist
cp csplib/.bin/*.dll dist
cp csplib/.bin/*.pyd dist
cp cspsim/.bin/*.dll dist
cp cspsim/.bin/*.pyd dist
cp cspwf/.bin/*.dll dist
# cp modules/demeter/.bin/demeter.dll dist
# cp modules/chunklod/.bin/chunklod.dll dist

copy_dependencies () {
    src_file=$1
    dst_dir=$2
    mapfile -t files < <(ntldd $src_file | grep -vi WINDOWS | grep -oP "(?<=\=\> ).*(?= \()")
    for f in "${files[@]}"; do
        if [[ "$f" =~ "$(pwd)/$dst_dir" ]]; then
            # Dependency is already present in the target directory.
            continue
        fi
        cp -v "$f" "$dst_dir"
    done
}

copy_dependencies csplib/.bin/csplib.dll dist
copy_dependencies cspsim/.bin/cspsim.dll dist
# copy_dependencies modules/demeter/.bin/demeter.dll dist
# copy_dependencies modules/chunklod/.bin/chunklod.dll dist

# export PATH=$(pwd)/dist:$PATH
export PYTHONPATH=$(pwd)/dist:$PYTHONPATH
# export PATH=$(pwd)/csplib/.bin:$(pwd)/cspsim/.bin:$(pwd)/cspwf/.bin:/ucrt64/lib/:$PATH
# export PYTHONPATH=$(pwd)/csplib/.bin:$(pwd)/cspsim/.bin:$(pwd)/cspwf/.bin:/ucrt64/bin:$PYTHONPATH
pyinstaller --clean sim.spec

rm dist/*.dll

# mv dist/sim/* dist
# rm -rf dist/sim
# cp bin/sim.ini dist
# mkdir dist/_internal/csp/
# cp -r data dist/_internal/csp/

# cat << EOF > dist/sim.sh
# #!/bin/bash
# export PATH=$(pwd):$PATH
# export PYTHONPATH=$(pwd):$PYTHONPATH
# ./sim.exe
# EOF
