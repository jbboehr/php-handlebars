{
  lib, php, stdenv, autoreconfHook, fetchurl, talloc, pcre, pcre2,
  valgrind, # dev/debug deps
  handlebarsc, php_psr, handlebars_spec, mustache_spec, # my special stuff
  buildPecl ? import <nixpkgs/pkgs/build-support/build-pecl.nix> {
    # re2c is required for nixpkgs master, must not be specified for <= 19.03
    inherit php stdenv autoreconfHook fetchurl;
  },
  phpHandlebarsVersion ? null,
  phpHandlebarsSrc ? null,
  phpHandlebarsSha256 ? null,

  astSupport ? false,
  checkSupport ? true,
  debugSupport ? false,
  devSupport ? false,
  hardeningSupport ? true,
  psrSupport ? true,
  WerrorSupport ? (debugSupport || devSupport),
  valgrindSupport ? (debugSupport || devSupport)
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

  buildInputs = [ handlebarsc talloc pcre pcre2 ]
    ++ lib.optional  valgrindSupport valgrind
    ++ lib.optional  psrSupport php_psr
    ;

  nativeBuildInputs = lib.optionals checkSupport [ handlebars_spec mustache_spec ];

  configureFlags = []
    ++ lib.optional  astSupport "--enable-handlebars-ast"
    ++ lib.optional  (!astSupport) "--disable-handlebars-ast"
    ++ lib.optional  hardeningSupport "--enable-handlebars-hardening"
    ++ lib.optional  (!hardeningSupport) "--disable-handlebars-hardening"
    ++ lib.optional  psrSupport "--enable-handlebars-psr"
    ++ lib.optional  (!psrSupport) "--disable-handlebars-psr"
    ++ lib.optional  WerrorSupport "--enable-compile-warnings=error"
    ++ lib.optionals (!WerrorSupport) ["--enable-compile-warnings=yes" "--disable-Werror"]
    ;

  makeFlags = ["phpincludedir=$(out)/include/php/ext/handlebars"];

  preBuild = lib.optionalString checkSupport ''
        HANDLEBARS_SPEC_DIR="${handlebars_spec}/share/handlebars-spec" \
            MUSTACHE_SPEC_DIR="${mustache_spec}/share/mustache-spec" \
            php generate-tests.php
    '';

  doCheck = checkSupport;
  checkTarget = "test";
  checkFlags = ["REPORT_EXIT_STATUS=1" "NO_INTERACTION=1"];
}
