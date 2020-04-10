<?php

$tmpl = <<<EOF
<div class="entry">
    <h1>{{title}}</h1>
    <div class="body">
        {{body}}
    </div>
</div>
EOF;

$data = array(
    'title' => 'My New Post',
    'body' => 'This is my first post!',
);

$vm = new Handlebars\VM();

$iterations = 100000;

// standard
$start = microtime(true);
for ($i = 0; $i < $iterations; $i++ ) {
    $vm->render($tmpl, $data);
}
$end = microtime(true);
$delta = $end - $start;

printf("%-12s iterations=%d cacheBackend=%-6s time=%g\n", "standard", $iterations, Handlebars\CACHE_BACKEND, $delta);

// precompiled
$start = microtime(true);
$binaryString = $vm->compile($tmpl);
for ($i = 0; $i < $iterations; $i++ ) {
    $vm->renderFromBinaryString($binaryString, $data);
}
$end = microtime(true);
$delta = $end - $start;

printf("%-12s iterations=%d cacheBackend=%-6s time=%g\n", "precompiled", $iterations, Handlebars\CACHE_BACKEND, $delta);
