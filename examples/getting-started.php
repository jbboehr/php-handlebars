<?php

// From http://handlebarsjs.com/ - Getting Started

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
echo $vm->render($tmpl, $data);
