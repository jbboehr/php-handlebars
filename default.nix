# when using nix-shell, run "configurePhase" otherwise you'll have missing headers
# to use a specific version of php, run `nix-shell --arg php '(import <nixpkgs> {}).php56'`

{
  pkgs ? import <nixpkgs> {},
  php ? pkgs.php,

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
    url = https://github.com/jbboehr/handlebars.c/archive/444272ab9503c5da7c06419ff38d61db70cf5b25.tar.gz;
    sha256 = "01mg8rkl67c3hw82l0ijdsn9kk0cq6vmd510ljy5ljdph1q2b8wm";
  })) { inherit mustache_spec handlebars_spec; },

  php_psr ? pkgs.callPackage (import (fetchTarball {
    url = https://github.com/jbboehr/php-psr/archive/v0.6.0.tar.gz;
    sha256 = "1zslwrjpzsvvnr5igy0j6z7hgjvrflzk7j20v6w0pc2v124g5ykk";
  })) {}
}:

pkgs.callPackage ./derivation.nix {
  inherit mustache_spec handlebars_spec handlebarsc php_psr phpHandlebarsVersion phpHandlebarsSrc phpHandlebarsSha256 phpHandlebarsAllTheTests;
}

