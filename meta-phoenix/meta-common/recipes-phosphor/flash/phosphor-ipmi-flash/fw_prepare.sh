#!/bin/sh

# Delete the verification results
rm /tmp/bmc.verify

# Delete all pending images in /tmp/images (doesn't seem to do anything)
busctl call xyz.openbmc_project.Software.BMC.Updater /xyz/openbmc_project/software xyz.openbmc_project.Collection.DeleteAll DeleteAll

# Get only the first FW file uploaded to the /tmp/images directory
FWID=($(ls /tmp/images))
FWID=${FWID[0]}

if [ "" = "$FWID" ] ; then

        exit 0

else

        echo "Deleting FWID $FWID"
	rm -rf /tmp/images/$FWID

	# Call dbus delete for FW that we just removed (also doesn't seem to do anything but do it anyhow to be safe)
	busctl call xyz.openbmc_project.Software.BMC.Updater /xyz/openbmc_project/software/$FWID xyz.openbmc_project.Object.Delete Delete

fi

exit 0

