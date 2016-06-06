SqlStrip
========

Simple tool to strip SQL dumps from sensitive data.

Currently the tool has been tested with just one very simple MySQL dump.
The only available stripping method is to convert values in some
specific columns to empty strings, but more is coming in the future.


Building
========

```g++ src/*.cpp -o sqlstrip```


Usage
=====

```mysqldump -uroot -p some_database | sqlstrip users.realname=empty_string users.another_sensitive_column=empty_string```
