{ php, stdenv, autoreconfHook, fetchurl, handlebars, talloc, pcre, mustache_spec, handlebars_spec }:

let
  buildPecl = import <nixpkgs/pkgs/build-support/build-pecl.nix> {
    inherit php stdenv autoreconfHook fetchurl;
  };
in

buildPecl rec {
  name = "handlebars-${version}";
  version = "0.8.2RC2";
  sha256 = "1x5mbqflr953kwic9drmnlj16vd7x2c3pq5qa7y9wwa1g1sa7f6s";
  buildInputs = [ handlebars talloc pcre ];
  nativeBuildInputs = [ mustache_spec handlebars_spec ];
  doCheck = true;
  checkTarget = "test";
  checkFlagsArray = ["REPORT_EXIT_STATUS=1" "NO_INTERACTION=1" "TEST_PHP_DETAILED=1"];
}
