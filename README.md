[![Build Status](https://travis-ci.org/Rant-and-Dev/maestro.svg?branch=master)](https://travis-ci.org/Rant-and-Dev/maestro)

# maestro
Maestro - A simple provisioning tool using SSH, hopefully POSIX compliant.

## Goal
The goal with Maestro is to have all the advantages of the hype provisionning tools such as puppet,
chef, salt, ansible, or others, without the downsides of those tools (which usually require an
agent, etc.). There is, also, an alternative that does exactly what Maestro aims at, but which [is
written in Perl](http://rexify.org/), and thus is missing the point of being simple (I tried to use
it instead of maestro, and came back to developing maestro after a few hours of trial & error).

## Design
The design of maestro is splitted in different binaries, each having a different purpose:

### maestro-spawn
1. `maestro-spawn` reads a description from the standard input, JSON formatted.
2. `maestro-spawn` executes one of its modules, depending on what provider is declared.
3. `maestro-spawn` executes the configuration and spawns the new infrastructure.

### maestro-apply
1. `maestro-apply` executes a binary passed as a parameter (argv[1..])
2. `maestro-apply` uses the file decriptors from that binary execution (`std*`) to communicate with
   the binary
3. `maestro-apply` then reads a description from the standard input, JSON formatted
4. For each item of this description, `maestro-apply` executes one of its modules, expecting a
   POSIX interface from the first point, and probing for non-posix interfaces as needed.

### maestro-translate
1. `maestro-translate` reads a JSON template description from the standard input
2. `maestro-translate` uses the environment (and optionally flags) to substitute the
   template-elements with correct data
3. `maestro-translate` outputs the completed JSON description to the standard output

### maestro-combine
1. `maestro-combine` reads all the files which names are passed in arguments, and dump
   them combined in a valid JSON document to the output.

### maestro-state
1. `maestro-state` reads a json file from stdin containing the result of an execution.
2. `maestro-state` queries the data-structure obtained from that file according to the parameters
   passed to it at invocation time.
3. `maestro-state` outputs the result of the query, in json.

## Examples
Let's assume we want to create a user on a newly created server, on which we can already log in as
root, via ssh, authenticated via an rsa key:

    $ echo '{"user": {"name": "newuser", "passwd": null, "sudo": true}, "sshd": {"root": "nologin"}}' | maestro-apply root@host1.domain.tld

will create a new user on the host1.domain.tld, with a randomly generated password (locally) that
will be displayed on `stdout`, will allow that user to sudo, and will then disallow the root user
to login via sshd.

Alternatively, we might want to create a user with our current user instead of newuser:

    $ echo '{"user": {"name": $USER, "passwd": null, "sudo": true}, "sshd": {"root": "nologin"}}' | maestro-translate | maestro-apply root@host1.domain.tld

Finally:

    $ maestro-apply root@host1.domain.tld < host1.domain.tld.json

will provision host1.domain.tld according to the description contained in host1.domain.tld.json.

And:

    $ maestro-translate < host1.domain.tld.json | maestro-apply root@host1.domain.tld

will provision host1.domain.tld according to the interpreted description contained in
`host1.domain.tld.json`.
