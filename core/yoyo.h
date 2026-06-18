#ifndef YOYO_HPP
#define YOYO_HPP
extern "C" {
// Start the Yoyo runtime.
void yoyo_init();

// Stop the Yoyo runtime.
void yoyo_stop();

#ifdef YOYO_APP
// Start the yoyo_repl. This will handle input processing.
// Just pass in the runtime as a number 
// 0 = lua, 1 = python, 2 = js
void yoyo_repl(int runtime);

// Run some code via the yoyo runtime.
const char* yoyo_run(int runtime, const char* code, const char* file_name);
#endif // YOYO_APP 
}
#endif // YOYO_HPP