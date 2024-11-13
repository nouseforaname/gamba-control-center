idf_component_register(SRCS "gamba-display.cpp" "water.cpp" "display.cpp" "input.cpp" #"internal/input_data.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES display arduino touch
)
