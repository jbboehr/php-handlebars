<?php

$tmpl = <<<EOF
<ul>
    {{#each items}}
    <li>{{agree_button}}</li>
    {{/each}}
</ul>
EOF;

$data = array(
    'items' => array(
        array('name' => 'Handlebars', 'emotion' => 'love'),
        array('name' => 'Mustache', 'emotion' => 'enjoy'),
        array('name' => 'Ember', 'emotion' => 'want to learn'),
    ),
);

$helpers = new Handlebars\DefaultRegistry();
$helpers['agree_button'] = function(Handlebars\Options $options) {
    $emotion = htmlspecialchars($options->scope['emotion']);
    $name = htmlspecialchars($options->scope['name']);
    $result = "<button>I agree. I " . $emotion . " " . $name . "</button>";
    return new Handlebars\SafeString($result);
};

$vm = new Handlebars\VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $data);
