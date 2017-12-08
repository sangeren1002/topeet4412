创建 eMMC 分区并格式化。如果原来已经做过此步骤，则可以跳过，不必每次烧写前都分区和格式化

//开发板串口输入
fdisk -c 0
fatformat mmc 0:1
ext3format mmc 0:2
ext3format mmc 0:3
ext3format mmc 0:4
fastboot

//windows 命令行输入
fastboot.exe flash bootloader u-boot-iTOP-4412.bin
fastboot.exe flash kernel zImage
fastboot.exe flash ramdisk ramdisk-uboot.img
fastboot.exe flash system system.img
fastboot -w
fastboot reboot