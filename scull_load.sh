#!/bin/sh
# $Id: scull_load,v 1.4 2004/11/03 06:19:49 rubini Exp $
module="scull_driver"
device="scull"
mode="664"
group="sudo"
major=236

# invoke insmod with all arguments we got
# and use a pathname, as insmod doesn't look in . by default
insmod ./$module.ko $* || exit 1

# retrieve major number

# Remove stale nodes and replace them, then give gid and perms
# Usually the script is shorter, it's scull that has several devices in it.

rm -f /dev/${device}[0-3]
mknod /dev/scull0 c $major 0
mknod /dev/scull1 c $major 1
mknod /dev/scull2 c $major 2
mknod /dev/scull3 c $major 3
ln -sf ${device}0 /dev/${device}
chgrp $group /dev/${device}[0-3]
chmod $mode  /dev/${device}[0-3]
