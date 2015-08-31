cat $(ls Merge/Pkt* | sort -V | tr '\n' ' ') > catmn.mp4
sudo rm -r Merge
sudo mkdir Merge
sudo chmod -R 777 Merge
