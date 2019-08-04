set(srcs
        "touchpad_api.c"
)

idf_component_register(SRCS "${srcs}"
        INCLUDE_DIRS include)

target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DHAVE_CONFIG_H")