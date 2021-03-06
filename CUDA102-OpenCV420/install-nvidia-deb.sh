#!/bin/bash

echo Script name: $0
echo Passed $# arguments 
if [ $# -ne 1 ]; then 
    echo "Illegal number of arguments"
    exit
fi


set -e  # Exit immediately if a command exits with a non-zero status. (Exit on error)
set -x  # Print commands and their arguments as they are executed.
set -u  # Treat unset variables as an error when substituting.

cd ~

case "$1" in

	"test")
		echo "TESTIING TESTING TESTING"
	;;

	"driver")

		# Add NVIDIA package repositories
		wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64/cuda-repo-ubuntu1804_10.1.243-1_amd64.deb
		sudo apt-key adv --fetch-keys https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64/7fa2af80.pub
		sudo dpkg -i cuda-repo-ubuntu1804_10.1.243-1_amd64.deb
		sudo apt update
		wget http://developer.download.nvidia.com/compute/machine-learning/repos/ubuntu1804/x86_64/nvidia-machine-learning-repo-ubuntu1804_1.0.0-1_amd64.deb
		sudo apt install ./nvidia-machine-learning-repo-ubuntu1804_1.0.0-1_amd64.deb
		sudo apt update

		# Install NVIDIA driver

		# For Ubuntu 19.10
		#sudo apt install -y --no-install-recommends nvidia-driver-440

		# This was, TODO investigate what to do.
		sudo apt install -y --no-install-recommends nvidia-driver-430


		# Reboot. Check that GPUs are visible using the command: nvidia-smi

	;;
	
  
  	"cuda")

		# Install development and runtime libraries (~4GB)
		
		# This was the install. TODO investigate
		sudo apt install -y --no-install-recommends cuda-10-1 libcudnn7=7.6.4.38-1+cuda10.1 libcudnn7-dev=7.6.4.38-1+cuda10.1

		# For Ubuntu 19.10 
		#sudo apt install -y cuda-10-1 libcudnn libcudnn-dev

		# Install TensorRT. Requires that libcudnn7 is installed above.
		sudo apt install -y --no-install-recommends libnvinfer6=6.0.1-1+cuda10.1 \
		libnvinfer-dev=6.0.1-1+cuda10.1 \
		libnvinfer-plugin6=6.0.1-1+cuda10.1


	;;
	
	
	 *)
		echo -e "unknown install state \n"
		echo -e "pass driver or cuda \n"
		


	;;

esac

echo -e "\n\n"
echo $0 $1      ---       completed

