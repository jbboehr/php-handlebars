let
    generateHandlebarsDrv = { pkgs, php }:
        let
            mustache_spec = pkgs.callPackage (import ((fetchTarball {
                url = https://github.com/jbboehr/mustache-spec/archive/5b85c1b58309e241a6f7c09fa57bd1c7b16fa9be.tar.gz;
                sha256 = "1h9zsnj4h8qdnzji5l9f9zmdy1nyxnf8by9869plyn7qlk71gdyv";
            }))) {};

            handlebars_spec = pkgs.callPackage (import ((fetchTarball {
                url = https://github.com/jbboehr/handlebars-spec/archive/9306f3062b4b03d5179c33834bee7263a621639a.tar.gz;
                sha256 = "0gn9v88fxdf0wwgd8ix1xfxvif89mgfhxkp316arv5ljbibx2x9h";
            }))) {};
        in
        pkgs.runCommand "pecl-handlebars.tgz" {
            buildInputs = [ php ];
            src = builtins.filterSource
                (path: type: baseNameOf path != ".idea" && baseNameOf path != ".git" && baseNameOf path != "ci.nix")
                ./.;
            HANDLEBARS_SPEC_DIR = "${handlebars_spec}/share/handlebars-spec";
            MUSTACHE_SPEC_DIR = "${mustache_spec}/share/mustache-spec";
        } ''
            cp -r $src/* .
            chmod -R +w tests
            php generate-tests.php
            pecl package | tee tmp.txt
            pecl_tgz=$(cat tmp.txt | grep -v Warning | awk '{print $2}')
            echo $pecl_tgz
            cp $pecl_tgz $out
        '';

    generateHandlebarsTestsForPlatform = { pkgs, path, phpAttr, phpHandlebarsSrc }:
        pkgs.recurseIntoAttrs {
            handlebars = let
                php = pkgs.${phpAttr};
            in pkgs.callPackage ./default.nix {
                inherit phpHandlebarsSrc;
                php = pkgs.${phpAttr};
                buildPecl = pkgs.callPackage "${path}/pkgs/build-support/build-pecl.nix" { inherit php; };
            };
            # cross-compile for 32bit
            handlebars32bit = let
                php = pkgs.pkgsi686Linux.${phpAttr};
            in pkgs.pkgsi686Linux.callPackage ./default.nix {
                inherit phpHandlebarsSrc;
                buildPecl = pkgs.pkgsi686Linux.callPackage "${path}/pkgs/build-support/build-pecl.nix" { inherit php; };
            };
        };
in
builtins.mapAttrs (k: _v:
  let
    path = builtins.fetchTarball {
       url = https://github.com/NixOS/nixpkgs/archive/release-19.09.tar.gz;
       name = "nixpkgs-19.09";
    };
    pkgs = import (path) { system = k; };

    phpHandlebarsSrc = generateHandlebarsDrv {
        inherit pkgs;
        inherit (pkgs) php;
    };
  in
  pkgs.recurseIntoAttrs {
    peclDist = phpHandlebarsSrc;

    php72 = generateHandlebarsTestsForPlatform {
        inherit pkgs path phpHandlebarsSrc;
        phpAttr = "php72";
    };

    php73 = generateHandlebarsTestsForPlatform {
        inherit pkgs path phpHandlebarsSrc;
        phpAttr = "php73";
    };

    php74 = let
        path = builtins.fetchTarball {
           url = https://github.com/NixOS/nixpkgs/archive/release-20.03.tar.gz;
           name = "nixpkgs-20.03";
        };
        pkgs = import (path) { system = k; };
    in generateHandlebarsTestsForPlatform {
        inherit pkgs path phpHandlebarsSrc;
        phpAttr = "php74";
    };
  }
) {
  x86_64-linux = {};
  # Uncomment to test build on macOS too
  # x86_64-darwin = {};
}
