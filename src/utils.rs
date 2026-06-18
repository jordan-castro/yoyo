use std::{io::Cursor, path::PathBuf};

use zip::ZipArchive;

pub type YoyoResult = Result<(), String>;
#[allow(unused)]
pub type YoyoRes<T> = Result<T, String>;

#[macro_export]
macro_rules! yoyo_error {
    ($($arg:tt)*) => {
        Err(format!("{}", format_args!($($arg)*)))
    }
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

/// Create a directory
pub fn create_dir(path: &PathBuf) -> YoyoResult {
    if path.exists() {
        return Ok(());
    }
    get_result!(std::fs::create_dir_all(path))
}

/// Extract the pixelscript zip bytes.
pub fn extract_pixelscript_zip(parent_dir: &PathBuf, zip_file_bytes: &[u8]) -> YoyoResult {
    let reader = Cursor::new(zip_file_bytes);
    let mut archive = get_result!(ZipArchive::new(reader))?;

    for i in 0..archive.len() {
        let mut file = get_result!(archive.by_index(i))?;
        let outpath = match file.enclosed_name() {
            Some(fpath) => parent_dir.join(fpath),
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
