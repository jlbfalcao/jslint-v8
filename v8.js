
for (i = 0; i < JSLINT.errors.length; i += 1) {
    e = JSLINT.errors[i];
    if (e) {
        print('Lint at line ' + e.line + ' character ' +
                e.character + ': ' + e.reason);
        print((e.evidence || '').
                replace(/^\s*(\S*(\s+\S+)*)\s*$/, "$1"));
        print('');
    }
}
