#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>

#define BADMATCH ((size_t)-1)
size_t regex(char *pattern, char *string);

static bool disjunction(char **regex, char **string);
static bool exclusion(char **regex, char **string);
static bool sequence(char **regex, char **string);
static bool compound(char **regex, char **string);
static bool atom(char **regex, char **string);

size_t regex(char *pattern, char *string) {
	char *start = string;
	return disjunction(&pattern, &string) && *pattern=='\0' ? string-start : BADMATCH;
}

static bool disjunction(char **regexp, char **stringp) {
	char *regex = *regexp;
	char *string = *stringp;
	bool matching = false;
	
	if (*regex != ')' && *regex != '\0') {
		char *ending = string;
		matching = exclusion(&regex, &string);
		if (matching) ending = string;
		while (*regex == '|') {
			regex++;
			char newmatch = exclusion(&regex, &string);
			if (newmatch && !matching) ending = string;
			matching = matching || newmatch;
		}
		string = ending;
	}
	
	*regexp = regex;
	if (matching) *stringp = string;
	return matching;
}

static bool exclusion(char **regexp, char **stringp) {
	char *regex = *regexp;
	char *string = *stringp;
	bool matching = true;
	
	if (*regex != '|' && *regex != ')' && *regex != '\0') {
		if (*regex == '-') {
			regex++;
			matching = !sequence(&regex, &string);
		} else {
			matching = sequence(&regex, &string);
		}
		char *ending = string;
		while (*regex == '-') {
			regex++;
			string = *stringp;
			bool matchseg = sequence(&regex, &string);
			matching = matching && !(matchseg && string==ending);
		}
		string = ending;
	}
	
	*regexp = regex;
	if (matching) *stringp = string;
	return matching;
}

static bool sequence(char **regexp, char **stringp) {
	char *regex = *regexp;
	char *string = *stringp;
	bool matching = true;
	
	while (*regex != '-' && *regex != '|' && *regex != ')' && *regex != '\0') {
		bool newmatch = compound(&regex, &string);
		matching = matching && newmatch;
	};
	
	*regexp = regex;
	if (matching) *stringp = string;
	return matching;
}

static bool compound(char **regexp, char **stringp) {
	char *regex = *regexp;
	char *string = *stringp;
	bool matching = true;
	
	matching = atom(&regex, &string);
	bool couldMatchNone = false;
	bool couldMatchMore = false;
	while (*regex=='?' || *regex=='*' || *regex=='+' && (!couldMatchMore || !couldMatchNone)) {
		if (*regex=='*' || *regex=='+') couldMatchMore = true;
		if (*regex=='?' || *regex=='*') couldMatchNone = true;
		regex++;
	}
	if (couldMatchNone) matching = true;
	if (matching && couldMatchMore) do {
		regex = *regexp;
	} while (atom(&regex, &string));
	while (*regex=='?' || *regex=='*' || *regex=='+') regex++;
	
	*regexp = regex;
	if (matching) *stringp = string;
	return matching;
}

static bool atom(char **regexp, char **stringp) {
	char *regex = *regexp;
	char *string = *stringp;
	bool matching = false;
	
	if (*regex == '(') {
		regex++;
		matching = disjunction(&regex, &string);
		if (*regex == ')') regex++;
	} else if (*regex == '?' || *regex == '*' || *regex == '+' || *regex == '|' || *regex == '-' || *regex == ')' || *regex == '\0') {
		matching = false;
	} else if (*regex == '\\') {
		regex++;
		char c = *regex++;
		switch (c) {
			case 'l': matching = islower(*string++); break;
			case 'u': matching = isupper(*string++); break;
			case 'a': matching = isalpha(*string++); break;
			case 'p': matching = ispunct(*string++); break;
			case 'd': matching = isdigit(*string++); break;
			case 'x': matching = isxdigit(*string++); break;
			case 'm': matching = isalnum(*string++); break;
			case 'w': matching = isspace(*string++); break;
			case 'n': matching = *string++ == '\n'; break;
			case 't': matching = *string++ == '\t'; break;
			case 'r': matching = *string++ == '\r'; break;
			case 's': matching = *string++ == ' '; break;
			default: matching = c == *string++;
		}
	} else if (*regex == '.') {
		regex++;
		matching = *string++ != '\0';
	} else if (*regex == '$') {
		regex++;
		matching = *string == '\0';
	} else {
		matching = *regex++ == *string++;
	}
	
	*regexp = regex;
	if (matching) *stringp = string;
	return matching;
}
