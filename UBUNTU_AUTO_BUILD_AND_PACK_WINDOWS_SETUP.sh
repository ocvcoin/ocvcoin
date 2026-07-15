#!/bin/bash
cat << 'EOF'
 ██████╗  ██████╗██╗   ██╗ ██████╗ ██████╗ ██╗███╗   ██╗
██╔═══██╗██╔════╝██║   ██║██╔════╝██╔═══██╗██║████╗  ██║
██║   ██║██║     ██║   ██║██║     ██║   ██║██║██╔██╗ ██║
██║   ██║██║     ╚██╗ ██╔╝██║     ██║   ██║██║██║╚██╗██║
╚██████╔╝╚██████╗ ╚████╔╝ ╚██████╗╚██████╔╝██║██║ ╚████║
 ╚═════╝  ╚═════╝  ╚═══╝   ╚═════╝ ╚═════╝ ╚═╝╚═╝  ╚═══╝
                                                        
EOF

if (( $EUID != 0 )); then
    echo -e "\nplease run as root\n"
    exit 1
fi

export DEBIAN_FRONTEND=noninteractive





FREE_RAM_MB=$(awk '/^MemAvailable:/ {print int($2 / 1024)}' /proc/meminfo)

if [ -z "$FREE_RAM_MB" ] || [ "$FREE_RAM_MB" -eq 0 ]; then
    FREE_RAM_MB=$(awk '/^(MemFree|Cached):/ {sum += $2} END {print int(sum / 1024)}' /proc/meminfo)
    RAM_DETECTION_METHOD="MemFree + Cached"
else
    RAM_DETECTION_METHOD="MemAvailable"
fi

if [ -z "$FREE_RAM_MB" ] || [ "$FREE_RAM_MB" -eq 0 ]; then
    FREE_RAM_MB=$(free -m | awk '
        NR==1 { for(i=1;i<=NF;i++) if($i~/avail/) col=i+1 } 
        NR==2 { print $col }
    ')
    RAM_DETECTION_METHOD="free -m (Available)"
fi

if [ -z "$FREE_RAM_MB" ] || [ "$FREE_RAM_MB" -eq 0 ]; then
    FREE_RAM_MB=1024
    RAM_DETECTION_METHOD="Default (Fallback)"
fi

TOTAL_CPU_CORE_COUNT=$(nproc 2>/dev/null || grep -c ^processor /proc/cpuinfo 2>/dev/null || echo 1)
[ "$TOTAL_CPU_CORE_COUNT" -lt 1 ] && TOTAL_CPU_CORE_COUNT=1

RAM_PER_THREAD_GB="1.5"
RAM_PER_THREAD_MB=1536

RECOMMENDED_THREAD_COUNT=$(awk -v ram="$FREE_RAM_MB" -v req="$RAM_PER_THREAD_MB" 'BEGIN {
    val = int(ram / req);
    print (val < 1) ? 1 : val
}')

if [ "$RECOMMENDED_THREAD_COUNT" -le "$TOTAL_CPU_CORE_COUNT" ]; then
    THREAD_COUNT=$RECOMMENDED_THREAD_COUNT
    LIMIT_REASON="RAM Limit"
else
    THREAD_COUNT=$TOTAL_CPU_CORE_COUNT
    LIMIT_REASON="CPU Limit"
fi

if [ "$RECOMMENDED_THREAD_COUNT" -eq "$TOTAL_CPU_CORE_COUNT" ]; then
    LIMIT_REASON="Balanced"
fi


if [ "$THREAD_COUNT" -lt 1 ]; then
    THREAD_COUNT=1
fi

if [ "$THREAD_COUNT" -gt 10 ]; then
    THREAD_COUNT=10
fi


echo "========================================="
printf "%-25s : %s\n" "Free RAM" "$FREE_RAM_MB MB"
printf "%-25s : %s\n" "RAM Detection Method" "$RAM_DETECTION_METHOD"
printf "%-25s : %s\n" "CPU Core Count" "$TOTAL_CPU_CORE_COUNT"
printf "%-25s : %s\n" "RAM Per Thread" "$RAM_PER_THREAD_GB GB ($RAM_PER_THREAD_MB MB)"
echo "-----------------------------------------"
echo "Formula: int($FREE_RAM_MB / $RAM_PER_THREAD_MB)"
echo "Recommended Threads: $RECOMMENDED_THREAD_COUNT"
echo "Comparison: min($RECOMMENDED_THREAD_COUNT, $TOTAL_CPU_CORE_COUNT)"
echo "Limiting Factor: $LIMIT_REASON"
echo "========================================="
printf "SELECTED THREAD COUNT: %s\n" "$THREAD_COUNT"
echo "========================================="



set -e
																
apt-get update	

set +e															

apt-get -y install software-properties-common

add-apt-repository -y universe

add-apt-repository -y contrib main non-free

apt-get update

apt-get -y install build-essential
apt-get -y install libtool
apt-get -y install autotools-dev
apt-get -y install automake
apt-get -y install pkg-config
apt-get -y install bsdmainutils
apt-get -y install python3
apt-get -y install cmake
apt-get -y install git
apt-get -y install byacc
apt-get -y install bison
apt-get -y install libdb-dev
apt-get -y install libdb++-dev
apt-get -y install curl
apt-get -y install wget 
apt-get -y install nsis
apt-get -y upgrade ca-certificates


if apt-get -y install g++-mingw-w64-x86-64-posix; then
    echo "Installed g++-mingw-w64-x86-64-posix"
else
    echo "Installing g++-mingw-w64-x86-64"
    apt-get -y install g++-mingw-w64-x86-64
fi




vercomp () {
    if [[ $1 == $2 ]]
    then
        return 0
    fi
    local IFS=.
    local i ver1=($1) ver2=($2)
    # fill empty fields in ver1 with zeros
    for ((i=${#ver1[@]}; i<${#ver2[@]}; i++))
    do
        ver1[i]=0
    done
    for ((i=0; i<${#ver1[@]}; i++))
    do
        if [[ -z ${ver2[i]} ]]
        then
            # fill empty fields in ver2 with zeros
            ver2[i]=0
        fi
        if ((10#${ver1[i]} > 10#${ver2[i]}))
        then
            return 1
        fi
        if ((10#${ver1[i]} < 10#${ver2[i]}))
        then
            return 2
        fi
    done
    return 0
}

testvercomp () {
    vercomp "$1" "$2"
    case $? in
        0) op='=';;
        1) op='>';;
        2) op='<';;
    esac
    if [[ $op != $3 ]]
    then
        return 1
    else
        return 0
    fi
}








if testvercomp "$(x86_64-w64-mingw32-gcc -dumpfullversion -dumpversion | sed 's/-.*//')" "10.0" "<"; then
    
	
	
	echo -e "\nyour x86_64-w64-mingw32-gcc version: $(x86_64-w64-mingw32-gcc -dumpfullversion -dumpversion)\n"
	echo -e "required minimum x86_64-w64-mingw32-gcc version: 10.0\n"
	echo -e "build failed!\n"
	
	exit 1
	
	
fi







autoconf_version=$(autoconf --version | awk 'NR==1{print $NF}')



if testvercomp "$autoconf_version" "2.69" "<"; then
    echo -e "\nyour autoconf version: $autoconf_version\n"
	echo -e "required minimum autoconf version: 2.69\n"
	echo -e "build failed!\n"
	
	exit 1
 
	
	
fi




automake_version=$(automake --version | awk 'NR==1{print $NF}')


if testvercomp "$automake_version" "1.13" "<"; then
    echo -e "\nyour automake version: $automake_version\n"
	echo -e "required minimum automake version: 1.13\n"
	echo -e "build failed!\n"
	
	exit 1
 
	
	
fi



SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"


if [ -n "$1" ] && [ "$1" = "usecurrentpath" ]; then
    myworkdir="${SCRIPT_DIR}/"
    
else
    myworkdir="/tmp/ocvcoin/ocvcoin/"
	rm -rf /tmp/ocvcoin

	mkdir /tmp/ocvcoin

	set -e
	
	cd /tmp/ocvcoin

	git clone --depth=1 https://github.com/ocvcoin/ocvcoin.git ocvcoin	
	
	set +e
	
    
fi






ORIGINAL_BINFMT_STATUS=$(cat /proc/sys/fs/binfmt_misc/status 2>/dev/null)

trap "echo $ORIGINAL_BINFMT_STATUS > /proc/sys/fs/binfmt_misc/status" ERR
trap "echo $ORIGINAL_BINFMT_STATUS > /proc/sys/fs/binfmt_misc/status" EXIT

PATH=$(echo "$PATH" | sed -e 's/:\/mnt.*//g') # strip out problematic Windows %PATH% imported var
bash -c "echo 0 > /proc/sys/fs/binfmt_misc/status" # Disable WSL support for Win32 applications.

set -e






cd "${myworkdir}"




#sed -i "s/define(_CLIENT_VERSION_BUILD, 0)/define(_CLIENT_VERSION_BUILD, 1)/" configure.ac

chmod -R 755 "${myworkdir}"
		
		
 
cd "${myworkdir}/depends"
make -j$THREAD_COUNT HOST=x86_64-w64-mingw32

SHARED_FOLDER="$(ls -td -- */ | head -n 1 | cut -d'/' -f1)"

cd "${myworkdir}"



sh autogen.sh

bash -c "cd ${myworkdir} && CONFIG_SITE=${myworkdir}/depends/${SHARED_FOLDER}/share/config.site ./configure --prefix=/ --disable-bench --with-sqlite=yes --with-miniupnpc --with-natpmp --with-qrencode --with-incompatible-bdb --disable-gui-tests --disable-bench --disable-fuzz --disable-fuzz-binary LIBS=\"`env PKG_CONFIG_LIBDIR=${myworkdir}/depends/${SHARED_FOLDER}/lib/pkgconfig pkg-config --static --libs opencv`\" CXXFLAGS=\"`env PKG_CONFIG_LIBDIR=${myworkdir}/depends/${SHARED_FOLDER}/lib/pkgconfig pkg-config --static --cflags opencv`\""


make -j$THREAD_COUNT

make deploy


cd "${myworkdir}"


bash -c "echo $ORIGINAL_BINFMT_STATUS > /proc/sys/fs/binfmt_misc/status" # restore value


