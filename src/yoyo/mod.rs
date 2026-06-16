use std::path::PathBuf;
use pixelscript::{pxs_finalize, pxs_initialize};
use zip::ZipArchive;

mod modules;

pub const YOYO_PATH: &str = ".yoyo";
pub const PIXEL_SCRIPT_ZIP_URL: &str = "https://github.com/jordan-castro/pixelscript/archive/refs/heads/master.zip";
pub const PIXEL_SCRIPT_ZIP_PATH: &str = "pixelscript.zip";

pub type YoyoResult = Result<(), String>;
pub type YoyoRes<T> = Result<T, String>;

#[macro_export]
macro_rules! yoyo_error {
    ($($arg:tt)*) => {
        Err(format!("{}", format_args!($($arg)*)))
    }
}

#[macro_export]
macro_rules! yoyo_dir {
    ($parent:expr) => {{
        let mut dir = PathBuf::from($parent);
        dir.push(crate::yoyo::YOYO_PATH);
        dir
    }};
}

#[macro_export]
macro_rules! get_result {
    ($result:expr) => {{
        match $result {
            Ok(res) => Ok(res),
            Err(err) => yoyo_error!("{}", err.to_string())
        }
    }};
}


/// Create a file
pub fn create_file(path: &PathBuf) -> YoyoRes<std::fs::File> {
    get_result!(std::fs::File::create(path))
}

/// Create a directory
pub fn create_dir(path: &PathBuf) -> YoyoResult {
    get_result!(std::fs::create_dir_all(path))
}

/// Read a file contents.
pub fn read_file(path: &PathBuf) -> YoyoRes<Vec<u8>> {
    get_result!(std::fs::read(path))
}

/// Initialize the yoyo modules.
pub fn add_modules() {
    modules::gen_yoyo_module();
}

/// Start yoyo engine
pub fn start() {
    pxs_initialize();
    add_modules();
}

/// Stop yoyo engine
pub fn stop() {
    pxs_finalize();
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