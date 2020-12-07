# when using nix-shell, run "configurePhase" otherwise you'll have missing headers
# to use a specific version of php, run `nix-shell --arg php '(import <nixpkgs> {}).php73'`

{
  pkgs ? import <nixpkgs> {},
  php ? pkgs.php,
  buildPecl ? if builtins.hasAttr "buildPecl" php then php.buildPecl else pkgs.callPackage <nixpkgs/pkgs/build-support/build-pecl.nix> {
    inherit php;
  },

  phpHandlebarsVersion ? "v0.9.2",
  phpHandlebarsSrc ? ./.,
  phpHandlebarsSha256 ? null,
  phpHandlebarsAllTheTests ? false,

  mustache_spec ? pkgs.callPackage (import ((fetchTarball {
    url = https://github.com/jbboehr/mustache-spec/archive/5b85c1b58309e241a6f7c09fa57bd1c7b16fa9be.tar.gz;
    sha256 = "1h9zsnj4h8qdnzji5l9f9zmdy1nyxnf8by9869plyn7qlk71gdyv";
  }))) {},

  handlebars_spec ? pkgs.callPackage (import ((fetchTarball {
    url = https://github.com/jbboehr/handlebars-spec/archive/9306f3062b4b03d5179c33834bee7263a621639a.tar.gz;
    sha256 = "0gn9v88fxdf0wwgd8ix1xfxvif89mgfhxkp316arv5ljbibx2x9h";
  }))) {},

  handlebarsc ? pkgs.callPackage (import (fetchTarball {
    url = https://github.com/jbboehr/handlebars.c/archive/v0.7.3.tar.gz;
    sha256 = "03c9j5p172n3vabpq3gm4wn8f6kx5r3cnm5k7ad0nmhy5cahwci7";
  })) { inherit mustache_spec handlebars_spec; },

  php_psr ? pkgs.callPackage (import (fetchTarball {
    url = https://github.com/jbboehr/php-psr/archive/bafe9804c621cccdfc5cce3151563dd09caf73f4.tar.gz;
    sha256 = "1iarlg3qa4fk0s69wglahiazvi2kg58r5firngyxbcbsf5alk783";
  })) { inherit buildPecl; }
}:

pkgs.callPackage ./derivation.nix {
  inherit buildPecl mustache_spec handlebars_spec handlebarsc php_psr phpHandlebarsVersion phpHandlebarsSrc phpHandlebarsSha256 phpHandlebarsAllTheTests;
}

