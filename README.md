# re
This is a regular expression compiler implemented in C11 based on [Ken Thompson's paper on string search techniques](https://dl.acm.org/doi/pdf/10.1145/363347.363387).
The project is heavily inspired by [Russ Cox's article](https://swtch.com/~rsc/regexp/regexp1.html) -
although the implementation is a naive attempt to implement
it in my own way to better understand the algorithm.
One key difference is that instead of using the conversion of the regular
expression to postfix and then using a pushdown stack to compile to an NFA - I opted to parse the expression into an AST
and then traverse the AST to compile to an NFA.

Right now the compiler only supports the `?`, `*`, `+`, `|`, `.` operators and expression concatenation.
It also supports range based character such as `[a-z0-9]`.
Anchored matches and reptition operators (such as `{m}`, `{m,n}`) should be easy to add but not yet done.

### Compilation
`$ make clean && make`

### Running tests

#### Lexer tests:
`$./lexer_tests`

#### Parser Tests:
`$./parser_tests`

#### Regular Expression Compiler Tests
`$./nfa_executor_tests`


### Benchmarking
Based on the benchmarking expression used in Russ Cox's article. The program generates the
expression
<a href="https://www.codecogs.com/eqnedit.php?latex=\inline&space;a?^na^n" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\inline&space;a?^na^n" title="a?^na^n" /></a>
. For example for `n=3` the expression would be `a?a?a?aaa` and it would be tested against the string `aaa`.
The program generates expressions for n=1 to n=100 and times their execution.
The output is in CSV format with the first value being value of `n` and
second value being time taken to compile and execute the regular expression against the input string.

`$./benchmark`


#### Benchmark results
Following is a comparison of performance of this implementation vs the Java regular expression library
![benchmark](https://github.com/abhinav-upadhyay/re/raw/master/benchmark.png)

Another benchmark is provided for expression `(a|aa)*b` for string of the form `a...a!`. With each increasing `a` in the
string, the Java regex engine gets slower exponentially.
![benchmark2](https://github.com/abhinav-upadhyay/re/raw/master/benchmark2.png)


