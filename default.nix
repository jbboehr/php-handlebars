# when using nix-shell, run "configurePhase" otherwise you'll have missing headers
# to use a specific version of php, run `nix-shell --arg php '(import <nixpkgs> {}).php56'`

{
  pkgs ? import <nixpkgs> {},
  php ? pkgs.php,
  buildPecl ? pkgs.callPackage <nixpkgs/pkgs/build-support/build-pecl.nix> {
    inherit php;
  },

  phpHandlebarsVersion ? null,
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
    url = https://github.com/jbboehr/handlebars.c/archive/57598c75f9b343e1af7608fa333821a1bca264eb.tar.gz;
    sha256 = "0rwyhn562idr7swsccb94x5cbna0rf9p6wzqx58ll6d3k1wp7k0w";
  })) { inherit mustache_spec handlebars_spec; },

  php_psr ? pkgs.callPackage (import (fetchTarball {
    url = https://github.com/jbboehr/php-psr/archive/v1.0.0.tar.gz;
    sha256 = "12237b392rz224r4d8p6pwnldpl2bfrvpcim5947avjd49sn8ss4";
  })) { inherit buildPecl; }
}:

pkgs.callPackage ./derivation.nix {
  inherit buildPecl mustache_spec handlebars_spec handlebarsc php_psr phpHandlebarsVersion phpHandlebarsSrc phpHandlebarsSha256 phpHandlebarsAllTheTests;
}

