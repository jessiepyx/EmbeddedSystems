sudo insmod ../Ubuntu/modules/driver.ko
cd /dev
sudo chown pi:pi matrix
echo 0123456789 > matrix
sudo rmmod driver
