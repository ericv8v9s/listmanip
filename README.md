A simple tool to use a text file as a list/stack/queue.
Each line in a file is treated as an entry.

Example:

```shell
$ cat example.list
first
second
third
$ listmanip example.list add 'one thing' 'another thing'
$ cat example.list
first
second
third
one thing
another thing
$ listmanip example.list pop 2
first
second
$ cat example.list
third
one thing
another thing
```
