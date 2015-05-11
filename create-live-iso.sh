#!/bin/bash

set -e

# Debian fix
export PATH=/usr/sbin/:$PATH

# ------------------------------------------------------------------------------------
# function to run a command inside the chroot

function run_chroot_cmd() {

echo "
export HOME=/root
export LANG=C
unset LC_TIME
$@
" | sudo tee ~/livecd/custom/_cmd >/dev/null

sudo chroot ~/livecd/custom /bin/bash /_cmd

}

# -------------------------------------------------------------------------------------------
# Check dependencies

if (which debootstrap > /dev/null); then true; else
  echo "debootstrap not found, please install it"
  exit
fi

if (which mksquashfs > /dev/null); then true; else
  echo "mksquashfs not found, please install it"
  exit
fi

# -------------------------------------------------------------------------------------------
# start bootstrap

if [ ! -d ~/livecd/custom/var/mod-live ]; then
  sudo debootstrap --arch=amd64 vivid ~/livecd/custom http://archive.ubuntu.com/ubuntu/
  sudo mkdir ~/livecd/custom/var/mod-live
fi

# -------------------------------------------------------------------------------------------
# enable network

sudo rm -f ~/livecd/custom/etc/hosts
sudo rm -f ~/livecd/custom/etc/resolv.conf
sudo cp /etc/resolv.conf /etc/hosts ~/livecd/custom/etc/

# ------------------------------------------------------------------------------------
# mount basic chroot points

run_chroot_cmd mount -t devpts none /dev/pts || true
run_chroot_cmd mount -t proc   none /proc    || true
run_chroot_cmd mount -t sysfs  none /sys     || true

# ------------------------------------------------------------------------------------
# fix upstart

if [ ! -f ~/livecd/custom/var/mod-live/initial-setup-1 ]; then
  run_chroot_cmd dpkg-divert --local --rename --add /sbin/initctl
  run_chroot_cmd ln -s /bin/true /sbin/initctl
  run_chroot_cmd touch /etc/init.d/systemd-logind
  run_chroot_cmd touch /etc/init.d/modemmanager
  sudo touch ~/livecd/custom/var/mod-live/initial-setup-1
fi

# ------------------------------------------------------------------------------------
# proper vivid repos + backports

if [ ! -f ~/livecd/custom/var/mod-live/initial-setup-2 ]; then
  echo "
deb http://archive.ubuntu.com/ubuntu/ vivid main restricted universe multiverse
deb http://archive.ubuntu.com/ubuntu/ vivid-updates main restricted universe multiverse
deb http://archive.ubuntu.com/ubuntu/ vivid-backports main restricted universe multiverse
deb http://security.ubuntu.com/ubuntu vivid-security main restricted universe multiverse
" | sudo tee ~/livecd/custom/etc/apt/sources.list >/dev/null
  sudo touch ~/livecd/custom/var/mod-live/initial-setup-2
fi

# ------------------------------------------------------------------------------------
# install kxstudio-repos package

if [ ! -f ~/livecd/custom/var/mod-live/initial-setup-3 ]; then
  run_chroot_cmd apt-get update
  run_chroot_cmd apt-get install -y software-properties-common wget
  run_chroot_cmd wget https://launchpad.net/~kxstudio-debian/+archive/kxstudio/+files/kxstudio-repos_8.1.0~kxstudio1_all.deb
  run_chroot_cmd dpkg -i kxstudio-repos_8.1.0~kxstudio1_all.deb
  sudo touch ~/livecd/custom/var/mod-live/initial-setup-3
fi

# ------------------------------------------------------------------------------------
# do a full system update

run_chroot_cmd apt-get update
run_chroot_cmd apt-get dist-upgrade -y

# -------------------------------------------------------------------------------------------
# Base Install

if [ ! -f ~/livecd/custom/var/mod-live/initial-setup-4 ]; then
  run_chroot_cmd apt-get install -y ubuntu-standard laptop-detect dkms
  sudo touch ~/livecd/custom/var/mod-live/initial-setup-4
fi

if [ ! -f ~/livecd/custom/var/mod-live/initial-setup-5 ]; then
  # skip grub install/configure to HDD here
  run_chroot_cmd apt-get install -y linux-lowlatency linux-image-lowlatency linux-headers-lowlatency
  sudo touch ~/livecd/custom/var/mod-live/initial-setup-5
fi

# -------------------------------------------------------------------------------------------

# -------------------------------------------------------------------------------------------
