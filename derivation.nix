{ lib, php, stdenv, autoreconfHook, fetchurl, handlebarsc, talloc, pcre, pcre2, mustache_spec, handlebars_spec, php_psr,
  buildPecl ? import <nixpkgs/pkgs/build-support/build-pecl.nix> {
    # re2c is required for nixpkgs master, must not be specified for <= 19.03
    inherit php stdenv autoreconfHook fetchurl;
  },
  phpHandlebarsVersion ? null,
  phpHandlebarsSrc ? null,
  phpHandlebarsSha256 ? null,
  phpHandlebarsAllTheTests ? false }:

let
  orDefault = x: y: (if (!isNull x) then x else y);
in

buildPecl rec {
  pname = "handlebars";
  name = "handlebars-${version}";
  version = orDefault phpHandlebarsVersion "v0.8.3";
  src = orDefault phpHandlebarsSrc (fetchurl {
    url = "https://github.com/jbboehr/php-handlebars/archive/${version}.tar.gz";
    sha256 = orDefault phpHandlebarsSha256 "1w054335fzz8xs1kxacczsfcyswrs6bjjfm2ma4l9mdqnxdspjzg";
  });

  buildInputs = [ handlebarsc talloc pcre pcre2 php_psr ];
  nativeBuildInputs = [ mustache_spec handlebars_spec ];
  makeFlags = ["phpincludedir=$(out)/include/php/ext/psr"];

  postBuild = lib.optionalString phpHandlebarsAllTheTests ''
      MUSTACHE_SPEC_DIR=${mustache_spec}/share/mustache-spec HANDLEBARS_SPEC_DIR=${handlebars_spec}/share/handlebars-spec php generate-tests.php
    '';

  doCheck = true;
  checkTarget = "test";
  checkFlagsArray = ["REPORT_EXIT_STATUS=1" "NO_INTERACTION=1" "TEST_PHP_DETAILED=1"];
}

