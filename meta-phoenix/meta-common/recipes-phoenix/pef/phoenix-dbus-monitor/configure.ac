# Initialization
AC_PREREQ([2.69])
AC_INIT([phoenix-dbus-monitor], [1.0], [])
AC_LANG([C++])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror foreign dist-xz])
AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_CXX
AM_PROG_AR
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_AWK

# Checks for libraries.
PKG_CHECK_MODULES([SDBUSPLUS], [sdbusplus])
PKG_CHECK_MODULES([SDEVENTPLUS], [sdeventplus])
PKG_CHECK_MODULES([PHOSPHOR_DBUS_INTERFACES], [phosphor-dbus-interfaces])
PKG_CHECK_MODULES([PHOSPHOR_LOGGING], [phosphor-logging])
PKG_CHECK_MODULES([PHOSPHOR_SNMP], [phosphor-snmp])

# Check for sdbus++
AC_PATH_PROG([SDBUSPLUSPLUS], [sdbus++])
AS_IF([test "x$SDBUSPLUSPLUS" == "x"],
    AC_MSG_ERROR(["Requires sdbus++"]))

# Checks for typedefs, structures, and compiler characteristics.
AX_CXX_COMPILE_STDCXX_17([noext])
AX_APPEND_COMPILE_FLAGS([-fpic -Wall -Werror -Wno-psabi], [CXXFLAGS])

# Checks for library functions.
LT_INIT

AC_ARG_ENABLE([oe-sdk],
    AS_HELP_STRING([--enable-oe-sdk], [Link testcases absolutely against OE SDK so they can be ran within it.])
)
AC_ARG_VAR(OECORE_TARGET_SYSROOT,
    [Path to the OE SDK SYSROOT])
AS_IF([test "x$enable_oe_sdk" == "xyes"],
    AS_IF([test "x$OECORE_TARGET_SYSROOT" == "x"],
          AC_MSG_ERROR([OECORE_TARGET_SYSROOT must be set with --enable-oe-sdk])
    )
    AC_MSG_NOTICE([Enabling OE-SDK at $OECORE_TARGET_SYSROOT])
    [
        testcase_flags="-Wl,-rpath,\${OECORE_TARGET_SYSROOT}/lib"
        testcase_flags="${testcase_flags} -Wl,-rpath,\${OECORE_TARGET_SYSROOT}/usr/lib"
        testcase_flags="${testcase_flags} -Wl,-dynamic-linker,`find \${OECORE_TARGET_SYSROOT}/lib/ld-*.so | sort -r -n | head -n1`"
    ]
    AC_SUBST([OESDK_TESTCASE_FLAGS], [$testcase_flags])
)

#AC_ARG_VAR(OBJ_EVENT, [The event manager D-Bus object path])
#AS_IF([test "x$OBJ_EVENT" == "x"], [OBJ_EVENT="/xyz/openbmc_project/phoenix/pef"])
#AC_DEFINE_UNQUOTED([OBJ_EVENT], ["$OBJ_EVENT"], [The event manager D-Bus object path])
#
#AC_ARG_VAR(BUSNAME_PEF, [The D-Bus busname to own])
#AS_IF([test "x$BUSNAME_PEF" == "x"], [BUSNAME_PEF="xyz.openbmc_project.phoenix.PEF"])
#AC_DEFINE_UNQUOTED([BUSNAME_PEF], ["$BUSNAME_PEF"], [The D-Bus busname to own])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
