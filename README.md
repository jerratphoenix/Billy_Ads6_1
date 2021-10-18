# Copyright © 2021 Phoenix Technologies Ltd.

### Prerequisites:
Please read https://github.com/openbmc/openbmc#openbmc and its links, \
as well as https://github.com/Intel-BMC/openbmc#openbmc

### Download the source from GitHub with a Phoenix Technologies Ltd. account.
```bash
git clone git@github.com:pteceng/ServerBMC_GM.git
cd ServerBMC_GM
```

### How to build OpenBMC ServerBMC_GM with `"debug-tweaks"` feature enabled.
target **Phoenix Technologies Ltd. Intel Wolf Pass** (i.e. S2600WF)
```bash
source setup wolfpass
bitbake obmc-phosphor-image
```

target **Phoenix Technologies Ltd. Intel Brighton City** 
```bash
source setup brightoncity
bitbake obmc-phosphor-image
```

target **Phoenix Technologies Ltd. Intel Archer City** 
```bash
source setup archercity
bitbake obmc-phosphor-image
```

### All of the other OpenBMC Targets are still possible choices
Any build requires an environment set up according to your hardware target.
There is a special script in the root of this repository that can be used
to configure the environment as needed. The script is called `setup` and
takes the name of your hardware target as an argument.

The script needs to be sourced while in the top directory of the OpenBMC
repository clone, and, if run without arguments, will display the list
of supported hardware targets, see the following example:

```
$ source setup <machine> [build_dir]
Target machine must be specified. Use one of:

bletchley               evb-ast2500               g220a
lanyang                 olympus-nuvoton           romulus
witherspoon             evb-ast2600               gbs
mihawk                  on5263m5                  s2600wf
witherspoon-tacoma      centriq2400-rep	          evb-npcm750
gsj                     mtjade                    p10bmc
stardragon4800-rep2     wolfpass                  dl360poc
evb-zx3-pm3             hr630                     neptune
palmetto                swift                     x11spi
e3c246d4i               f0b                       hr855xg2
nicole                  qemuarm                   thor
yosemitev2              ethanolx                  fp5280g2
kudo                    olympus                   quanta-q71l
tiogapass               zaius
```

Once you know the target (e.g. romulus), source the `setup` script as follows:

```
source setup romulus
```

For evb-ast2500, please use the below command to specify the machine config,
because the machine in `meta-aspeed` layer is in a BSP layer and does not
build the openbmc image.

```
TEMPLATECONF=meta-evb/meta-evb-aspeed/meta-evb-ast2500/conf . openbmc-env
```

### Build

```
bitbake obmc-phosphor-image
```

### Below is a work in progress (i.e. WIP).

### After the build is done, `build/<target>/tmp/deploy/` has the useful results.
```bash
build/<target>/tmp/deploy/images/<target> has the binary files including the .mtd (file for Flashing the SPI part with a DediProg)
build/<target>/tmp/deploy/licenses/ has all the licenses files; what you most likely want is license.manifest so
cd build/<target>/tmp/deploy/licenses/
find ./ -name license.manifest -print
that will show you the license.manifest files are, there maybe more than one.
```

### To build / package the required open source items add the following lines to `build/<target>/conf/local.conf` after `source openbmc-env` and before `bitbake obmc-phosphor-image`, then after the build is done there will be a `"sources"` directory in `build/<target>/tmp/deploy/`
```bash
INHERIT += "archiver"
ARCHIVER_MODE[src] = "original"
```

### Default User Credentials; if `"debug-tweaks"` feature is DISABLED.

To meet security requirements, this OpenBMC implementation will not have
default user credentials enabled by default.

IPMI commands are available to enable the root user for serial console access
and to enable users for IPMI.

There is also a `"debug-tweaks"` feature that can be added to a build to
re-enable the default user credentials.

#### Enable root user

Without `"debug-tweaks"`, the root user is disabled by default.

The following IPMI command can be used to enable the root user.  This root
user allows access to the BMC serial console, but cannot be used to access
IPMI.

IPMI OEM net function 0x30, command 0x5f.  For root user, the first byte is
0 followed by the password.

For example, to enable the root user with password `0penBmc1`:

```ipmitool raw 0x30 0x5f 0x00 0x30 0x70 0x65 0x6e 0x42 0x6d 0x63 0x31```

#### Enable IPMI.

Without `"debug-tweaks"`, there are no IPMI users by default.

The standard IPMI commands to set usernames and passwords are supported.
These users allow access to IPMI but cannot be  used to access the BMC serial console.

#### debug-tweaks

Debug features, including the default user credentials, can be enabled by
adding the `"debug-tweaks"` feature to the build by including the following
in your `local.conf` file:

```EXTRA_IMAGE_FEATURES += "debug-tweaks"```

And from https://github.com/pteceng/ServerBMC_GM/blob/master/meta-phoenix/meta-wolfpass/conf/local.conf.sample

```bash
#
# Extra image configuration defaults
#
# The EXTRA_IMAGE_FEATURES variable allows extra packages to be added to the generated
# images. Some of these options are added to certain image types automatically. The
# variable can contain the following options:
#  "dbg-pkgs"       - add -dbg packages for all installed packages
#                     (adds symbol information for debugging/profiling)
#  "dev-pkgs"       - add -dev packages for all installed packages
#                     (useful if you want to develop against libs in the image)
#  "ptest-pkgs"     - add -ptest packages for all ptest-enabled packages
#                     (useful if you want to run the package test suites)
#  "tools-sdk"      - add development tools (gcc, make, pkgconfig etc.)
#  "tools-debug"    - add debugging tools (gdb, strace)
#  "eclipse-debug"  - add Eclipse remote debugging support
#  "tools-profile"  - add profiling tools (oprofile, exmap, lttng, valgrind)
#  "tools-testapps" - add useful testing tools (ts_print, aplay, arecord etc.)
#  "debug-tweaks"   - make an image suitable for development
#                     e.g. ssh root access has a blank password
# There are other application targets that can be used here too, see
# meta/classes/image.bbclass and meta/classes/core-image.bbclass for more details.
# We default to enabling the debugging tweaks.
EXTRA_IMAGE_FEATURES = "debug-tweaks"
```
