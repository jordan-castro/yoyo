use std::{io::Cursor, path::PathBuf};
use pixelscript::{pxs_finalize, pxs_initialize};
use zip::ZipArchive;

mod core;

pub const YOYO_PATH: &str = ".yoyo";
/// The pixelscript.zip file for bundling.
static PIXELSCRIPT_ZIP: &[u8] = include_bytes!("../../pixelscript.zip");
static MAIN_CPP: &[u8] = include_bytes!("../../main.cpp");

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
        if dir.ends_with(crate::yoyo::YOYO_PATH) {
            dir
        } else {
            dir.push(crate::yoyo::YOYO_PATH);
            dir
        }
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
    if path.exists() {
        return Ok(());
    }
    get_result!(std::fs::create_dir_all(path))
}

/// Read a file contents.
pub fn read_file(path: &PathBuf) -> YoyoRes<Vec<u8>> {
    get_result!(std::fs::read(path))
}

pub fn remove_dir(path: &PathBuf) -> YoyoResult {
    get_result!(std::fs::remove_dir_all(path))
}

/// Initialize the yoyo modules.
pub fn add_modules() {
    core::gen_yoyo_module();
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

/// Create .yoyo/
pub fn create_yoyo_temp(parent: &PathBuf) -> YoyoResult {
    let yoyo_dir = yoyo_dir!(parent);
    create_dir(&yoyo_dir)
}

/// Remove .yoyo/
pub fn remove_yoyo_temp(parent: &PathBuf) -> YoyoResult {
    let yoyo_dir = yoyo_dir!(parent);
    remove_dir(&yoyo_dir)
}

/// Extract the pixelscript zip file
pub fn extract_zip_file(parent: PathBuf) -> YoyoResult {
    let dir = yoyo_dir!(parent);
    let reader = Cursor::new(PIXELSCRIPT_ZIP);
    let mut archive = get_result!(ZipArchive::new(reader))?;

    for i in 0..archive.len() {
        let mut file = get_result!(archive.by_index(i))?;
        let outpath = match file.enclosed_name() {
            Some(fpath) => dir.join(fpath),
            None => continue,
        };
        let outpath = outpath.to_str().unwrap().replace("-master", "");

        if file.name().ends_with("/") {
            get_result!(std::fs::create_dir_all(&outpath))?;
        } else {
            let mut outfile = get_result!(std::fs::File::create(outpath))?;
            get_result!(std::io::copy(&mut file, &mut outfile))?;
        }
    }

    Ok(())
}
