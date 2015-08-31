sudo ifconfig lo multicast
sudo route add -net 224.0.0.0 netmask 240.0.0.0 dev lo
sudo sysctl -w net.core.rmem_max=8388608;sudo sysctl -w net.core.wmem_max=8388608;sudo sysctl -w net.core.rmem_default=65536;sudo sysctl -w net.core.wmem_default=65536;sudo sysctl -w net.ipv4.tcp_rmem='4096 87380 8388608';sudo sysctl -w net.ipv4.tcp_wmem='4096 65536 8388608';sudo sysctl -w net.ipv4.tcp_mem='8388608 8388608 8388608';sudo sysctl -w net.ipv4.route.flush=1
route -n