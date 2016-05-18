<?php

$tmpl = <<<EOF
<div class="post">
    {{> userMessage tagName="h1" }}

    <h1>Comments</h1>

    {{#each comments}}
    {{> userMessage tagName="h2" }}
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

$partials = new Handlebars\DefaultRegistry();
$partials['userMessage'] = '<{{tagName}}>By {{author.firstName}} {{author.lastName}}</{{tagName}}>' . "\n"
    . '<div class="body">{{body}}</div>';

$vm = new Handlebars\VM();
$vm->setPartials($partials);
echo $vm->render($tmpl, $data);
