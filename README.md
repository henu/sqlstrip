SqlStrip
========

Simple tool to strip SQL dumps from sensitive data.

Currently the tool has been tested with just few very simple MySQL dumps.


Building
========

```g++ src/*.cpp -o sqlstrip```


Usage
=====

The tool is based on simple code you can run to columns you choose.

Here is a simple example:

```mysqldump -uroot -p some_database | sqlstrip users.realname=random_string(8) users.email=random_email() usermessages=delete()```


Primitive types
===============

Primitive types that are supported:

- NULL
- numbers
- "strings"


Functions
=========

Here is a list of functions:

- delete() This is the only function is for whole tables. Use it to ignore content of tables.
- decimals(number, decimals)
- if(condition, value_if_true, value_if_false)
- random_number(min, max)
- random_string(min_len, max_len)
- random_words(min_words, max_words)
- random_email()
- random_phonenumber()
