<?php

// From http://handlebarsjs.com/ - Block Expressions

$tmpl = <<<EOF
{{#list people}}{{firstName}} {{lastName}}{{/list}}
EOF;
$data = array(
    'people' => array(
        array('firstName' => 'Yehuda', 'lastName' => 'Katz'),
        array('firstName' => 'Carl', 'lastName' => 'Lerche'),
        array('firstName' => 'Alan', 'lastName' => 'Johnson'),
    )
);

$helpers = new Handlebars\DefaultRegistry();
$helpers['list'] = function($items, Handlebars\Options $options) {
    $out = '<ul>';
    foreach( $items as $item ) {
        $out .= '<li>' . $options->fn($item) . '</li>';
    }
    return $out . '</ul>';
};

$vm = new Handlebars\VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $data);
