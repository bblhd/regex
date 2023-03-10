# regex
A minimalist greedy LHS-first whole-string-match based regular expression library. (*Currently doesn't support unicode*)

## Installation
Link the program against the provided `regex.c` file, and include the following, either in some header file, or in whatever source file the library is being used.
```C
#define BADMATCH ((size_t) -1)
size_t regex(char *, char *);
```

## Usage
Using this library involves a single function.

```C
size_t regex(char *pattern, char *string);
```

When calling this function, *pattern* represents the regular expression and *string* represents the string to match against.
Both are assumed to be null-terminated strings.

It returns the length of *string* that was needed to exhaust the regular expression, and `BADMATCH` if the regular expression didn't exhaust for any reason, including a badly formed regular expression.

### Operations
Order of precedence follows the order presented in this list.

- `A|B` Matches either A or B.
- `A-B` Matches A but not B.
- `AB` Matches A followed by B.
- `A*` Matches zero or more instances of A.
- `A+` Matches one or more instances of A.
- `A?` Matches A optionally.
- `(...)` Matches anything in between the brackets.
- `(?=...)` Regex lookahead, matches the appearance of anything within the brackets without traversing the string
- `(?!...)` Regex negative lookahead, same as previous, but matches non-appearances

### Special characters
| Symbol | Matches
| --- | ---
| `\l` | Any lowercase character.
| `\u` | Any uppercase character.
| `\a` | Any alphabetic character.
| `\p` | Any punctuation character.
| `\d` | Any decimal digit.
| `\x` | Any hexadecimal digit.
| `\m` | Any alphanumeric character.
| `\w` | Any whitespace character.
| `\n` | Newline character.
| `\t` | Tab character.
| `\r` | Carriage return.
| `\s` | Space character.
| `$` | The end of the string.
| `.` | Any character.
