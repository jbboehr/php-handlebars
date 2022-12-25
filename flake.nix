{
  description = "php-handlebars";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs";
    flake-utils = {
      url = "github:numtide/flake-utils";
    };
    mustache_spec = {
      url = "github:jbboehr/mustache-spec";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    handlebars_spec = {
      url = "github:jbboehr/handlebars-spec";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    handlebars-c = {
      url = "github:jbboehr/handlebars.c";
      inputs.mustache_spec.follows = "mustache_spec";
      inputs.handlebars_spec.follows = "handlebars_spec";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    php-psr = {
      url = "github:jbboehr/php-psr";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    gitignore = {
      url = "github:hercules-ci/gitignore.nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    pre-commit-hooks = {
      url = "github:cachix/pre-commit-hooks.nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = { self, nixpkgs, flake-utils, mustache_spec, handlebars_spec, handlebars-c, php-psr, gitignore, pre-commit-hooks }@args:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        php = pkgs.php;

        src = pkgs.lib.cleanSourceWith {
          filter = (path: type: (builtins.all (x: x != baseNameOf path)
            [ ".idea" ".git" "nix" "ci.nix" ".travis.sh" ".travis.yml" ".github" "flake.nix" "flake.lock" ]));
          src = gitignore.lib.gitignoreSource ./.;
        };

        pre-commit-check = pre-commit-hooks.lib.${system}.run {
          inherit src;
          hooks = {
            #editorconfig-checker.enable = true;
            #markdownlint.enable = true;
            nixpkgs-fmt.enable = true;
          };
        };
      in
      rec {
        packages = flake-utils.lib.flattenTree rec {
          php-handlebars = pkgs.callPackage ./nix/derivation.nix {
            mustache_spec = mustache_spec.packages.${system}.mustache-spec;
            handlebars_spec = handlebars_spec.packages.${system}.handlebars-spec;
            handlebarsc = handlebars-c.packages.${system}.handlebars-c;
            php_psr = args.php-psr.packages.${system}.php-psr;
            inherit (gitignore.lib) gitignoreSource;
            inherit php;
            inherit (php) buildPecl;
          };
          php-handlebars-dist = pkgs.runCommand "handlebars-pecl.tgz"
            {
              buildInputs = [ php ];
              src = php-handlebars.src;
            } ''
            cp -r $src/* .
            PHP_PEAR_PHP_BIN=${php}/bin/php pecl package
            mv handlebars-*.tgz $out
          '';
          default = php-handlebars;
        };

        devShells.default = pkgs.mkShell {
          inputsFrom = builtins.attrValues self.packages.${system};
          buildInputs = with pkgs; [ nixpkgs-fmt pre-commit editorconfig-checker ];
          shellHook = ''
            ${pre-commit-check.shellHook}
          '';
        };

        checks = {
          # @todo we could move/copy everything in nix/ci.nix here
          inherit pre-commit-check;
          inherit (packages) default;
        };

        formatter = pkgs.nixpkgs-fmt;
      }
    );
}
