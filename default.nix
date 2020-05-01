# when using nix-shell, run "configurePhase" otherwise you'll have missing headers
# to use a specific version of php, run `nix-shell --arg php '(import <nixpkgs> {}).php73'`

{
  pkgs ? import <nixpkgs> {},
  php ? pkgs.php,
  buildPecl ? pkgs.callPackage <nixpkgs/pkgs/build-support/build-pecl.nix> {
    inherit php;
  },

  phpHandlebarsVersion ? "v0.9.1",
  phpHandlebarsSrc ? ./.,
  phpHandlebarsSha256 ? null,
  phpHandlebarsAllTheTests ? false,

  mustache_spec ? pkgs.callPackage (import ((fetchTarball {
    url = https://github.com/jbboehr/mustache-spec/archive/5b85c1b58309e241a6f7c09fa57bd1c7b16fa9be.tar.gz;
    sha256 = "1h9zsnj4h8qdnzji5l9f9zmdy1nyxnf8by9869plyn7qlk71gdyv";
  }))) {},

  handlebars_spec ? pkgs.callPackage (import ((fetchTarball {
    url = https://github.com/jbboehr/handlebars-spec/archive/v104.7.6.tar.gz;
    sha256 = "08dvx3s8j6i3npvh65halv18f5ilm0iisbrqxxv9gpfcav0m3hi6";
  }))) {},

  handlebarsc ? pkgs.callPackage (import (fetchTarball {
    url = https://github.com/jbboehr/handlebars.c/archive/7636786b281f127669c4fad565e8db0631d6164d.tar.gz;
    sha256 = "0krghnncabjg6xw62hvnkvnzyzgjki4vi0700lz2q1qzkb3d050y";
  })) { inherit mustache_spec handlebars_spec; },

  php_psr ? pkgs.callPackage (import (fetchTarball {
    url = https://github.com/jbboehr/php-psr/archive/v1.0.0.tar.gz;
    sha256 = "12237b392rz224r4d8p6pwnldpl2bfrvpcim5947avjd49sn8ss4";
  })) { inherit buildPecl; }
}:

pkgs.callPackage ./derivation.nix {
  inherit buildPecl mustache_spec handlebars_spec handlebarsc php_psr phpHandlebarsVersion phpHandlebarsSrc phpHandlebarsSha256 phpHandlebarsAllTheTests;
}

