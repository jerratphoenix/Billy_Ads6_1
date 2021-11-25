project(
    'phoenix-gpio-monitor',
    'cpp',
    default_options: [
        'warning_level=3',
        'werror=true',
        'cpp_std=c++20',
        'buildtype=debugoptimized'
    ],
    license: 'Apache-2.0',
    version: '1.0',
    meson_version: '>=0.57.0',
)

libgpiod = dependency('libgpiod')
phosphor_logging = dependency('phosphor-logging')
sdbusplus = dependency('sdbusplus')
systemd = dependency('systemd')

boost_args = ['-DBOOST_ASIO_DISABLE_THREADS',
              '-DBOOST_ERROR_CODE_HEADER_ONLY',
              '-DBOOST_SYSTEM_NO_DEPRECATED']

systemd_system_unit_dir = systemd.get_pkgconfig_variable(
    'systemdsystemunitdir',
    define_variable: ['prefix', get_option('prefix')])

configure_file(
    copy: true,
    input: 'phoenix-gpio-monitor.service',
    install: true,
    install_dir: systemd_system_unit_dir,
    output: 'phoenix-gpio-monitor.service'
)

configure_file(
    copy: true,
    input: 'phoenix-gpio-monitor.json',
    install: true,
    install_dir: '/usr/share/phoenix-gpio-monitor',
    output: 'phoenix-gpio-monitor.json'
)

executable(
   'phoenix-gpio-monitor',
   'gpioMonMain.cpp',
   'gpioMon.cpp',
   dependencies: [
        phosphor_logging,
        sdbusplus,
        libgpiod,
    ],
    cpp_args: boost_args,
    install: true,
)
