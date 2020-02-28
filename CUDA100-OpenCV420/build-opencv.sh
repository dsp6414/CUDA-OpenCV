#!/bin/bash

# Compileing an GPU example like this
# ~/opencv/samples/gpu$ nvcc `pkg-config --libs opencv4` -L. -L/usr/local/cuda/lib -lcuda -lcudart `pkg-config --cflags opencv4` -I . -I /usr/local/cuda-10.1 bgfg_segm.cpp -o bgfg_seg

# In case of rebuild, delete everything before building
sudo rm -rf ~/opencv/build
mkdir ~/opencv/build

source .setupvars

# We build here
cd ~/opencv/build

# The following NEW packages will be installed:
#  libatk-bridge2.0-dev libatspi2.0-dev libdbus-1-dev libepoxy-dev libgtk-3-dev
#  libxkbcommon-dev libxtst-dev wayland-protocols x11proto-record-dev

# $ sudo apt-get install build-essential cmake unzip pkg-config
# $ sudo apt-get install libjpeg-dev libpng-dev libtiff-dev
# $ sudo apt-get install libavcodec-dev libavformat-dev libswscale-dev
# $ sudo apt-get install libv4l-dev libxvidcore-dev libx264-dev
# $ sudo apt-get install libgtk-3-dev
# $ sudo apt-get install libatlas-base-dev gfortran
# $ sudo apt-get install python3-dev

# python yolo_object_detection.py --input ../example_videos/janie.mp4 --output ../output_videos/yolo_janie.avi --yolo yolo-coco --display 0 --use-gpu 1

cmake -D CMAKE_BUILD_TYPE=RELEASE \
	-D CMAKE_INSTALL_PREFIX=/usr/local \
	-D INSTALL_PYTHON_EXAMPLES=ON \
	-D INSTALL_C_EXAMPLES=OFF \
	-D OPENCV_ENABLE_NONFREE=ON \
	-D WITH_CUDA=ON \
	-D WITH_CUDNN=ON \
	-D OPENCV_DNN_CUDA=ON \
	-D ENABLE_FAST_MATH=1 \
	-D CUDA_FAST_MATH=1 \
        -D CUDA_ARCH_BIN=$SCRIPT_CUDA_ARCH_BIN \
	-D WITH_CUBLAS=1 \
        -D CUDA_TOOLKIT_ROOT_DIR=$SCRIPT_CUDAPATH \
	-D OPENCV_EXTRA_MODULES_PATH=~/opencv_contrib/modules \
	-D HAVE_opencv_python3=ON \
        -D PYTHON_EXECUTABLE=$(which python3) \
	-D BUILD_EXAMPLES=ON ..





#### Make clean in case we run script multiple times
make clean

#### Now make the OpenCV project
make -j$(nproc)

#### At this point the sudo password have timed out, you need to enter it again 
#### TODO. Try to fix this issue....
sudo make install 

#TODO I'm not certain, need to investigare
sudo ldconfig

# Some applications have hardcoded links to example data files like samples/gpu/bgfg_segm.cpp 
# Link in the sample data
#ln -s ~/opencv/samples/data/* ~/opencv/build/data/

# cd ~/.virtualenvs/opencv_cuda/lib/python3.6/site-packages/
# ln -s /usr/local/lib/python3.6/site-packages/cv2/python-3.6/cv2.cpython-36m-x86_64-linux-gnu.so cv2.so


