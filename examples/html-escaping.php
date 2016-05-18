<?php

// From http://handlebarsjs.com/ - HTML Escaping

$tmpl = <<<EOF
<div class="entry">
    <h1>{{title}}</h1>
    <div class="body">
        {{{body}}}
    </div>
</div>
EOF;

$vm = new Handlebars\VM();
echo $vm->render($tmpl, array(
    'title' => "All about <p> Tags",
    'body' => '<p>This is a post about &lt;p&gt; tags</p>',
));
