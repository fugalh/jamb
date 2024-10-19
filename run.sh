set -e
tup bin/jamb
{
    sleep 1
    aconnect -l
} &
exec bin/jamb
