{
  description = "A very basic flake";

  inputs = {
    nixpkgs = { 
      url = "github:NixOS/nixpkgs/nixos-unstable";
    };
    flake-utils.url = "github:numtide/flake-utils";
    esp-dev = {
      url = "github:mirrexagon/nixpkgs-esp-dev";
      inputs.nixpkgs.follows = "nixpkgs";
      inputs.flake-utils.follows = "flake-utils";
    };

  };

  outputs = { self, nixpkgs, flake-utils, esp-dev }: 
   flake-utils.lib.eachDefaultSystem
      (system:
        let
          inherit ( pkgs ) mkShell;
          pkgs = import nixpkgs {
            inherit system;
            overlays = [ esp-dev.overlays.default ];
          };
          touch = pkgs.fetchFromGitHub {
            owner = "PaulStoffregen";
            repo  = "XPT2046_Touchscreen";
            rev  = "v1.4";
            hash  = "sha256-RZ8S6Fw2lbJptg+nNlzKR+p+eqGRLfL20xACxjQ9YWA=";
            postFetch = ''
              pushd $out
                find -maxdepth 1 -type d | tail -n+2 | xargs -n1 rm -r
                cat << EOF > CMakeLists.txt
idf_component_register(SRCS "XPT2046_Touchscreen.cpp"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES arduino)
set_source_files_properties(XPT2046_Touchscreen.cpp
    PROPERTIES COMPILE_FLAGS
    -Wno-error=misleading-indentation
)
EOF
            '';
          };
          arduino-core = pkgs.fetchFromGitHub {
            owner = "espressif";
            repo = "arduino-esp32";
              rev = "3.0.2"; # this needs to be a version that works with the idf.py version ( in this case 5.1.4
              hash = "sha256-Qi0qCJbjlhALFD2cljTCtmdekEVQCuH72BHWwKJXZdE=";
             #rev = "3.0.0"; # this needs to be a version that works with the idf.py version ( in this case 5.1.4
             #hash = "sha256-AV/uNUZ53WsG+LEfCl9EBUaFHA4MV4/YLlBNh3MEyf0=";
          };
          esp32s3-env =  pkgs.esp-idf-esp32.override {
            toolsToInclude = [
              "xtensa-esp32s3-elf"
              "esp-clang"
              "esp32ulp-elf"
              "openocd-esp32"
              "xtensa-esp-elf-gdb"
            ];  #pkgs.lib.fakeSha256;
            #rev = "v5.2.2";
            #sha256 = "sha256-I4YxxSGdQT8twkoFx3zmZhyLTSagmeLD2pygVfY/pEk="; #pkgs.lib.fakeSha256;
            rev = "v5.1.4";
            sha256 = "sha256-WJIt0ixoD3lhLg5A/ILUhcGPd7oSlMn6e6xsgRGP2Kk=";
          };

          display-sdk-src = pkgs.fetchFromGitHub {
            owner = "Bodmer";
            repo = "TFT_eSPI";
            rev = "V2.5.43";
            hash = "sha256-oLCK5KD+LYC2qAM5X/vG32JDrB9oCimu8ZobDcChskc=";
            postFetch = ''
              pushd $out
                rm Kconfig
                echo '
  set_source_files_properties(TFT_eSPI.cpp
      PROPERTIES COMPILE_FLAGS
      -Wno-error=misleading-indentation
  )' >> CMakeLists.txt
                sed -i 's%//#include\(.*\)T_HMI.h%#include\1T_HMI.h%g' User_Setup_Select.h
                #sed -i '/"hal\/gpio_ll.h"/a #include "driver\/gpio.h"' Processors/TFT_eSPI_ESP32_S3.h
              popd
            '';
          };
        in
        {
          devShells.default = mkShell  {
            nativeBuildInputs = [ esp32s3-env arduino-core display-sdk-src ];
            buildInputs = [ esp32s3-env arduino-core display-sdk-src ];

            shellHook = ''
mkdir -p components
ln -fs ${arduino-core} components/arduino
ln -fs ${display-sdk-src} components/display
ln -fs ${touch} components/touch
ESP32S3_TOOLING=$(dirname $(dirname $(which xtensa-esp32s3-elf-ar)))
cat << EOF > .clangd
CompileFlags:
  Add: [-mlong-calls, -isysroot=$ESP32S3_TOOLING ]
  Remove: [-fno-tree-switch-conversion, -mtext-section-literals, -mlongcalls, -fstrict-volatile-bitfields]
EOF
          '';
            DISPLAY_SDK_SRC = display-sdk-src;
            ARDUINO_CORE_SDK = arduino-core;
          };
        }
      );
}
