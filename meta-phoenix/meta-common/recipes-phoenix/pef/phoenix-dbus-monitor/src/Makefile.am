AM_DEFAULT_SOURCE_EXT = .cpp
AM_CPPFLAGS = -iquote ${top_srcdir}

bin_PROGRAMS = phoenix-dbus-monitor

phoenix_dbus_monitor_SOURCES = \
	main.cpp \
	phoenix_discrete_monitor.cpp \
	phoenix_threshold_monitor.cpp \
	json_parser.cpp

phoenix_dbus_monitor_LDADD = \
	$(SDBUSPLUS_LIBS) \
	$(SDEVENTPLUS_LIBS) \
	$(PHOSPHOR_DBUS_INTERFACES_LIBS) \
	$(PHOSPHOR_LOGGING_LIBS) \
	$(PHOSPHOR_SNMP_LIBS) \
	-lstdc++fs \
	-lpthread
phoenix_dbus_monitor_CXXFLAGS = \
	$(SDBUSPLUS_CFLAGS) \
	$(SDEVENTPLUS_CFLAGS) \
	$(PHOSPHOR_DBUS_INTERFACES_CFLAGS) \
	$(PHOSPHOR_LOGGING_CFLAGS) \
	$(PHOSPHOR_SNMP_CFLAGS)

SUBDIRS = .
