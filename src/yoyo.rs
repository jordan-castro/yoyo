use std::path::PathBuf;

use zip::ZipArchive;

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

/// Generate the .yoyo folder
pub fn generate_yoyo_folder(parent: PathBuf) -> YoyoResult {
    let dir = yoyo_dir!(parent);

    // Generate dir
    get_result!(std::fs::create_dir(&dir))
}

/// Download pixelscript zip file
pub fn download_pixelscript_file(parent: PathBuf) -> YoyoRes<PathBuf> {
    let mut dir = yoyo_dir!(parent);

    let response = get_result!(reqwest::blocking::get(PIXEL_SCRIPT_ZIP_URL))?;

    if !response.status().is_success() {
        return yoyo_error!("Server returned error: {}", response.status());
    }

    dir.push(PIXEL_SCRIPT_ZIP_PATH);

    let mut file = get_result!(std::fs::File::create(&dir))?;
    let mut content = std::io::Cursor::new(get_result!(response.bytes())?);
    let res = get_result!(std::io::copy(&mut content, &mut file));
    if res.is_ok() {
        Ok(dir)
    } else {
        yoyo_error!("{}", res.unwrap_err().to_string())
    }
}

/// Extract the pixelscript zip file
pub fn extract_zip_file(parent: PathBuf) -> YoyoResult {
    let dir = yoyo_dir!(parent);
    let file = get_result!(std::fs::File::open(dir.join(PIXEL_SCRIPT_ZIP_PATH)))?;
    let mut archive = get_result!(ZipArchive::new(file))?;

    for i in 0..archive.len() {
        let mut file = get_result!(archive.by_index(i))?;
        let outpath = match file.enclosed_name() {
            Some(fpath) => dir.join(fpath),
            None => continue,
        };
        let outpath = outpath.to_str().unwrap().replace("pixelscript-master", "pixelscript");

        if file.name().ends_with("/") {
            get_result!(std::fs::create_dir_all(&outpath))?;
        } else {
            let mut outfile = get_result!(std::fs::File::create(outpath))?;
            get_result!(std::io::copy(&mut file, &mut outfile))?;
        }
    }
    // Delete the pixelscript.zip file
    get_result!(std::fs::remove_file(dir.join(PIXEL_SCRIPT_ZIP_PATH)))
}