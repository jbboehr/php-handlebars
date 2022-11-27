# Changelog

All Notable changes to `php-handlebars` will be documented in this file.

Updates should follow the [Keep a CHANGELOG](http://keepachangelog.com/) principles.

## [1.0.0]

### Changed
- PHP 8.1 and 8.2 compat
- Target handlebars.js v4.7.7
- Target handlebars.c v1.0.0

### Fixed
- Objects can now be iterated with `#each`
- `known_helpers` is now passed properly to the compiler in `render`

## [0.9.2] - 2020-12-06

### Changed
- Target handlebars.c v0.7.3

### Fixed
- Fixes for PHP 8

## [0.9.1] - 2020-04-18

### Fixed
- Test failure on PHP 8 (@remicollet)
- Test failure on 32bit

### Changed
- The compiled template produced by `Handlebars\VM::compile()` should now be deterministic, if using handlebars.c >= 0.7.1.
- Target handlebars.c v0.7.1. The extension is fully backwards compatible to v0.6.4.

## [0.9.0] - 2020-04-17

### Added
- Preliminary PHP 8 support
- Support for precompiling templates. New methods `Handlebars\VM::compile()` and `Handlebars\VM::renderFromBinaryString()`.
- Mustache-style lambda support behind a new compiler flag `mustacheStyleLambdas`. Please note that due to limitations in
the parser, they will make (functionally compatible) transformations to the template passed to the lambda and may not work
with some handlebars syntax.

### Changed
- Drop max PHP version constraint from PECL
- Selective helper options omission - If the argument is a closure: we will omit passing the options
object if it will go into a slot with an incompatible type. If the argument is a callable object: we will append the
options if the appended parameter will go into a declared parameter with either no type signature or explicitly
typed with `Handlebars\Options`. This will allow some standard helpers to have typed default arguments while still allowing
access to the Options object for other helpers. See `tests/helper-with-options.phpt` for details.
- `Handlebars\ParseException` is deprecated and now an alias of `Handlebars\CompileException`
- Improved typehints
- Target handlebars.c v0.7.0. The extension is fully backwards compatible with v0.6.4.

### Removed
- PHP 5 support, require at least PHP 7.2

## [0.8.3] - 2018-11-11

### Added
- Allow installation on PHP 7.3

### Removed
- Class aliases `Handlebars\Registry\Registry` for `Handlebars\Registry` and `Handlebars\Registry\DefaultRegistry` for `Handlebars\DefaultRegistry`

## [0.8.2] - 2018-02-22

### Added
- Mustache delimiter preprocessing support behind the compat flag
- Property lookup now generates a closure from a method if the value of the property is null or does not exist to emulate the behaviour of javascript where functions and properties share a symbol table - only supported for PHP 7 or greater
- Windows support

### Changed
- Changed target handlebars.c version to 0.6.4

## [0.8.1] - 2016-11-08

### Fixed
- SIGABORT on invalid helper type
- Segfault during callable object call, caused by uninitialized value from [0d38317](https://github.com/jbboehr/php-handlebars/commit/0d38317b983cf7411adc9d93f2f43e0ecab69642u)

## [0.8.0] - 2016-11-07

### Added
- `strict` and `assumeObjects` flag support
- `handlebars_cache_reset()` to completely empty the cache

### Changed
- Target handlebars.c version to 0.6.0

## [0.7.4] - 2016-09-21

### Added
- [Examples](examples)

### Fixed
- Compilation fails on PHP 7.1 ([#35](https://github.com/jbboehr/php-handlebars/pull/35))

## [0.7.3] - 2016-05-05

### Changed
- Changed the license from `LGPLv3` to `Simplified BSD License`
- Changed target handlebars.c version to 0.5.1
- Changed target php-psr version to 0.2.3

## [0.7.2] - 2016-04-24

### Fixed
- Link failure with inline attribute on `php_handlebars_is_int_array` and `php_handlebars_is_callable` using GCC 5.3 on Ubuntu 16.04 LTS


## [0.7.1] - 2016-04-15

### Fixed
- Segfault in phpinfo when cache is disabled

## [0.7.0] - 2016-04-13

### Added
- New handlebars VM


[Unreleased]: https://github.com/jbboehr/php-handlebars/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/jbboehr/php-handlebars/compare/v0.9.2...v1.0.0
[0.9.2]: https://github.com/jbboehr/php-handlebars/compare/v0.9.1...v0.9.2
[0.9.1]: https://github.com/jbboehr/php-handlebars/compare/v0.9.0...v0.9.1
[0.9.0]: https://github.com/jbboehr/php-handlebars/compare/v0.8.3...v0.9.0
[0.8.3]: https://github.com/jbboehr/php-handlebars/compare/v0.8.2...v0.8.3
[0.8.2]: https://github.com/jbboehr/php-handlebars/compare/v0.8.1...v0.8.2
[0.8.1]: https://github.com/jbboehr/php-handlebars/compare/v0.8.0...v0.8.1
[0.8.0]: https://github.com/jbboehr/php-handlebars/compare/v0.7.4...v0.8.0
[0.7.4]: https://github.com/jbboehr/php-handlebars/compare/v0.7.3...v0.7.4
[0.7.3]: https://github.com/jbboehr/php-handlebars/compare/v0.7.2...v0.7.3
[0.7.2]: https://github.com/jbboehr/php-handlebars/compare/v0.7.1...v0.7.2
[0.7.1]: https://github.com/jbboehr/php-handlebars/compare/v0.7.0...v0.7.1
[0.7.0]: https://github.com/jbboehr/php-handlebars/compare/v0.6.1...v0.7.1

