<?php

$tmpl = <<<EOF
<div class="post">
    <h1>By {{fullName author}}</h1>
    <div class="body">{{body}}</div>

    <h1>Comments</h1>

    {{#each comments}}
    <h2>By {{fullName author}}</h2>
    <div class="body">{{body}}</div>
    {{/each}}
</div>
EOF;

$data = array(
    'author' => array('firstName' => 'Alan', 'lastName' => 'Johnson'),
    'body' => 'I Love Handlebars',
    'comments' => array(
        array(
            'author' => array('firstName' => 'Yehuda', 'lastName' => 'Katz'),
            'body' => 'Me too!',
        ),
    ),
);

$helpers = new Handlebars\DefaultRegistry();
$helpers['fullName'] = function($person) {
    return $person['firstName'] . ' ' . $person['lastName'];
};

$vm = new Handlebars\VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $data);
