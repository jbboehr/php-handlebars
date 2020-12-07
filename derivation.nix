{ lib, php, stdenv, autoreconfHook, fetchurl, handlebarsc, talloc, pcre, pcre2, mustache_spec, handlebars_spec, php_psr,
  buildPecl ?  if builtins.hasAttr "buildPecl" php then php.buildPecl else import <nixpkgs/pkgs/build-support/build-pecl.nix> {
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
  version = orDefault phpHandlebarsVersion "v0.9.0";
  src = orDefault phpHandlebarsSrc (fetchurl {
    url = "https://github.com/jbboehr/php-handlebars/archive/${version}.tar.gz";
    sha256 = orDefault phpHandlebarsSha256 "16k182cpz73hpm6jshh6bl4sma5z9l7b98wh43r95h7m09azy6hl";
  });

  buildInputs = [ handlebarsc talloc pcre pcre2 php_psr ];
  nativeBuildInputs = [ mustache_spec handlebars_spec ];
  makeFlags = ["phpincludedir=$(out)/include/php/ext/handlebars"];

  postBuild = lib.optionalString phpHandlebarsAllTheTests ''
      MUSTACHE_SPEC_DIR=${mustache_spec}/share/mustache-spec HANDLEBARS_SPEC_DIR=${handlebars_spec}/share/handlebars-spec php generate-tests.php
    '';

  doCheck = true;
  checkTarget = "test";
  checkFlags = ["REPORT_EXIT_STATUS=1" "NO_INTERACTION=1"];
}

