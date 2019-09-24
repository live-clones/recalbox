#!/bin/bash

/usr/bin/emulationstation

while [ -f /var/run/es.restart ]; do
        rm /var/run/es.restart
        /usr/bin/emulationstation
done
