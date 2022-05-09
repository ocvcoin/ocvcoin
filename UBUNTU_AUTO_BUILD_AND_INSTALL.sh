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
    echo "Please run as root"
    exit
fi




																
apt-get update																

apt-get -y install build-essential libtool autotools-dev automake pkg-config bsdmainutils python3 cmake git byacc bison libdb-dev libdb++-dev

if [ "$(lsb_release -c -s)" = "xenial" ] || [ "$(lsb_release -c -s)" = "yakkety" ]; then
  add-apt-repository -y ppa:ubuntu-toolchain-r/test
  apt-get update
  apt-get -y install gcc-7 g++-7
  update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 60
  update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 60
  apt-get -y upgrade ca-certificates  
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

bash -c "cd /tmp/ocvcoin/ocvcoin && CONFIG_SITE=/tmp/ocvcoin/ocvcoin/depends/${SHARED_FOLDER}/share/config.site ./configure --enable-upnp-default --enable-natpmp-default --disable-tests --disable-gui-tests --disable-bench --with-sqlite=yes --with-miniupnpc --with-natpmp --with-qrencode --with-incompatible-bdb LIBS=\"`env PKG_CONFIG_LIBDIR=/tmp/ocvcoin/ocvcoin/depends/${SHARED_FOLDER}/lib/pkgconfig pkg-config --static --libs opencv`\" CXXFLAGS=\"`env PKG_CONFIG_LIBDIR=/tmp/ocvcoin/ocvcoin/depends/${SHARED_FOLDER}/lib/pkgconfig pkg-config --static --cflags opencv`\""


make

make install






useradd ocvcoin
groupadd ocvcoin


mkdir /etc/ocvcoin


cp /tmp/ocvcoin/ocvcoin/share/examples/ocvcoin.conf /etc/ocvcoin/ocvcoin.conf



cp /tmp/ocvcoin/ocvcoin/contrib/init/ocvcoind.service /etc/systemd/system/ocvcoind.service

sed -i 's:/usr/bin/ocvcoind:/usr/local/bin/ocvcoind:g' /etc/systemd/system/ocvcoind.service




if [ "$(lsb_release -c -s)" = "xenial" ] || [ "$(lsb_release -c -s)" = "yakkety" ]; then
  sed -i 's:ExecStartPre=/bin/chgrp ocvcoin /etc/ocvcoin::g' /etc/systemd/system/ocvcoind.service
  chgrp ocvcoin /etc/ocvcoin
  mkdir /var/lib/ocvcoind
  chown ocvcoin:ocvcoin /var/lib/ocvcoind  
fi




systemctl enable ocvcoind.service
systemctl start ocvcoind.service






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







