@echo off
echo "Starting AVT Remote Camera Server"
server.exe -help
echo "================================"
server.exe -out-dir=out -out-file=cap#fi8 -out-ext=jpg -w=800 -h=450 -fps=60 rtsp://admin@192.168.1.64//Streaming/Channels/101