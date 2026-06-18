use std::{ffi::{CStr, CString}, path::PathBuf};

use crate::{get_result, utils::{YoyoRes, YoyoResult, extract_pixelscript_zip}, yoyo_error};

pub const YOYO_PATH: &str = ".yoyo";
/// The pixelscript.zip file for bundling.
pub static PIXELSCRIPT_ZIP: &[u8] = include_bytes!("../pixelscript.zip");
#[allow(unused)]
static MAIN_CPP: &[u8] = include_bytes!("../main.cpp");

#[macro_export]
macro_rules! yoyo_dir {
    ($parent:expr) => {{
        let mut dir = PathBuf::from($parent);
        if dir.ends_with(crate::yoyo::YOYO_PATH) {
            dir
        } else {
            dir.push(crate::yoyo::YOYO_PATH);
            dir
        }
    }};
}

/// Create a file
pub fn create_file(path: &PathBuf) -> YoyoRes<std::fs::File> {
    get_result!(std::fs::File::create(path))
}

/// Read a file contents.
pub fn read_file(path: &PathBuf) -> YoyoRes<Vec<u8>> {
    get_result!(std::fs::read(path))
}

pub fn remove_dir(path: &PathBuf) -> YoyoResult {
    get_result!(std::fs::remove_dir_all(path))
}

/// Get the user's input, allowing for multi-line input with balanced braces `{}`.
pub fn get_input() -> String {
    let mut result = String::new();
    let mut brace_count = 0;

    loop {
        let mut input = String::new();
        // Read user input
        match std::io::stdin().read_line(&mut input) {
            Ok(_) => {
                // Trim input and add to result with a newline for formatting
                input = input.trim().to_string();
                result.push_str(&input);
                result.push('\n'); // Preserve line breaks

                // Adjust brace count
                for char in input.chars() {
                    match char {
                        '{' => brace_count += 1,
                        '}' => brace_count -= 1,
                        _ => {}
                    }
                }

                // If all braces are balanced, exit loop
                if brace_count <= 0 {
                    break;
                }
            }
            Err(err) => {
                eprintln!("Error reading input: {}", err);
                break;
            }
        }
    }

    result
}

/// Extract the pixelscript zip file
pub fn extract_zip_file(parent: PathBuf) -> YoyoResult {
    let dir = yoyo_dir!(parent);
    get_result!(extract_pixelscript_zip(&dir, PIXELSCRIPT_ZIP))
}

// FFI yoyo.h
unsafe extern "C" {
    pub fn yoyo_init();
    pub fn yoyo_stop();
    pub fn yoyo_repl(runtime: i32);
    fn yoyo_run(runtime: i32, code: *const core::ffi::c_char, file_name: *const core::ffi::c_char) -> *const core::ffi::c_char;
}

pub fn init() {
    unsafe {
        yoyo_init();
    }
}

pub fn stop() {
    unsafe {
        yoyo_stop();
    }
}

pub fn start_repl(runtime: i32) {
    unsafe {
        yoyo_repl(runtime);
    }
}

/// Safe abstraction around `yoyo_run`.
pub fn run(runtime: i32, code: &str, file_name: &str) -> YoyoResult {
    let c_code = get_result!(CString::new(code.to_string()))?;
    let c_name = get_result!(CString::new(file_name.to_string()))?;

    let c_result = unsafe { yoyo_run(runtime, c_code.as_ptr(), c_name.as_ptr()) };
    if c_result.is_null() {
        return yoyo_error!("Null pointer.");
    }

    let result = unsafe { CStr::from_ptr(c_result) };
    let result = get_result!(result.to_str())?;

    if result != "null" && !result.is_empty() {
        yoyo_error!("{result}")
    } else {
        Ok(())
    }
}