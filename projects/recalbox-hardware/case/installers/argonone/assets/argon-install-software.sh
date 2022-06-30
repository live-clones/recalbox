#!/bin/bash

# Variables
daemonconfigfile=/recalbox/share/system/$daemonname.conf
# Generate configuration file for fan speed
if [ ! -f $daemonconfigfile ]; then
	touch $daemonconfigfile
	chmod 666 $daemonconfigfile

	echo '#' >> $daemonconfigfile
	echo '# Argon One Fan Configuration' >> $daemonconfigfile
	echo '#' >> $daemonconfigfile
	echo '# List below the temperature (Celsius) and fan speed (in percent) pairs' >> $daemonconfigfile
	echo '# Use the following form:' >> $daemonconfigfile
	echo '# min.temperature=speed' >> $daemonconfigfile
	echo '#' >> $daemonconfigfile
	echo '# Example:' >> $daemonconfigfile
	echo '# 55=10' >> $daemonconfigfile
	echo '# 60=55' >> $daemonconfigfile
	echo '# 65=100' >> $daemonconfigfile
	echo '#' >> $daemonconfigfile
	echo '# Above example sets the fan speed to' >> $daemonconfigfile
	echo '#' >> $daemonconfigfile
	echo '# NOTE: Lines begining with # are ignored' >> $daemonconfigfile
	echo '#' >> $daemonconfigfile
	echo '# Type the following at the command line for changes to take effect:' >> $daemonconfigfile
	echo '# '$daemonfanservice' restart' >> $daemonconfigfile
	echo '#' >> $daemonconfigfile
	echo '# Start below:' >> $daemonconfigfile
	echo '55=10' >> $daemonconfigfile
	echo '60=55' >> $daemonconfigfile
	echo '65=100' >> $daemonconfigfile
fi