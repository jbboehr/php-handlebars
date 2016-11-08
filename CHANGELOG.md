# Changelog

All Notable changes to `php-handlebars` will be documented in this file.

Updates should follow the [Keep a CHANGELOG](http://keepachangelog.com/) principles.

## [Unreleased]

### Fixed
- SIGABORT on invalid helper type

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


[Unreleased]: https://github.com/jbboehr/php-handlebars/compare/v0.8.0...HEAD
[0.8.0]: https://github.com/jbboehr/php-handlebars/compare/v0.7.4...v0.8.0
[0.7.4]: https://github.com/jbboehr/php-handlebars/compare/v0.7.3...v0.7.4
[0.7.3]: https://github.com/jbboehr/php-handlebars/compare/v0.7.2...v0.7.3
[0.7.2]: https://github.com/jbboehr/php-handlebars/compare/v0.7.1...v0.7.2
[0.7.1]: https://github.com/jbboehr/php-handlebars/compare/v0.7.0...v0.7.1
[0.7.0]: https://github.com/jbboehr/php-handlebars/compare/v0.6.1...v0.7.1

