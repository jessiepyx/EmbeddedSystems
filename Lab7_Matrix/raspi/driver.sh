sudo insmod ../Ubuntu/modules/driver.ko
sleep 1s
cd /dev
sudo chown pi:pi matrix
echo 0123456789 > matrix
sleep 10s
sudo rmmod driver
