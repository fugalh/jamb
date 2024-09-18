set -e
./build.sh
{
    sleep 1
    aconnect -l
} &
exec linux/jamb
