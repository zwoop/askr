# askr

A framework for searching in arbitrary log formats. What makes this useful is it's plugin design, together
with the embedded YAML configurations. This makes it easy and convenient to prepare your "scripts" suitable
for various jobs. A couple of key rules:

* A script can can have only one reader, which can possibly take multiple inputs / files
* A script can use any number of filter plugins, which can be controlled via CL options
* A script supports exactly one output module

All plugins, at all stages, functions as pipelines both within the processing stage, as well as between
stages. However, the order is always input plugin(s) -> filter plugin(s)-> output plugin(s).

The tldr; is that the script describes how to parse the input, which filters to process the lines, and
how to present the output.

## Example

```
#!/usr/bin/env askr
#
# The order of the sections does not matter, but evaluation always happens in a pipeline ordering:
#
#    input -> filter(s) -> output
#
# The options section is optional
#
options:
  # This scripts takes only one additional command line option, but there can be any number of them
  - name: selKeys
    description: "selects the keys for the result output"
    long: --select
    short: -s
input:
  # The input plugins, which reads the file(s) given
  - plugin: csv_reader.so
    source: files
    configs:
      col-separator:
        - "\t"
      keyval-separator:
        - "="
      rec-separator:
        - "\n"
filter:
  # Assures that lines from multiple files shows up in chronological order after filtering
  - plugin: ordered.so
    order:
      type: float
      key: time
  - plugin: pcre2.so
    configs:
      enable: expression
      logic: or
  - plugin: selector.so
    # Selects which keys to output, without this filter we would show all key=value pairs that passed.
    # This also dictates the ordering, such that the output is always consistent regardless of input.
    configs:
      option: selKeys
output:
  # Plugins that present the results (after filtering)
  - plugin: text.so
    configs:
      format: "$(key)=$(value)"
      col-separator: "\t"
      rec-separator: "\n"
```

## Command line options


All askr scripts takes the following command line options:

```
Query / filtering / output options (augmenting or overriding the script parameters)
  -e    Query expression, e.g. key1=val1
  -o    Output plugin to use, overriding the script default
  -O    Output plugin arguments (for -o plugin.so)

System level options
  -t    Number of threads (defaults to max one thread per core)
  -c    Number of CPU cores to use (defaults to all, no affinity)

```
## Dependencies


The following packages needs to be installed for the respective platforms:

* pcre2
* jemalloc

### macOS

The package dependencies are easiest installed using HomeBrew:
```
brew install pcre2 jemalloc
```

## Notes on Git subtree

I've included the following two Github projects as sub-tree's into this
repository:

* yaml-cpp
* GSL: Guidelines Support Library

The commands I ran to import these, and to maintain these, are as follow:
```
git remote add -f GSL https://github.com/Microsoft/GSL.git
git remote add -f yaml-cpp https://github.com/jbeder/yaml-cpp.git

git subtree add --prefix lib/gsl GSL master --squash
git subtree add --prefix lib/yaml-cpp yaml-cpp yaml-cpp-0.6.2  --squash
```
