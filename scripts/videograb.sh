#!/bin/sh
# sh is link to zsh on unitv2

#ssh m5stack@10.254.239.1
#sudo su

sdir="/media/sdcard/video"
mkdir -p "$sdir"
cd "$sdir"

if [ -d "$sdir" ] ; then
    fn="unitv2_$(date "+%Y-%m-%d-%H-%M").mp4"
    ffmpeg -f v4l2 -i /dev/video0 -c:v mpeg4 -s 640x480 "$sdir/unitv2_$(date "+%Y-%m-%d-%H-%M").mp4"
else
    echo "$sdir does not exist, no sdcard? exiting"
fi

#with audio:
#ffmpeg -f v4l2 -i /dev/video0  -f alsa -i default -c:v mpeg4 -s 640x480 -c:a aac -ar 44100 -ac 2 -b:a 192k output.mp4


