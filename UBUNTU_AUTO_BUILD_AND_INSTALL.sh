#!/bin/bash
printf "
 ██████╗  ██████╗██╗   ██╗ ██████╗ ██████╗ ██╗███╗   ██╗
██╔═══██╗██╔════╝██║   ██║██╔════╝██╔═══██╗██║████╗  ██║
██║   ██║██║     ██║   ██║██║     ██║   ██║██║██╔██╗ ██║
██║   ██║██║     ╚██╗ ██╔╝██║     ██║   ██║██║██║╚██╗██║
╚██████╔╝╚██████╗ ╚████╔╝ ╚██████╗╚██████╔╝██║██║ ╚████║
 ╚═════╝  ╚═════╝  ╚═══╝   ╚═════╝ ╚═════╝ ╚═╝╚═╝  ╚═══╝
                                                        \n"


if (( $EUID != 0 )); then
    echo -e "\nplease run as root\n"
    exit 1
fi

export DEBIAN_FRONTEND=noninteractive

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

apt-get -y upgrade ca-certificates



set -e



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
    vercomp $1 $2
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








if testvercomp "$(gcc -dumpfullversion -dumpversion)" "10.1" "<"; then
    echo -e "\nyour GCC version: $(gcc -dumpfullversion -dumpversion)\n"
	echo -e "required minimum GCC version: 10.1\n"
	echo -e "installing GCC 11 ...\n"
	
	set +e
  add-apt-repository -y ppa:ubuntu-toolchain-r/test
  apt-get update
  apt-get -y install gcc-11 g++-11
  update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 60
  update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 60	
	set -e
	
fi





if testvercomp "$(gcc -dumpfullversion -dumpversion)" "10.1" "<"; then
    echo -e "\nyour GCC version: $(gcc -dumpfullversion -dumpversion)\n"
	echo -e "required minimum GCC version: 10.1\n"
	echo -e "building GCC from source...\n"

rm -rf /tmp/gcc_build
	
mkdir /tmp/gcc_build

cd /tmp/gcc_build	

wget -O /tmp/gcc_build/gcc-13.2.0.tar.gz https://github.com/gcc-mirror/gcc/archive/refs/tags/releases/gcc-13.2.0.tar.gz


tar -xvzf gcc-13.2.0.tar.gz

cd gcc-releases-gcc-13.2.0

./contrib/download_prerequisites 

cd ..
mkdir objdir
cd objdir


/tmp/gcc_build/gcc-releases-gcc-13.2.0/configure --enable-languages=c,c++ --disable-multilib

set +e
apt-get -y install flex
set -e
make
make install	
	
fi






if testvercomp "$(gcc -dumpfullversion -dumpversion)" "10.1" "<"; then
    echo -e "\nyour GCC version: $(gcc -dumpfullversion -dumpversion)\n"
	echo -e "required minimum GCC version: 10.1\n"
	echo -e "installation failed!\n"
	
	exit 1
 
	
	
fi

autoconf_version=$(autoconf --version | awk 'NR==1{print $NF}')



if testvercomp "$autoconf_version" "2.69" "<"; then
    echo -e "\nyour autoconf version: $autoconf_version\n"
	echo -e "required minimum autoconf version: 2.69\n"
	echo -e "installation failed!\n"
	
	exit 1
 
	
	
fi




automake_version=$(automake --version | awk 'NR==1{print $NF}')


if testvercomp "$automake_version" "1.13" "<"; then
    echo -e "\nyour automake version: $automake_version\n"
	echo -e "required minimum automake version: 1.13\n"
	echo -e "installation failed!\n"
	
	exit 1
 
	
	
fi


rm -rf /tmp/ocvcoin

mkdir /tmp/ocvcoin

cd /tmp/ocvcoin

git clone https://github.com/ocvcoin/ocvcoin.git ocvcoin




chmod -R 755 /tmp/ocvcoin
		
		
 
cd /tmp/ocvcoin/ocvcoin/depends
make

SHARED_FOLDER="$(ls -td -- */ | head -n 1 | cut -d'/' -f1)"

cd /tmp/ocvcoin/ocvcoin

sh autogen.sh

bash -c "cd /tmp/ocvcoin/ocvcoin && CONFIG_SITE=/tmp/ocvcoin/ocvcoin/depends/${SHARED_FOLDER}/share/config.site ./configure   --disable-bench --with-sqlite=yes --with-miniupnpc --with-natpmp --with-qrencode --with-incompatible-bdb LIBS=\"`env PKG_CONFIG_LIBDIR=/tmp/ocvcoin/ocvcoin/depends/${SHARED_FOLDER}/lib/pkgconfig pkg-config --static --libs opencv`\" CXXFLAGS=\"`env PKG_CONFIG_LIBDIR=/tmp/ocvcoin/ocvcoin/depends/${SHARED_FOLDER}/lib/pkgconfig pkg-config --static --cflags opencv`\""


make

make install




set +e

useradd ocvcoin
groupadd ocvcoin


mkdir /etc/ocvcoin


cp /tmp/ocvcoin/ocvcoin/share/examples/ocvcoin.conf /etc/ocvcoin/ocvcoin.conf



cp /tmp/ocvcoin/ocvcoin/contrib/init/ocvcoind.service /etc/systemd/system/ocvcoind.service

sed -i 's:/usr/bin/ocvcoind:/usr/local/bin/ocvcoind:g' /etc/systemd/system/ocvcoind.service


systemd_version=$(systemd --version | awk 'NR==1{print $2}')


if testvercomp "$systemd_version" "237" "<"; then
  sed -i 's:ExecStartPre=/bin/chgrp ocvcoin /etc/ocvcoin::g' /etc/systemd/system/ocvcoind.service
  chgrp ocvcoin /etc/ocvcoin
  mkdir /var/lib/ocvcoind
  chown ocvcoin:ocvcoin /var/lib/ocvcoind  
fi




systemctl enable ocvcoind.service
systemctl restart ocvcoind.service || systemctl start ocvcoind.service






cronjob_editor () {         
# usage: cronjob_editor '<interval>' '<command>' <add|remove>

if [[ -z "$1" ]] ;then printf " no interval specified\n" ;fi
if [[ -z "$2" ]] ;then printf " no command specified\n" ;fi
if [[ -z "$3" ]] ;then printf " no action specified\n" ;fi

if [[ "$3" == add ]] ;then
    # add cronjob, no duplication:
    ( crontab -l | grep -v -F -w "$2" ; echo "$1 $2" ) | crontab -
elif [[ "$3" == remove ]] ;then
    # remove cronjob:
    ( crontab -l | grep -v "$2" ) | crontab -
fi 
} 

cronjob_editor "@reboot" "/pow_server.py" "remove"

cronjob_editor "@reboot" "/usr/local/bin/ocvcoind" "remove"


if [ -f "/usr/local/bin/ocvcoind" ]; then
    echo -e "\n\n\n\nOcvcoin Core has been successfully installed!\nIf you want to customize it, all the settings are here: /etc/ocvcoin/ocvcoin.conf\n(don't forget to run: sudo service ocvcoind restart)\nAlso, if you are using one of the Ubuntu desktop environment versions, you can launch the graphical interface with this command: ocvcoin-qt\n"
else 
    echo -e "\ninstallation failed!\n"
fi





