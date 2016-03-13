--TEST--
Handlebars phpinfo
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
phpinfo(INFO_MODULES);
--EXPECTF--
%A
handlebars
%A
Version => %s
Released => %s
Authors => %s
Spec Version => %s
libhandlebars Version => %s
libhandlebars Handlebars Spec Version => %s
libhandlebars Mustache Spec Version => %s
Memory usage => %s
Cache entries => %s
Cache size => %s
%A
Directive => Local Value => Master Value
handlebars.cache.enabled => %s => %s
handlebars.cache.max_age => %s => %s
handlebars.cache.max_entries => %s => %s
handlebars.cache.max_size => %s => %s
handlebars.pool_size => %s => %s
%A
