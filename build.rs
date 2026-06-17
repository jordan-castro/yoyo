use std::io::Write;

/// Zip location
const ZIP_URL: &str = "https://github.com/jordan-castro/pixelscript/archive/refs/heads/master.zip";
const ZIP_LOC: &str = "pixelscript.zip";

macro_rules! unwrap_result {
    ($result:expr) => {
        match $result {
            Ok(pos) => pos,
            Err(err) => {
                eprintln!("{}", err.to_string());
                return;
            }
        }
    };
}

fn main() {
    println!("cargo:rerun-if-changed=build.rs");
    // Forward target to binary
    println!("cargo:rustc-env=TARGET={}", std::env::var("TARGET").unwrap());

    let result = reqwest::blocking::get(ZIP_URL);
    let response = unwrap_result!(result);

    let bytes = unwrap_result!(response.bytes());

    // Write to file
    let mut file = unwrap_result!(std::fs::File::create_new(ZIP_LOC));
    unwrap_result!(file.write(&bytes));
}