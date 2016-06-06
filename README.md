SqlStrip
========

Simple tool to strip SQL dumps from sensitive data.

Currently the tool has been tested with just few very simple MySQL dumps.


Building
========

```g++ src/*.cpp -o sqlstrip```


Usage
=====

```mysqldump -uroot -p some_database | sqlstrip users.realname=random_string(8) users.another_sensitive_column=empty_string```


Rules
=====

There are currently two rules available:

* empty_string: Converts value to empty string.
* random_string(length): Converts value to random string with specific length. a-z, A-Z and 0-9 will be used when selecting characters.
