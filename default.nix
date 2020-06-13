# when using nix-shell, run "configurePhase" otherwise you'll have missing headers
# to use a specific version of php, run `nix-shell --arg php '(import <nixpkgs> {}).php73'`

{
  pkgs ? import <nixpkgs> {},
  php ? pkgs.php,
  buildPecl ? pkgs.callPackage <nixpkgs/pkgs/build-support/build-pecl.nix> {
    inherit php;
  },
  gitignoreSource ? (import (pkgs.fetchFromGitHub {
      owner = "hercules-ci";
      repo = "gitignore";
      rev = "00b237fb1813c48e20ee2021deb6f3f03843e9e4";
      sha256 = "sha256:186pvp1y5fid8mm8c7ycjzwzhv7i6s3hh33rbi05ggrs7r3as3yy";
  }) { inherit (pkgs) lib; }).gitignoreSource,

  mustache_spec ? pkgs.callPackage (import ((fetchTarball {
    url = https://github.com/jbboehr/mustache-spec/archive/5b85c1b58309e241a6f7c09fa57bd1c7b16fa9be.tar.gz;
    sha256 = "1h9zsnj4h8qdnzji5l9f9zmdy1nyxnf8by9869plyn7qlk71gdyv";
  }))) {},

  handlebars_spec ? pkgs.callPackage (import ((fetchTarball {
    url = https://github.com/jbboehr/handlebars-spec/archive/v104.7.6.tar.gz;
    sha256 = "0i2czm6yhiv5xbq93yj249xjxqrfv70mk1qgl0abkbm0qmmkc4vk";
  }))) {},

  handlebarsc ? pkgs.callPackage (import (fetchTarball {
    url = "https://github.com/jbboehr/handlebars.c/archive/f0a8e777bd2a127d20ce42b18b6e5fc3c428de41.tar.gz";
    sha256 = "00cxv0gqyy08j76z32pyb1kn7h1p98xzb42cks1qmnzmimwrmn5r";
  })) {
    inherit mustache_spec handlebars_spec;
    inherit debugSupport devSupport hardeningSupport staticSupport valgrindSupport WerrorSupport;
    sharedSupport = !staticSupport;
  },

  php_psr ? pkgs.callPackage (import (fetchTarball {
    url = https://github.com/jbboehr/php-psr/archive/v1.0.0.tar.gz;
    sha256 = "12237b392rz224r4d8p6pwnldpl2bfrvpcim5947avjd49sn8ss4";
  })) { inherit buildPecl; },

  phpHandlebarsVersion ? "v0.9.1",
  phpHandlebarsSha256 ? null,
  phpHandlebarsSrc ? pkgs.lib.cleanSourceWith {
    filter = (path: type: (builtins.all (x: x != baseNameOf path) [".idea" ".git" "ci.nix" ".travis.sh" ".travis.yml"]));
    src = gitignoreSource ./.;
  },

  astSupport ? false,
  checkSupport ? true,
  debugSupport ? false,
  devSupport ? false,
  hardeningSupport ? true,
  psrSupport ? true,
  WerrorSupport ? (debugSupport || devSupport),
  valgrindSupport ? (debugSupport || devSupport),
  staticSupport ? false
}:

pkgs.callPackage ./derivation.nix {
  inherit buildPecl handlebarsc php_psr phpHandlebarsVersion phpHandlebarsSrc phpHandlebarsSha256;
  inherit handlebars_spec mustache_spec;
  inherit astSupport checkSupport debugSupport devSupport hardeningSupport psrSupport valgrindSupport WerrorSupport;
}
