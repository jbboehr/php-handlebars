let
    generateHandlebarsTestsForPlatform = { pkgs, phpAttr }:
        pkgs.recurseIntoAttrs {
            # std
            std = let
                php = pkgs.${phpAttr};
            in pkgs.callPackage ../default.nix {
                inherit php;
                inherit (php) buildPecl;
                astSupport = false;
            };
            # i686
            i686 = let
                php = pkgs.pkgsi686Linux.${phpAttr};
            in pkgs.pkgsi686Linux.callPackage ../default.nix {
                inherit php;
                inherit (php) buildPecl;
                astSupport = true;
            };
            # ast
            ast = let
                php = pkgs.${phpAttr};
            in pkgs.callPackage ../default.nix {
                inherit php;
                inherit (php) buildPecl;
                astSupport = true;
            };
            # clang
            clang = let
                php = pkgs.${phpAttr};
                stdenv = pkgs.clangStdenv;
            in pkgs.callPackage ../default.nix {
                inherit stdenv php;
                inherit (php) buildPecl;
                astSupport = true;
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
        inherit pkgs;
        phpAttr = "php73";
    };

    php74 = generateHandlebarsTestsForPlatform {
        inherit pkgs;
        phpAttr = "php74";
    };

#    php80 = let
#        path = builtins.fetchTarball {
#            url = https://github.com/NixOS/nixpkgs/archive/nixos-unstable.tar.gz;
#            name = "nixos-unstable";
#        };
#        pkgs = import (path) { system = k; };
#    in generateHandlebarsTestsForPlatform {
#        inherit pkgs;
#        phpAttr = "php80";
#    };
  }
) {
  x86_64-linux = {};
  # Uncomment to test build on macOS too
  # x86_64-darwin = {};
}
