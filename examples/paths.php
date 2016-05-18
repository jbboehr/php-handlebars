<?php

$tmpl = <<<EOF
<h1>Comments</h1>

<div id="comments">
    {{#each comments}}
    <h2><a href="/posts/{{../permalink}}#{{id}}">{{title}}</a></h2>
    <div>{{body}}</div>
    {{/each}}
</div>
EOF;

$data = array(
    'permalink' => '2014-08-08',
    'comments' => array(
        array(
            'id' => '1892',
            'title' => 'My title',
            'body' => 'Body',
        )
    )
);

$vm = new Handlebars\VM();
echo $vm->render($tmpl, $data);
