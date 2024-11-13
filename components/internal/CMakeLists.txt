idf_component_register(SRCS "water.cpp" "display.cpp" "input.cpp" #"internal/input_data.cpp"
                    INCLUDE_DIRS "../../" "."
                    REQUIRES display arduino touch
)
