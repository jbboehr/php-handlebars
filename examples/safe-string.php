<?php

// From http://handlebarsjs.com/ - HTML Escaping

$tmpl = <<<EOF
<div>
    {{link text url}}
</div>
EOF;

$helpers = new Handlebars\DefaultRegistry();
$helpers['link'] = function($text, $url) {
    $text = htmlspecialchars($text);
    $url = htmlspecialchars($url);
    $result = '<a href="' . $url . '">' . $text . '</a>';
    return new Handlebars\SafeString($result);
};

$vm = new Handlebars\VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, array(
    'url' => "https://google.com",
    'text' => "Google",
));
