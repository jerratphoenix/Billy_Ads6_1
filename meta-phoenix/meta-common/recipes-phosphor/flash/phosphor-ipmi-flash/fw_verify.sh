#!/bin/sh

# Signal that we're running verification
echo "running" > /tmp/bmc.verify

# Move the update file into the software image manager
mv /tmp/image-update.tar /tmp/images/image-update.tar

# Get only the first FW file uploaded to the /tmp/images directory
FWID=($(ls /tmp/images))
FWID=${FWID[0]}

while [ -z "$FWID" -o "image-update.tar" = "$FWID" ] ; do
	# Limit the number of times we check
        COUNT=$(($COUNT+1))

        echo "Checking for FW..."

        if [ $COUNT -ge 5 ] ; then
                echo "No FW found"
		echo "failed" > /tmp/bmc.verify
                exit 1
        fi

        sleep 1

        # Check the directory again
        FWID=($(ls /tmp/images))
	FWID=${FWID[0]}
done

echo "Found FWID $FWID"
echo "success" > /tmp/bmc.verify

