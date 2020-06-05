{ lib, php, stdenv, autoreconfHook, fetchurl, handlebarsc, talloc, pcre, pcre2, mustache_spec, handlebars_spec, php_psr,
  buildPecl ? import <nixpkgs/pkgs/build-support/build-pecl.nix> {
    # re2c is required for nixpkgs master, must not be specified for <= 19.03
    inherit php stdenv autoreconfHook fetchurl;
  },
  phpHandlebarsVersion ? null,
  phpHandlebarsSrc ? null,
  phpHandlebarsSha256 ? null,
  phpHandlebarsAllTheTests ? false,

  astSupport ? false,
  hardeningSupport ? true
}:

let
  orDefault = x: y: (if (!isNull x) then x else y);
in

buildPecl rec {
  pname = "handlebars";
  name = "handlebars-${version}";
  version = orDefault phpHandlebarsVersion "v0.9.1";
  src = orDefault phpHandlebarsSrc (fetchurl {
    url = "https://github.com/jbboehr/php-handlebars/archive/${version}.tar.gz";
    sha256 = orDefault phpHandlebarsSha256 "0s82gp9l6d63wjv0f5x7pb4q0iw0fiig2cis35ag2sbbk7lrgrjv";
  });

  buildInputs = [ handlebarsc talloc pcre pcre2 php_psr ];
  nativeBuildInputs = [ mustache_spec handlebars_spec ];

  configureFlags = []
    ++ lib.optional  astSupport "--enable-handlebars-ast"
    ++ lib.optional  (!astSupport) "--disable-handlebars-ast"
    ++ lib.optional  hardeningSupport "--enable-handlebars-hardening"
    ++ lib.optional  (!hardeningSupport) "--disable-handlebars-hardening"
    ;

  makeFlags = ["phpincludedir=$(out)/include/php/ext/handlebars"];

  postBuild = lib.optionalString phpHandlebarsAllTheTests ''
      MUSTACHE_SPEC_DIR=${mustache_spec}/share/mustache-spec HANDLEBARS_SPEC_DIR=${handlebars_spec}/share/handlebars-spec php generate-tests.php
    '';

  doCheck = true;
  checkTarget = "test";
  checkFlags = ["REPORT_EXIT_STATUS=1" "NO_INTERACTION=1"];
}

