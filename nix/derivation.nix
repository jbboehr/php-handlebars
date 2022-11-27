{
  lib, php, stdenv, autoreconfHook, fetchurl, talloc, pcre, pcre2,
  buildPecl, handlebarsc, gitignoreSource,
  valgrind ? null,
  php_psr ? null,
  handlebars_spec ? null,
  mustache_spec ? null,
  astSupport ? false,
  checkSupport ? true,
  debugSupport ? false,
  devSupport ? false,
  hardeningSupport ? true,
  psrSupport ? true,
  WerrorSupport ? (debugSupport || devSupport),
  valgrindSupport ? (debugSupport || devSupport)
}:

buildPecl rec {
  pname = "handlebars";
  name = "handlebars-${version}";
  version = "v1.0.0";

  src = lib.cleanSourceWith {
    filter = (path: type: (builtins.all (x: x != baseNameOf path)
        [".idea" ".git" ".github" "ci.nix" ".ci" "nix" "default.nix" "flake.nix" "flake.lock"]));
    src = gitignoreSource ../.;
  };

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
            ${php}/bin/php generate-tests.php
    '';

  doCheck = checkSupport;
  checkTarget = "test";
  checkFlags = ["REPORT_EXIT_STATUS=1" "NO_INTERACTION=1"];
}
