sudo echo 0 > /sys/devices/system/cpu/cpuquiet/tegra_cpuquiet/enable
sudo echo 1 > /sys/devices/system/cpu/cpu0/online
sudo echo 1 > /sys/devices/system/cpu/cpu1/online
sudo echo 1 > /sys/devices/system/cpu/cpu2/online
sudo echo 1 > /sys/devices/system/cpu/cpu3/online
echo "All cores enabled!"
exit
