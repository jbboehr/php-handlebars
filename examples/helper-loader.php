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

class FullNameHelper {
    public function __invoke($person) {
        return $person['firstName'] . ' ' . $person['lastName'];
    }
}

class HelperLoader implements IteratorAggregate, Handlebars\Registry {
    private $helpers = array();
    public function offsetGet($offset) {
        if( isset($helpers[$offset]) ) {
            return $helpers[$offset];
        }
        $className = ucfirst($offset) . 'Helper'; // Transform helper name to class
        if( class_exists($className, true) ) {
            return $helpers[$offset] = new $className;
        }
    }
    public function offsetSet($offset, $value) {
        $this->helpers[$offset] = $value;
    }
    public function offsetExists($offset) {
        return null !== $this->offsetGet($offset);
    }
    public function offsetUnset($offset) {
        unset($this->helpers[$offset]);
    }
    public function getIterator() {
        return new ArrayIterator($this->helpers);
    }
}

$vm = new Handlebars\VM();
$vm->setHelpers(new HelperLoader());
echo $vm->render($tmpl, $data);
