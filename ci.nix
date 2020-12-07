let
    generateHandlebarsTestsForPlatform = { pkgs, path, phpAttr }:
        pkgs.recurseIntoAttrs {
            handlebars = let
                php = pkgs.${phpAttr};
            in pkgs.callPackage ./default.nix {
                inherit php;
                inherit (php) buildPecl;
            };
            # cross-compile for 32bit
            handlebars32bit = let
                php = pkgs.pkgsi686Linux.${phpAttr};
            in pkgs.pkgsi686Linux.callPackage ./default.nix {
                inherit php;
                inherit (php) buildPecl;
            };
        };
in
builtins.mapAttrs (k: _v:
  let
    path = builtins.fetchTarball {
        url = https://github.com/NixOS/nixpkgs/archive/nixos-20.09.tar.gz;
        name = "nixos-20.09";
    };
    pkgs = import (path) { system = k; };
  in
  pkgs.recurseIntoAttrs {
    php73 = generateHandlebarsTestsForPlatform {
        inherit pkgs path;
        phpAttr = "php73";
    };

    php74 = generateHandlebarsTestsForPlatform {
        inherit pkgs path;
        phpAttr = "php74";
    };
  }
) {
  x86_64-linux = {};
  # Uncomment to test build on macOS too
  # x86_64-darwin = {};
}
