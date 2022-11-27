{
  description = "php-handlebars";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
    mustache_spec.url = "github:jbboehr/mustache-spec";
    handlebars_spec.url = "github:jbboehr/handlebars-spec";
    handlebars-c = {
        url = "github:jbboehr/handlebars.c";
        inputs.mustache_spec.follows = "mustache_spec";
        inputs.handlebars_spec.follows = "handlebars_spec";
    };
    php-psr.url = "github:jbboehr/php-psr";
    gitignore = {
        url = "github:hercules-ci/gitignore.nix";
        inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = { self, nixpkgs, flake-utils, mustache_spec, handlebars_spec, handlebars-c, php-psr, gitignore }@args:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        php = pkgs.php;
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
          default = php-handlebars;
        };

        devShells.default = pkgs.mkShell {
          inputsFrom = builtins.attrValues self.packages.${system};
          buildInputs = [];
        };
      }
    );
}
