# Crosscompiling QT and QTWebEngine (for rpi)

To cross-compile QT we need multiple things:
* Crosscimpiler
* sysroot of target
* 


## Setting up sysroot
```bash
:~$ fdisk -l Downloads/2020-02-13-raspbian-buster-lite.img
Disk Downloads/2020-02-13-raspbian-buster-lite.img: 1.74 GiB, 1849688064 bytes, 3612672 sectors
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: dos
Disk identifier: 0x738a4d67

Device                                         Boot  Start     End Sectors  Size Id Type
Downloads/2020-02-13-raspbian-buster-lite.img1        8192  532479  524288  256M  c W95 FAT32 (LBA)
Downloads/2020-02-13-raspbian-buster-lite.img2      532480 3612671 3080192  1.5G 83 Linux

```
Here we are interested in the root file system (.img2)  
To mount this fs use following command
```bash
sudo mount 2020-02-13-raspbian-buster-lite.img -o loop,offset=$(( 512 * 532480)) /media/rpi-sysroot
```

##Configure command
```bash
./configure -no-opengl -release -device linux-rasp-pi-g++ \
-device-option CROSS_COMPILE=/usr/bin/arm-linux-gnueabihf- \
-sysroot /media/rpi-sysroot \
-opensource -confirm-license \
-make libs -prefix /usr/local/qt5pi -extprefix ~/raspi/qt5pi -hostprefix ~/raspi/qt5 \
-no-use-gold-linker -no-gbm  \
-nomake examples -nomake tests \
-skip qtwayland -skip qtlocation -skip qtscript \
-skip qtserialbus -skip qtpim -skip qtdocgallery \
-skip qtcharts -skip qtgamepad -skip qtspeech -skip qttranslations -v
```