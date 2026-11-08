// Regression test: a dynamic statement that imports a .geo file that starts with 'clear' (see
// import_clear.g). The 'clear' removed the dynamic statement while it was executing (use after
// free); now it is deleted when the update of the dynamic statements has finished.

batch import_clear_dynamic()
{
    k = 1;
    dynamic{d: if (k == 2) {cmd("import import_clear.geo");}};
    k = 2; // (the dynamic statement imports the file at the end of the command)
}
