# when using nix-shell, run "configurePhase" otherwise you'll have missing headers
# to use a specific version of php, run `nix-shell --arg php '(import <nixpkgs> {}).php73'`
# or clang: nix-shell --arg stdenv '(import <nixpkgs> {}).clangStdenv'
{
  pkgs ? import <nixpkgs> {},
  stdenv ? pkgs.stdenv,
  php ? pkgs.php,
  buildPecl ? if builtins.hasAttr "buildPecl" php then php.buildPecl else pkgs.callPackage <nixpkgs/pkgs/build-support/build-pecl.nix> {
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
    url = "https://github.com/jbboehr/handlebars.c/archive/7acdab048657196d42b4b663bc493775b4134aaf.tar.gz";
    sha256 = "04fzlx84x0ljcbpw5v4y46wkx44p6rbkl6adnq0pmh5yg3cqylpv";
  })) {
    inherit stdenv;
    inherit mustache_spec handlebars_spec;
    inherit debugSupport devSupport hardeningSupport ltoSupport valgrindSupport WerrorSupport;
    sharedSupport = (!staticSupport && !ltoSupport);
    staticSupport = (ltoSupport || staticSupport);
  },

  php_psr ? pkgs.callPackage (import (fetchTarball {
    url = https://github.com/jbboehr/php-psr/archive/bafe9804c621cccdfc5cce3151563dd09caf73f4.tar.gz;
    sha256 = "1iarlg3qa4fk0s69wglahiazvi2kg58r5firngyxbcbsf5alk783";
  })) { inherit buildPecl stdenv; },

  phpHandlebarsVersion ? "v0.9.2",
  phpHandlebarsSha256 ? null,
  phpHandlebarsSrc ? pkgs.lib.cleanSourceWith {
    filter = (path: type: (builtins.all (x: x != baseNameOf path) [".idea" ".git" "ci.nix" ".travis.sh" ".travis.yml" ".ci"]));
    src = gitignoreSource ./.;
  },

  astSupport ? false,
  checkSupport ? true,
  debugSupport ? false,
  devSupport ? false,
  ltoSupport ? false,
  hardeningSupport ? true,
  psrSupport ? true,
  WerrorSupport ? (debugSupport || devSupport),
  valgrindSupport ? (debugSupport || devSupport),
  staticSupport ? false
}:

pkgs.callPackage ./nix/derivation.nix {
  inherit stdenv buildPecl handlebarsc php_psr phpHandlebarsVersion phpHandlebarsSrc phpHandlebarsSha256;
  inherit handlebars_spec mustache_spec;
  inherit astSupport checkSupport debugSupport devSupport hardeningSupport psrSupport valgrindSupport WerrorSupport;
}
