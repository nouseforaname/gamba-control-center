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
          pkgs = import nixpkgs {
            overlays = [ (import esp-dev) ];
          };
          # 👇 new! note that it refers to the path ./rust-toolchain.toml
        in
        with pkgs;
        {
          devShells.default = mkShell {
            # 👇 we can just use `rustToolchain` here:
            buildInputs = [ ];
          };
        }
      );
}
