# when using nix-shell, run "configurePhase" otherwise you'll have missing headers
# to use a specific version of php, run `nix-shell --arg php '(import <nixpkgs> {}).php73'`
# or clang: nix-shell --arg stdenv '(import <nixpkgs> {}).clangStdenv'
{ pkgs ? import <nixpkgs> { }
, stdenv ? pkgs.stdenv
, php ? pkgs.php
, buildPecl ? php.buildPecl
, gitignoreSource ? (import
    (pkgs.fetchFromGitHub {
      owner = "hercules-ci";
      repo = "gitignore";
      rev = "00b237fb1813c48e20ee2021deb6f3f03843e9e4";
      sha256 = "sha256:186pvp1y5fid8mm8c7ycjzwzhv7i6s3hh33rbi05ggrs7r3as3yy";
    })
    { inherit (pkgs) lib; }).gitignoreSource
, mustache_spec ? pkgs.callPackage
    (import (
      (fetchTarball {
        url = "https://github.com/jbboehr/mustache-spec/archive/18cca097c306e8cbfd9f6a30f86a52f4c1d219e4.tar.gz";
        sha256 = "0xg9x2adbvp565imza0hg622qm2hdy0w3z944krv1f3bjs7332wi";
      })
    ))
    { }
, handlebars_spec ? pkgs.callPackage
    (import (
      (fetchTarball {
        url = "https://github.com/jbboehr/handlebars-spec/archive/289859aba16bc9e63d5b05ce176b777cb67700dc.tar.gz";
        sha256 = "1rsapj9d37srynvjlg89ww9lyci1bv9ysi0baqqgj45sp87fw6yc";
      })
    ))
    { }
, handlebarsc ? pkgs.callPackage
    (import (fetchTarball {
      url = "https://github.com/jbboehr/handlebars.c/archive/v1.0.0.tar.gz";
      sha256 = "027f1h65nam3wpvd1qfah4j9m21q299wwya2m13f0fwm5qxbd0s3";
    }))
    {
      inherit stdenv;
      inherit mustache_spec handlebars_spec;
      inherit debugSupport devSupport hardeningSupport ltoSupport valgrindSupport WerrorSupport;
      sharedSupport = (!staticSupport && !ltoSupport);
      staticSupport = (ltoSupport || staticSupport);
    }
, php_psr ? pkgs.callPackage
    (import (fetchTarball {
      url = "https://github.com/jbboehr/php-psr/archive/a46f438d0e2669ad56ffe859650ef0be10fc5e91.tar.gz";
      sha256 = "0q18038qyqn6rzdyljhxla3qmixaspbwi6981q929qnp2bx69ggx";
    }))
    { inherit buildPecl stdenv php; }
, astSupport ? false
, checkSupport ? true
, debugSupport ? false
, devSupport ? false
, ltoSupport ? false
, hardeningSupport ? true
, psrSupport ? true
, WerrorSupport ? (debugSupport || devSupport)
, valgrindSupport ? (debugSupport || devSupport)
, staticSupport ? false
}:

pkgs.callPackage ./nix/derivation.nix {
  inherit stdenv php buildPecl gitignoreSource handlebars_spec mustache_spec handlebarsc php_psr;
  inherit astSupport checkSupport debugSupport devSupport hardeningSupport psrSupport valgrindSupport WerrorSupport;
}
