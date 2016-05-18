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

class PartialLoader implements IteratorAggregate, Handlebars\Registry {
    private $impl;
    private $partials = array();
    public function __construct(Handlebars\Impl $impl) {
        $this->impl = $impl;
    }
    public function offsetGet($offset) {
        if( isset($partials[$offset]) ) {
            return $partials[$offset];
        }
        // Transform partial name to file
        $partialFile = $offset . '.hbs';
        $impl = $this->impl;
        // The partial's opcodes will be cached if opcode caching is enabled
        // in the extension
        return $partials[$offset] = function($context = null) use ($impl, $partialFile) {
            return $impl->renderFile(__DIR__ . '/' . $partialFile, $context);
        };
    }
    public function offsetSet($offset, $value) {
        $this->partials[$offset] = $value;
    }
    public function offsetExists($offset) {
        return null !== $this->offsetGet($offset);
    }
    public function offsetUnset($offset) {
        unset($this->partials[$offset]);
    }
    public function getIterator() {
        return new ArrayIterator($this->partials);
    }
}


$vm = new Handlebars\VM();
$vm->setPartials(new PartialLoader($vm));
$output = $vm->render($tmpl, $data);
echo $output;
