<?php

class FakeImpl extends Handlebars\BaseImpl {
    public function render(string $tmpl, $context = null, array $options = null): string {}
    public function renderFile(string $filename, $context = null, array $options = null): string {}
}
