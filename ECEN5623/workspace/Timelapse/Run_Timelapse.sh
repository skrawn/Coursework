cd Debug
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/cuda-6.5/lib:/usr/include:/usr/include/arm-linux-gnueabihf:/usr/lib/gcc/arm-linux-gnueabihf/4.8/include:/usr/lib/gcc/arm-linux-gnueabihf/4.8/include-fixed:/usr/local/include:/usr/lib:/usr/local/lib
echo $LD_LIBRARY_PATH
sudo ./Timelapse 0

