use std::path::PathBuf;

use crate::yoyo::{YoyoResult};

mod yoyo;

const USAGE: &str = r#"YoYo v0.0.1
-h, --help          Get this message.

start <name>        Start a new YoYo project.
"#;

struct Context {
    cwd: String
}

impl Context {
    pub fn new() -> Self {
        Context { cwd: String::new() }
    }

    pub fn set_cwd(&mut self, buf: String) {
        self.cwd = buf;
    }

    pub fn path_buf(&self) -> PathBuf {
        PathBuf::from(self.cwd.clone())
    }
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let mut context = Context::new();

    if args.len() < 2 {
        println!("{USAGE}");
        return;
    }

    for i in 0..args.len() {
        let arg = &args[i];
        if arg.contains("--path") && i != args.len() - 1 {
            context.set_cwd(args[i + 1].to_string());
        }
    }

    if context.cwd.is_empty() {
        context.set_cwd(std::env::current_dir().unwrap().to_str().unwrap().to_string());
    }

    let res = if args[1] == "start" {
        start(&context, args[1..].iter().collect::<Vec<&String>>())
    } else if args[1] == "pp" {
        pull_pxs(&context, args[1..].iter().collect::<Vec<&String>>())
    } else {
        return;
    };

    match res {
        Ok(_) => return,
        Err(err) => eprintln!("{err}"),
    }
}

/// Pull Pixel Script after started.
fn pull_pxs(context: &Context, args: Vec<&String>) -> YoyoResult {
    let branch = if args.len() > 1 {
        args[1]
    } else {
        "master"
    };

    let mut parent = context.path_buf();
    // Check if we are in a yoyo enviroment
    let files = get_result!(std::fs::read_dir(&parent))?;
    let files = files.collect();

    Ok(())
}

/// Process the start command
fn start(context: &Context, args: Vec<&String>) -> YoyoResult {
    if args.len() == 1 {
        eprintln!("Expected <name>");
        return Ok(());
    }

    let name = args[1];

    let mut parent = context.path_buf();
    parent.push(name);
    get_result!(std::fs::create_dir(&parent))?;

    yoyo::generate_yoyo_folder(parent.clone())?;
    yoyo::download_pixelscript_file(parent.clone())?;
    yoyo::extract_zip_file(parent.clone())?;
    Ok(())
}
