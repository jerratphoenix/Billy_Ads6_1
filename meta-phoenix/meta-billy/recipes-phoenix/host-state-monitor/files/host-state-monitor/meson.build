project('host-state-monitor', 'cpp',
        version : '1.0',
        meson_version: '>=0.56.0',
        default_options: [
            'werror=true',
            'warning_level=3',
            'cpp_std=c++20',
            'buildtype=debug',
            'b_lto=true',
           ])

#Validate the c++ Standard
if get_option('cpp_std') != 'c++20'
    error('This project requires c++20 support')
endif

#Get compiler and default build type
cxx = meson.get_compiler('cpp')
build = get_option('buildtype')
optimization = get_option('optimization')

#Set Compiler Security flags
security_flags = [
    '-fstack-protector-strong',
    '-fPIE',
    '-fPIC',
    '-D_FORTIFY_SOURCE=2',
    '-Wformat',
    '-Wformat-security'
]

## Add security flags for builds of type 'release','debugoptimized' and 'minsize'
if not (get_option('buildtype') == 'plain' or get_option('buildtype').startswith('debug'))
    add_project_arguments(
        cxx.get_supported_arguments([
            security_flags
    ]),
    language: 'cpp')
endif

# Add option to disable -Werror=write-strings
add_project_arguments('-Wno-error=write-strings', language: 'cpp')

#Include Directories
incdir = include_directories('include')

#Find the dependency modules, if not found use meson wrap to get them
#automatically during the configure step
sdbusplus = dependency(
    'sdbusplus',
    fallback : ['sdbusplus', 'sdbusplus_dep'],
    required : true)

boost = dependency(
    'boost',
    version: '>=1.77',
    fallback : ['boost', 'boost_dep'],
    required: true,
    )

    #Boost dependency configuration
add_project_arguments(
    cxx.get_supported_arguments([
        '-DBOOST_ASIO_USE_TS_EXECUTOR_AS_DEFAULT',
        '-DBOOST_ASIO_DISABLE_THREADS',
        '-DBOOST_BEAST_USE_STD_STRING_VIEW',
        '-DBOOST_ERROR_CODE_HEADER_ONLY',
        '-DBOOST_SYSTEM_NO_DEPRECATED',
        '-DBOOST_ASIO_NO_DEPRECATED',
        '-DBOOST_ALL_NO_LIB',
        '-DBOOST_NO_RTTI',
        '-DBOOST_NO_TYPEID',
        '-DBOOST_COROUTINES_NO_DEPRECATION_WARNING',
        '-DBOOST_URL_STANDALONE',
        '-DBOOST_URL_HEADER_ONLY'
    ]),
    language : 'cpp')

i2c = meson.get_compiler('cpp').find_library('i2c')

#Source files
srcfiles_app = [
    'src/main.cpp',
    'src/HostStateManager.cpp',
    'src/HostStateMonitor.cpp'
    ]

bindir = get_option('prefix') + '/' + get_option('bindir')

packagedir = join_paths(
    get_option('prefix'), 
    get_option('datadir'), 
    meson.project_name(),
)

#Install service files
systemd_service_dir = 'not requested'
if (get_option('install-service').enabled())
    systemd = dependency('systemd', required: true)
    systemd_service_dir = systemd.get_variable(pkgconfig: 'systemdsystemunitdir')

    install_data('xyz.openbmc_project.host-state-monitor.service', 
        install_dir: systemd_service_dir)
endif

nlohmann_json = declare_dependency(
    include_directories: include_directories('.')
)

summary({
        'Build Type' : build,
        'Optimization' : optimization,
    }, section: 'Build Info')
summary({
        'prefix' : get_option('prefix'),
        'bindir' : bindir,
        'packagedir' : packagedir,
        'systemd service file': systemd_service_dir,
    }, section : 'Directories')

#Generate the host-state-monitor executable and the test binary
executable('host-state-monitor',srcfiles_app,
            include_directories : incdir,
            dependencies: [
                            boost, 
                            sdbusplus,
                            i2c,
                            nlohmann_json,
                          ],
            install: true,
            install_dir:bindir
            )

#Tests are placed in the tests folder, with it's own meson.build
if (get_option('tests').enabled())
    subdir('tests')
endif
