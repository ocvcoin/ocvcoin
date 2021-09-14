#!/bin/bash
printf "
 ██████╗  ██████╗██╗   ██╗ ██████╗ ██████╗ ██╗███╗   ██╗
██╔═══██╗██╔════╝██║   ██║██╔════╝██╔═══██╗██║████╗  ██║
██║   ██║██║     ██║   ██║██║     ██║   ██║██║██╔██╗ ██║
██║   ██║██║     ╚██╗ ██╔╝██║     ██║   ██║██║██║╚██╗██║
╚██████╔╝╚██████╗ ╚████╔╝ ╚██████╗╚██████╔╝██║██║ ╚████║
 ╚═════╝  ╚═════╝  ╚═══╝   ╚═════╝ ╚═════╝ ╚═╝╚═╝  ╚═══╝
                                                        \n"
																
apt-get update																

apt-get -y install python3-pip
pip3 install opencv-python==3.2.0.6

apt-get -y install build-essential libtool autotools-dev autoconf pkg-config libssl-dev
apt-get -y install libboost-all-dev
apt-get -y install libqt5gui5 libqt5core5a libqt5dbus5 qttools5-dev qttools5-dev-tools libprotobuf-dev protobuf-compiler
apt-get -y install libqrencode-dev autoconf openssl libssl-dev libevent-dev
apt-get -y install libminiupnpc-dev
apt-get -y install git curl automake

rm -rf /tmp/ocvcoin

mkdir /tmp/ocvcoin

cd /tmp/ocvcoin

git clone https://github.com/ocvcoin/ocvcoin.git ocvcoin


rm -rf /home/pow_server
git clone https://github.com/ocvcoin/pow_server.git /home/pow_server

chmod -R 755 /tmp/ocvcoin
		
		
 
cd /tmp/ocvcoin/ocvcoin/depends
make

SHARED_FOLDER="$(ls -td -- */ | head -n 1 | cut -d'/' -f1)"

cd /tmp/ocvcoin/ocvcoin

sh autogen.sh

bash -c "cd /tmp/ocvcoin/ocvcoin && CONFIG_SITE=/tmp/ocvcoin/ocvcoin/depends/${SHARED_FOLDER}/share/config.site ./configure --disable-tests --with-incompatible-bdb"


make

make install

PASS_WORD="$(env LC_CTYPE=C tr -dc a-zA-Z0-9 < /dev/urandom| head -c 32; echo)"


mkdir /root/.ocvcoin


cp /tmp/ocvcoin/ocvcoin/share/examples/ocvcoin.conf /root/.ocvcoin/ocvcoin.conf

sudo sed -i 's/#rpcuser=alice/rpcuser=ocvcoinrpc/g' /root/.ocvcoin/ocvcoin.conf
sudo sed -i "s/#rpcpassword=DONT_USE_THIS_YOU_WILL_GET_ROBBED_8ak1gI25KFTvjovL3gAM967mies3E=/rpcpassword=${PASS_WORD}/g" /root/.ocvcoin/ocvcoin.conf




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

cronjob_editor "@reboot" "sudo /usr/bin/python3 /home/pow_server/pow_server.py >/dev/null 2>&1" "add"

cronjob_editor "@reboot" "sudo /usr/local/bin/ocvcoind -listen -rpcallowip=0.0.0.0/0 -rpcbind=0.0.0.0 -bind=0.0.0.0 -daemon" "add"





iptables -I INPUT -p tcp --dport 8333 -j ACCEPT


iptables -I INPUT -p tcp --dport 18333 -j ACCEPT


iptables -I INPUT -p tcp --dport 38333 -j ACCEPT


iptables -I INPUT -p tcp --dport 18444 -j ACCEPT


ip6tables -I INPUT -p tcp --dport 8333 -j ACCEPT


ip6tables -I INPUT -p tcp --dport 18333 -j ACCEPT


ip6tables -I INPUT -p tcp --dport 38333 -j ACCEPT


ip6tables -I INPUT -p tcp --dport 18444 -j ACCEPT



echo iptables-persistent iptables-persistent/autosave_v4 boolean true | sudo debconf-set-selections
echo iptables-persistent iptables-persistent/autosave_v6 boolean true | sudo debconf-set-selections

apt-get -y install iptables-persistent




iptables-save > /etc/iptables/rules.v4
ip6tables-save > /etc/iptables/rules.v6












printf "\\n"
printf "Please reboot system so the daemon can start."
printf "\\n"
printf "\\n"
printf "Please keep the following information!"
printf "\\n"
printf "\\n"
printf "RPC User: ocvcoinrpc"
printf "\\n"
printf "RPC Password: ${PASS_WORD}"
printf "\\n"
printf "\\n"
printf "For more settings, you can take a look at this file: /root/.ocvcoin/ocvcoin.conf"
printf "\\n"
printf "\\n"
