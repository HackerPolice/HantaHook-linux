#!/usr/bin/env bash
function echo_orange() {
  echo -e "\\e[33m$*\\e[0m"
}

function echo_red() {
  echo -e "\\e[31m$*\\e[0m"
}

function echo_green() {
  echo -e "\\e[32m$*\\e[0m"
}

function launch_csgo() {
  
  local csgo_pid=$(pidof csgo_linux64)
  echo_orange "Launching CSGO"
  steam steam://rungameid/730 >/dev/null 2>&1 &
  disown
  echo -ne "Launching..."
  while [ -z "$csgo_pid" ]; 
  do
    csgo_pid=$(pidof csgo_linux64)
    echo -ne "."
  done

  echo " "
  echo_green "Wait for 25 sec"
  sleep 25 # wait for 25 secs
}

function build(){
  cmake . || exit 1

  if ! hash patchelf 2>/dev/null; then
    echo_red -e "You need to install the \"patchelf\" command!\nUse your package manager to install it, or get source here:\nhttps://github.com/NixOS/patchelf"
    exit 1
  fi

  rm .build_id_hex
  # Get a Random name from the build_names file.
  BUILD_ID=$(shuf -n 1 build_names)

  echo_orange "Checking for file names"
  # In case this file exists, get another one. ( checked it works )
  while [ -f "/usr/lib/${BUILD_ID}" ] || grep -q ${BUILD_ID} /proc/$(pidof csgo_linux64)/maps; do
    BUILD_ID=$(shuf -n 1 build_names)
  done

  echo_orange "Checking File name done"

  echo_orange "Hexify it and put it into build_id_hex"
  for ((i = 0; i < ${#BUILD_ID}; i++)); do
    printf '0x%x,\n' "'${BUILD_ID:$i:1}" >>.build_id_hex
  done
  printf '0x0' >>.build_id_hex

  make -j"$(grep -c "^processor" /proc/cpuinfo)" "$@" || exit 1

  if [ -f .build_id ]; then
    filename=$(cat .build_id)
    rm "$filename"
    if [ -f "/usr/lib/${filename}" ]; then
      echo -e "Note: Old file found at /usr/lib/${filename}\n"
      sudo rm "/usr/lib/${filename}"

    fi
    chmod 660 .build_id
    mv .build_id .build_id_old
  fi

  echo $BUILD_ID >.build_id
  mv libHantaHook.so "$BUILD_ID"

  strip -s -v "$BUILD_ID"
  patchelf --set-soname "$BUILD_ID" "$BUILD_ID" || echo_orange "Warning: your patchelf version does not support \"--set-soname\"\nConsider building from source: https://github.com/NixOS/patchelf"

  chmod 400 .build_id # Make the build_id read-only for safety.

  echo "Build Completed."
}

function load(){
  # Check_For_logDirectory
  csgo_pid=$(pidof csgo_linux64)

  if [ -z "$csgo_pid" ]; 
    then
      launch_csgo
      echo_green "Injecting Started"
    else
      echo_green "Injecting Started"
  fi

  # pBypass for crash dumps being sent
  # You may also want to consider using -nobreakpad in your launch options.
  sudo rm -rf /tmp/dumps           # Remove if it exists
  sudo mkdir --mode=000 /tmp/dumps # Make it as root with no permissions

  if [ ! -f .build_id ]; then
    echo "Build ID not found. Please rebuild using the './HantaHook -b' script."
    exit 1
  fi

  filename=$(cat .build_id)

  # Credit: Aixxe @ aixxe.net
  if grep -q "$filename" /proc/"$csgo_pid"/maps; then
    echo_orange "Already injected, aborting..."
    exit 1
  fi
  echo "Injecting Build ID: $filename"

  # https://www.kernel.org/doc/Documentation/security/Yama.txt
  echo "2" | sudo tee /proc/sys/kernel/yama/ptrace_scope # Only allows root to inject code. This is temporary until reboot.

  sudo cp "$filename" "/usr/lib/${filename}"

  sudo killall -19 steam
  sudo killall -19 steamwebhelper

  lib_dir_name="lib"
  if [ $(getconf LONG_BIT) = 64 ]; then
    lib_dir_name+="64"
  fi

  input="$(
    sudo gdb -n -q -batch-silent \
      -ex "set logging on" \
      -ex "set logging file /dev/null" \
      -ex "set logging redirect on" \
      -ex "set auto-load safe-path /usr/share/gdb/auto-load/usr/$lib_dir_name/:/usr/$lib_dir_name/" \
      -ex "attach $csgo_pid" \
      -ex "set \$dlopen = (void*(*)(char*, int)) dlopen" \
      -ex "call \$dlopen(\"/usr/lib/$filename\", 1)" \
      -ex "detach" \
      -ex "quit"
  )"

  sleep 1
  sudo killall -18 steamwebhelper
  sudo killall -18 steam

  sudo rm "/usr/lib/${filename}"

  last_line="${input##*$'\n'}"

  if [ "$last_line" != "\$1 = (void *) 0x0" ]; then
    echo_green "Successfully injected!"
  else
    echo_red "Injection failed, make sure you have compiled..."
  fi

}

function unload(){
  # Credit: Aixxe @ aixxe.net
  csgo_pid=$(pidof csgo_linux64)
  filename=$(cat .build_id)
  filename_old=$(cat .build_id_old)

  if [ -f .build_id ]; then
    if grep -q "$filename" /proc/"$csgo_pid"/maps; then
      echo "unloading $filename"
      sudo gdb -n -q -batch-silent \
        -ex "set logging on" \
        -ex "set logging file /dev/null" \
        -ex "set logging redirect on" \
        -ex "attach $csgo_pid" \
        -ex "set \$dlopen = (void*(*)(char*, int)) dlopen" \
        -ex "set \$dlclose = (int(*)(void*)) dlclose" \
        -ex "set \$library = \$dlopen(\"/usr/lib/$filename\", 6)" \
        -ex "call \$dlclose(\$library)" \
        -ex "call \$dlclose(\$library)" \
        -ex "detach" \
        -ex "quit"
      sudo rm "/usr/lib/${filename}"
    fi
  fi

  # "build_id_old" is used for unloading in case you rebuild while injected.
  if [ -f .build_id_old ]; then
    if grep -q "$filename_old" /proc/"$csgo_pid"/maps; then
      echo "unloading old file - $filename_old"
      sudo gdb -n -q -batch-silent \
        -ex "set logging on" \
        -ex "set logging file /dev/null" \
        -ex "set logging redirect on" \
        -ex "attach $csgo_pid" \
        -ex "set \$dlopen = (void*(*)(char*, int)) dlopen" \
        -ex "set \$dlclose = (int(*)(void*)) dlclose" \
        -ex "set \$library = \$dlopen(\"/usr/lib/$filename_old\", 6)" \
        -ex "call \$dlclose(\$library)" \
        -ex "call \$dlclose(\$library)" \
        -ex "detach" \
        -ex "quit"
      sudo rm "/usr/lib/${filename_old}"
    fi
  fi

  echo "Done. See CS:GO Console."
}

function clean(){
  rm -rf ./CMakeFiles
  rm ./CMakeCache.txt
  rm ./Makefile
  rm ./.build_id_hex

  filename = $(cat .build_id)
  rm "$filename"

  rm ./.build_id
  rm ./.build_id_old
}

function reverce_update(){
  git reset --hard
}

function kill_process(){
  sudo killall -9 csgo_linux64
  sudo killall -9 csgo.sh
  sudo killall -9 gdb
  # sudo killall -9 steam
}

function __main__(){
  local option="$1"
  sudo -v ## asking for root permission

  case $option in 
      -b|--build)
        echo_orange "Building the project"
        build
      ;;
      -l|--load)
        echo_orange "loading..."
        load
      ;;
      -ul|--unload)
        echo_orange "un loading..."
        unload
        ;;
      -c|--clean)
        echo_orange "Cleaning the mess :)"
        clean
      ;;
      -ru|--reverce-update)
      reverce_update
      ;;
      -k|--kill)
      kill_process
      ;;
      *)
        echo_red "Unknown Command $option || use -h for help"
      ;;
  esac
}

#dor loop not working
while [[ $# -gt 0 ]]
do
  __main__ $1
  shift
done

