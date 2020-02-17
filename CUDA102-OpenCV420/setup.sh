#!/bin/bash

# 
# lspci | grep -i GTX
# 01:00.0 VGA compatible controller: NVIDIA Corporation GP106 [GeForce GTX 1060 3GB] (rev a1)

#sudo apt install -y git wget
# git clone
# cd ~/CUDA
# ./setup.sh


cd "$(dirname "$0")"
source .setupstate

echo "We are at sate $OPENCV_SETUPSTATE "
sleep 3

case $OPENCV_SETUPSTATE in


  1)

    echo -e "Check if you have all nonfree files and do first update of system\n\n"
    ./pre-start.sh

    retVal=$?
    if [ $retVal -eq 0 ]; then
        echo "NVIDIA files found"
    else
        echo "Download the missing files"
        exit
    fi

    cp md5/*.md5 ~/

    sudo apt update
    
    sudo apt upgrade -y
    sudo apt install -y vim vlc screen ssh
    sleep 10
    echo "OPENCV_SETUPSTATE="2"" > .setupstate
    sudo reboot


    ;;


  2)
    echo -e "Install CUDA and Graphics driver\n\n"
    ./cuda-10-0.sh
 

    echo -e "Install cuDNN\n\n"
    ./cudnn-10-0.sh




    sudo apt update
    sudo apt upgrade -y
    sleep 10
    echo "OPENCV_SETUPSTATE="3"" > .setupstate
    sudo reboot
    ;;

  3)
    echo -e "Install OpenCV requirements \n\n"
    ./prepare-opencv.sh

    sudo apt update
    sudo apt upgrade -y
    sleep 10
    echo "OPENCV_SETUPSTATE="4"" > .setupstate
    sudo reboot

    ;;

  4)
    echo -e "Build FFMPEG \n\n"
    ./build-ffmpeg.sh
    sudo apt update
    sudo apt upgrade -y
    sleep 10
    echo "OPENCV_SETUPSTATE="5"" > .setupstate
    sudo reboot
    ;;

  5)
    echo -e "Build OpenCV \n\n"
    ./build-opencv.sh

    sudo apt update
    sudo apt upgrade -y
    sleep 10
    echo "OPENCV_SETUPSTATE="6"" > .setupstate
    sudo reboot
    ;;


  *)
    echo -e "unknown install state \n\n"
    ;;
esac

