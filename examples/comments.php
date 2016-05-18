<?php

$tmpl = <<<EOF
<div class="entry">
    {{! This comment will not be in the output }}
    <!-- This comment will be in the output -->
</div>
EOF;

$vm = new Handlebars\VM();
echo $vm->render($tmpl);
