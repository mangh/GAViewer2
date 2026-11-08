// Regression test: cmd("import <file>") of a .geo file that starts with 'clear' (as every saved
// state does). The 'clear' replaces the global scope while the command, and this function, which
// belongs to that scope, are still running in it (this crashed: use after free). Now the old scope
// is deleted when the command has finished, and the rest of the command is skipped.

function import_clear()
{
    before = 1,
    cmd("import import_clear.geo");
    after = 2, // not executed: the console was cleared
}
