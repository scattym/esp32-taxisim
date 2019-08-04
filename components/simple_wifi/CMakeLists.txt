set(srcs
        "simple_wifi.c"
)

idf_component_register(SRCS "${srcs}"
        INCLUDE_DIRS include
        REQUIRES nvs_flash)

target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DHAVE_CONFIG_H")