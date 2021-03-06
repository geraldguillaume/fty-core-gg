= The tools/builder.sh script
:Author:        Evgeny Klimov
:Email:         EvgenyKlimov@eaton.com

The project includes a `tools/builder.sh` script whose main purpose
is to automate the repetitive standard GNU automake procedure of
`./autogen.sh && ./configure && make && make install` with further
optimizations to run a parallelized build when possible, into a
single script with several short-named methods to quickly run the
frequently needed building scenarios.

Calls to the `./configure` script can be customized with any of its
usual tweaking command-line parameters, using either the 'configure'
or 'configure-subdir' methods (with all flags passed verbatim to the
`configure` script) followed by execution of the 'make*' methods,
or by use of the 'CONFIGURE_FLAGS' enccar or '--configure-flags'
option. See below for more details on usage of these possibilities.
Any environment variables which influence its progress and results
(like 'CFLAGS') should quite pass through from the caller's shell, too.

As part of development and maintenance of the 'Makefile' and related
build files, it occasionally happens that something happens differently
if the build is executed "in-place" from the project's root directory,
or "relocated" from another directory, or kicked around with the evil
`make dist` or `make distcheck` logic. To simplify verification that
everything behaves consistently in these cases, the `builder.sh` also
automates these three build modes.

This script can be executed explicitly or via our `autogen.sh` by
passing any command-line parameters to it -- in that case `autogen.sh`
completes its job of making sure that the `configure` script exists
and is up to date, and then falls through to execute the `builder.sh`.
Conversely, if `builder.sh` is called by itself, it first executes
`autogen.sh` to make sure the `configure` script is up to date, and
then proceeds with its own application logic to automate the build
method requested by the user.

Calling either `./autogen.sh` or `./tools/builder.sh` without any
command-line parameters should do just the default autogen job --
make sure that the `configure` script exists and is up to date, and
quit with success (or fail otherwise).


== Command-line options
Currently these optional modifier are supported on the command-line:

 * '--warnless-unused' -- this suppresses the compiler warnings about
unused variables (see details below in the 'WARNLESS_UNUSED' envvar
description)
 * '--warn-fatal' or '-Werror' -- this sets up the compiler to fail when
it has warnings to report, allowing easier tracing and recompilation of
not-yet-perfect pieces of source code
 * '--build-subdir' -- relative (to source code root) or absolute path
for a relocated build directory (used if the '*-subdir' methods are called);
this overrides the 'BUILD_SUBDIR' environment variable or automatic value
 * '--install-dir' -- this overrides the 'DESTDIR' environment variable
or automatic value for the 'install*' actions
 * '--configure-flags "--flag1=a --flag2=b"' -- (re)sets '$CONFIGURE_FLAGS'
to the single-token parameter with values that would be passed to `configure`
as its set of command-line parameters, should it be invoked in this run
 * '--nodistclean' or '--disable-distclean' -- this sets 'NODISTCLEAN=yes'
(see below) to skip the `make distclean` activity otherwise typical for
the 'conf*' and 'build*' actions
 * '--noparmake' or '--disable-parallel-make' -- this sets 'NOPARMAKE=yes'
(see below) for this invokation of the script i.e. to override the current
environment variable
 * '--noseqmake' or '--disable-sequential-make' -- this sets 'NOSEQMAKE=yes'
(see below) for this invokation of the script i.e. to override the current
environment variable
 * '--parmake' or '--enable-parallel-make' -- this sets 'NOPARMAKE=no'
to enable parallel makes (enabled by default unless envvars forbid)
 * '--seqmake' or '--enable-sequential-make' -- this sets 'NOSEQMAKE=no'
to enable sequential makes (enabled by default unless envvars forbid)
 * '--optseqmake' or '--optional-sequential-make' -- this sets 'OPTSEQMAKE'
to the next provided value ('yes', 'no', 'auto') or to 'yes' if no value
was provided. The effect of 'yes' is to skip the sequential build phase
iff the parallel build phase succeeded, which may be acceptable for certain
targets ('auto' changes into a 'yes' if the selected actions or targets
match a predefined pattern).
 * '--parmake-la-limit' -- this sets 'PARMAKE_LA_LIMIT' to the next provided
value that is expected to be a positive integer or floating-point number and
defines a limit of OS load average where parallel gmake would stop spawning
jobs on this build host (default: 4.0)
 * '--show-builder-flags' -- before executing an action, display the
setup of `builder.sh` for this run
 * '--show-repository-metadata' -- before executing an action, display
the last commit of the source code (and differences after it, if any)
 * '--show-time-make' -- measure how long each `make` call took
 * '--show-time-conf' -- measure how long each `configure` call took
 * '--show-timing' -- measure the times above
 * '--verbose' flag is reserved for verbosity settings; currently it
just enables the '--show-builder-flags', '--show-repository-metadata'
and '--show-timing' activities
 * '--debug-makefile' -- sets a number of verbosity preferences to ease
debugging of 'Makefile' development with just one option to set (turns
the parallel pass into a sequential `make` of lowered verbosity, so it
is well seen where the errors originate from, and follows up by a usual
sequential pass).

The first command-line attribute that is not an option defined above
is considered to be the request of a command-line method as described
below.


== Command-line methods
The currently defined following methods (selected by first attribute
on the command-line) include:

 * (no attributes) Recreate the `configure` script if needed, and exit
 * 'help' -- display short usage notice on currently supported methods
 * 'build' or 'build-samedir' -- (re)create `configure` if needed,
run it in the project root directory, execute first a parallel (for
speed) and then a sequential (for correctness) `make` for targets
that are named further on the command line (or default to 'all' as
implicitly defined in the 'Makefile')
 * 'build-subdir' -- same as above, except that a subdirectory is
created and changed into before doing the job
 * 'install' or 'install-samedir' -- do the 'build-samedir' routine
(including the wiping of the workspace from earlier build products)
followed by a `make install`
 * 'install-subdir' -- do the 'build-subdir' routine followed by
a `make install`
 * 'configure' -- (re)create `configure` if needed, clean up the
project root directory with a `make distclean`, and run `./configure`
 * 'configure-subdir' -- (re)create `configure` if needed, wipe (if
needed), create and cahange into the subdirectory for the build, and
run `configure` (possibly with flags); the project can be further
compiled with `builder.sh make-subdir` from this point
 * 'distcheck' -- (re)create `configure` if needed, clean up the
project root directory with a `make distclean`, run `./configure`
in the project root directory, and finally run `make distcheck`
 * 'distclean' -- (re)create `configure` if needed, run it in the
project root directory to create the proper 'Makefile', and run
`make distclean` to delete everything as configured in 'Makefile'
 * 'make' or 'make-samedir', and 'make-subdir' -- run just the parallel
and sequential 'make' routine for the optionally specified target(s)
from the relevant (base or "relocated") directory; that is -- do not
cleanup and reconfigure the build area
 * 'run-subdir' -- change into the build sub-directory (if exists)
and run the specified command and arguments (if any), so that the
caller generally does not have to think in which exact sub-directory
name a particular build was made (see example below).

Any parameters on the command line after the method specification
are processed according to the method. Currently this means:

 * the optional list of 'Makefile' targets for the 'make-samedir',
'make-subdir', 'build-samedir', 'build-subdir', 'install-samedir'
and 'install-subdir' methods;
 * the optional list of `configure` command-line parameters (one per
token as is normally done for `configure`) can be passed to 'configure'
and 'distcheck' methods;
 * and ignored for others.

The 'run-subdir' mode can be used along with automated testing like this:
----
:; ./autogen.sh build-subdir all test-web && \
   ./autogen.sh run-subdir "`pwd`"/tests/CI/ci-test-restapi.sh sysinfo
----

== Environment variables
The script's behavior can be tuned by environment variables, which
allows for repetitive tuning during the development and rebuild
cycle, while the command-line to execute the build remains short.

Some such variables are intended as "flags" set by the user so as to
modify nuances in behavior, others provide specific values for some
working variables so the script does not have to guess them or fall
back onto hardcoded defaults.

All of these are optional.


=== 'CHECKOUTDIR' path
The 'CHECKOUTDIR' is an optionally defined (usually by our CI scripts)
directory name which should contain the project sources.

The `builder.sh` script rebases into the root of the project sources
as specified by 'CHECKOUTDIR' if present, or guessed from the script's
own path name by default. Then during the script's work the variable
is redefined to contain the full filesystem path of the root of project
sources.


=== 'FORCE_AUTORECONF' flag
An environment variable that can be passed to `autogen.sh` (or `builder.sh`)
to enforce regeneration of the `configure` script even if it is not detected
to be obsolete.

Currently there is no corresponding command-line option equivalent to this.

The accepted values are case-sensitive 'yes' (enforce a rebuild), 
'no' (don't enforce a rebuild even if it would be detected obsolete --
but that logic is effectively skipped by this value) and 'auto' (default).


=== 'MAKE' program
In some systems, there may be a need to specify an exact `make` program
(such as `gmake` where non-GNU `make` may be available as a default).
For such purposes, the 'MAKE' envvar (defaults to 'make') is supported
to set the `make` program name (and standard arguments).

If the 'MAKE' variable value contains a space, it is split into 'MAKE'
(value of the first token) and all the rest becomes an appendix to
'MAKE_OPTS'.


=== 'MAKE_OPTS', 'MAKE_OPTS_PAR' and 'MAKE_OPTS_SEQ' parameters to the `make` program
Optional parameters commonly passed to the `$MAKE` program during a build
operation (basically any of the actions except 'configure', 'distclean'
and 'distcheck'). The value can be parsed at runtime from a multi-token
'MAKE' value as well as explicitly passed from envvars by the user. If
both methods are used, the resulting value should be concatenated.

For example, this can be used to enforce quiet (re-)builds even during a
sequential-only mode:
----
:; export MAKE="make V=0"
----

On the contrary, this should enable high verbosity from GNU `make`, and
enables it even for the parallel `make` phase:
----
:; export MAKE="make V=1"
:; export MAKE_OPTS="--trace"
----

The 'MAKE_OPTS_PAR' and 'MAKE_OPTS_SEQ' define *additional* parameters
for the "parallel" (fast and optional) or "sequential" (reliable and
enforced) phases, and are empty by default (can be set by user envvars.
These values are prepended before the general 'MAKE_OPTS', but if the
flags are in conflict (i.e. both variables set different values of
'V=xxx' for verbosity), the resulting behavior depends on the `make`
program's implementation.


=== 'TIME_MAKE' and 'TIME_CONF' flags or paths
These variables correcpond to command-line flags '--show-timing-make'
and '--show-timing-conf', and allow to specify the command to measure
the execution time of the `make` or `configure` invokations.

The values default to empty (no measurements), can accept the `time`
program name or shell builtin, or the yes/no values for simplicity.
Unknown values are considered as empty with a warning.


=== 'CONFIGURE_FLAGS' list of parameters
Set as an environment variable or overridden by command-line parameter
'--configure-flags="string"', this variable holds a space-separated list
of tokens that would be passed to the `configure` command should one be
called (as part of the 'build*', 'install*', 'configure' or 'distcheck'
methods).

=== 'BLDARCH' tag
The 'BLDARCH' is a string tag for the "relocated" build environment
to use uniquely named directories for the compilation and installation.
This way the same checked-out copy of the source code (or the same
developer workspace) can be used by several building processes.
This is generally needed for multiplatform builds, which we do intend
to support due to x86 development and ARM target devices.

As a bonus, the "relocated build" products are stored in a separate
subdirectory and do not pollute the directories with the source code.

The default value depends on the compilation host's OS and CPU:
----
:; BLDARCH="`uname -s`-`uname -m`"
----


=== 'DESTDIR' path
The 'DESTDIR' generally specifies the prefix used during `make install`
and is the path prepended to the root directory assumed by the project.
That is, the built files are copied into '$DESTDIR/usr/some/thing' when
the installation target is being made.

The default value generally is empty (install into the currently running
OS), and the default value in `builder.sh` depends on '$BLDARCH':
----
:; DESTDIR="/var/tmp/bios-core-instroot-${BLDARCH}"
----


=== 'BUILDSUBDIR' path
If the "relocated" build is invoked ('make-subdir', 'build-subdir', or
'install-subdir'), then the specified directory (absolute, or relative
to the '$CHECKOUTDIR') is used to contain the temporary build products.

The default value in `builder.sh` depends on '$BLDARCH':
----
:; BUILDSUBDIR="build-${BLDARCH}"
----



=== 'WARNLESS_UNUSED' toggle
`export WARNLESS_UNUSED=yes` is one of two ways to quickly suppress the
compiler warnings about unused variables (there is a lot of those in
some of our automatically generated code) so that they do not distract
the developer from noticing some more severe errors.

If suppression of these warnings is enabled by the environment variable
'WARNLESS_UNUSED=yes' or by the command-line option '--warnless-unused',
the `builder.sh` script appends some compiler options to 'CFLAGS' and
'CXXFLAGS'. See the definition in the script source, but at the time of
this writing, the added `gcc` flags are:
----
  -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable
----

Note that these changes to compiler options require regeneration of the
`configure` script, to become enforced (or disabled later on).


=== 'WARN_FATAL' toggle
`export WARN_FATAL=yes` is one of two ways to quickly enforce failures
of compilations if the flaky code has warnings to report. Cumulative with
'WARNLESS_UNUSED' (if both are enabled, then the "unused" warnings are not
reported and thus are not fatal to the build).

Note that these changes to compiler options require regeneration of the
`configure` script, to become enforced (or disabled later on).

This option allows to easily reiterate compilation of just the source files
being improved to counter the warnings, e.g.:
----
:; ./tools/builder.sh -Werror --warnless-unused build
...
:; ./tools/builder.sh make
...
:; ./tools/builder.sh make
...
----


=== 'NODISTCLEAN' toggle
The `builder.sh` script typically wraps a `make -k distclean` to
wipe the workspace before doing a 'configure*' or a 'build*' action.

For an incremental reconfigure+rebuild you can `export NODISTCLEAN=yes`
or pass the '--nodistclean' parameter to `builder.sh`.


=== 'NOPARMAKE' toggle
The `builder.sh` script makes a lot of effort to build the project in
parallel first (for speed, with a best-effort approach ignoring any
errors that might pop up) and afterwards sequentially in order of
dependencies to make sure everything is built and linked correctly.

Still, sometimes there may be concerns that some interim failure of
the parallel make uncovered some race conditions or other brokenness
in our 'Makefile' or in the tools used, and this should generally be
fixed.

`export NOPARMAKE=yes` allows to confirm or rule out such problems: it
tells `builder.sh` to skip the parallel building attempts and proceed
to a sequential `make` right after a successful `configure`.

A sequential-only build also allows easier tracing of the build logs,
as each task is done one by one in order and is easily linked with a
reported error, if any.


=== 'NOSEQMAKE' toggle
For general feature-parity, as well as to speed up certain kinds of
`Makefile`-debugging builds, the sequential build phase can be disabled
with `export NOSEQMAKE=yes` so that only the parallel build is attempted
(if not disabled as well).


=== 'OPTSEQMAKE' toggle
Valid values include 'yes' ('on', 'true'), 'no' ('off', 'false'), and 'auto',
to which the variable defaults if an invalid or empty value was detected.
The effect of 'yes' is to skip the sequential build phase if (and only if)
the parallel build phase succeeded, which may be acceptable for certain
targets; an 'auto' changes into a 'yes' if the selected actions or targets
match a predefined pattern.


=== 'NCPUS' count (semi-private)
The 'NCPUS' variable contains the number of processors available for
the build on this system. While this allows a developer to influence
roughly how much of the hardware resources the build run may consume
(via `make` parallelization), this variable is not quite intended for
manual specification -- rather use 'NPARMAKES' for actual control.

The `builder.sh` script tries several methods to detect the number of
CPUs on the system, or defaults to "1" upon errors.


=== 'NPARMAKES' count, 'PARMAKE_LA_LIMIT' and 'MAXPARMAKES' limiters
The 'NPARMAKES' variable contains the number of `make` processes that
should be run in parallel on the first pass (if not disabled with the
'NOPARMAKE' toggle).

It may be set by the developer as desired, otherwise it defaults to
twice the 'NCPUS' per general recommendation (two processes per CPU,
one is actively compiling and another is waiting for disk I/O), or
to "2" upon errors.

If the enviroment defines a non-negative 'MAXPARMAKES' value (e.g. set
by Jenkins), and if the automatically detected or explicitly requested
'NPARMAKES' value exceeds that limit, then 'NPARMAKES' is reduced to
match the required 'MAXPARMAKES'.

Additionally, the 'PARMAKE_LA_LIMIT' (defaults to 4.0, can be set via
the environment or command-line argument '--parmake-la-limit') should
forbid `make` to spawn additional jobs (second or more) if the system
load average currently exceeds the specified value. Depending on the OS,
this value roughly means the average length of the queue of processes
that are ready for execution and are waiting for an available CPU, over
the past "short stretch of time" (usually one or a few seconds).

