gtest_dep = dependency(
    'gtest',
    fallback : ['gtest', 'gtest_dep'],
    required : true)
    
gmock_dep = dependency(
    'gmock',
    fallback : ['gtest', 'gmock_dep'],
    required : true)

summary(
    {
    'built' : 'YES',
    'config path': meson.current_build_dir(),
    }, section: 'Unit tests')

test(
    'scm-i2c-memory-map-ut',
    executable(
        'scm-i2c-memory-map-ut',
        [
            'src/main.cpp',

            'src/DBusTranslator_test.cpp',
            'src/EfficientMap_test.cpp',
            'src/RegisterAccess_test.cpp',
        ],
        dependencies: [
            boost,
            gmock_dep,
            gtest_dep,
            sdbusplus,
            i2c,
        ],
        include_directories: ['../include', 'include'],
    ),
    workdir: meson.current_build_dir(),
    timeout: 120,
)
