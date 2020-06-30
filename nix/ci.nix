let
    generateHandlebarsTestsForPlatform = { pkgs, path, phpAttr }:
        pkgs.recurseIntoAttrs {
            # std
            std = let
                php = pkgs.${phpAttr};
            in pkgs.callPackage ../default.nix {
                inherit php;
                buildPecl = pkgs.callPackage "${path}/pkgs/build-support/build-pecl.nix" { inherit php; };
                astSupport = false;
            };
            # i686
            i686 = let
                php = pkgs.pkgsi686Linux.${phpAttr};
            in pkgs.pkgsi686Linux.callPackage ../default.nix {
                inherit php;
                buildPecl = pkgs.pkgsi686Linux.callPackage "${path}/pkgs/build-support/build-pecl.nix" { inherit php; };
                astSupport = true;
            };
            # ast
            ast = let
                php = pkgs.${phpAttr};
            in pkgs.callPackage ../default.nix {
                inherit php;
                buildPecl = pkgs.callPackage "${path}/pkgs/build-support/build-pecl.nix" { inherit php; };
                astSupport = true;
            };
            # clang
            clang = let
                php = pkgs.${phpAttr};
                stdenv = pkgs.clangStdenv;
            in pkgs.callPackage ../default.nix {
                inherit stdenv php;
                buildPecl = pkgs.callPackage "${path}/pkgs/build-support/build-pecl.nix" { inherit php stdenv; };
                astSupport = true;
            };
        };
in
builtins.mapAttrs (k: _v:
  let
    path = builtins.fetchTarball {
       url = https://github.com/NixOS/nixpkgs-channels/archive/nixos-20.03.tar.gz;
       name = "nixos-20.03";
    };
    pkgs = import (path) { system = k; };
  in
  pkgs.recurseIntoAttrs {
    php72 = generateHandlebarsTestsForPlatform {
        inherit pkgs path;
        phpAttr = "php72";
    };

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
