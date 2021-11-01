#!/bin/sh

COUNT=0

STATUS_READY="s \"xyz.openbmc_project.Software.Activation.Activations.Ready\""
STATUS_ACTIVATING="s \"xyz.openbmc_project.Software.Activation.Activations.Activating\""

# Get only the first FW file uploaded to the /tmp/images directory
FWID=($(ls /tmp/images))
FWID=${FWID[0]}

while [ -z "$FWID" -o "image-update.tar" = "$FWID" ] ; do
	# Limit the number of times we check
	COUNT=$(($COUNT+1))

	echo "Checking for FW..."

	if [ $COUNT -ge 5 ] ; then
		echo "No FW found"
		exit 1
	fi

	sleep 1
 
	# Check the directory again
	FWID=($(ls /tmp/images))
	FWID=${FWID[0]}
done

echo "Found FWID $FWID"
	
ACTIVATION=`busctl get-property xyz.openbmc_project.Software.BMC.Updater /xyz/openbmc_project/software/$FWID xyz.openbmc_project.Software.Activation Activation`

if [ "$ACTIVATION" != "$STATUS_READY" ] ; then
		
	echo "FW image not ready: $ACTIVATION"
	exit 1
	
else

	# Activate the image (do the update)
	busctl set-property xyz.openbmc_project.Software.BMC.Updater /xyz/openbmc_project/software/$FWID xyz.openbmc_project.Software.Activation RequestedActivation s xyz.openbmc_project.Software.Activation.RequestedActivations.Active

	while [ 1 ] ; do

		ACTIVATION=`busctl get-property xyz.openbmc_project.Software.BMC.Updater /xyz/openbmc_project/software/$FWID xyz.openbmc_project.Software.Activation Activation`
		#PROGRESS=`busctl get-property xyz.openbmc_project.Software.BMC.Updater /xyz/openbmc_project/software/$FWID xyz.openbmc_project.Software.ActivationProgress Progress`

		if [ "$ACTIVATION" != "$STATUS_ACTIVATING" ] ; then

			echo "Failed to update: $ACTIVATION"
			exit 1
				
		fi
			
		#echo "Current activation State: $ACTIVATION"
		echo "Activating..."

		sleep 1
	done

fi

exit 1
