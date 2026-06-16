use std::{io::{Cursor, Write}, path::PathBuf};

use pixelscript::{own_string, pxs_clear, pxs_debugvar, pxs_eval, pxs_exec, pxs_freevar, pxs_getstring, pxs_tostring, pxs_varis, shared::{PtrMagic, pxs_Runtime, utils::CStringSafe, var::pxs_VarType}};
use zip::ZipArchive;

use crate::yoyo::{YoyoResult};

mod yoyo;

const USAGE: &str = r#"YoYo v0.0.1
-h, --help          Get this message.

init <name>
compile <main_file>
"#;

/// The pixelscript.zip file for bundling.
static PIXELSCRIPT_ZIP: &[u8] = include_bytes!("../pixelscript.zip");
static MAIN_CPP: &[u8] = include_bytes!("../main.cpp");

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

    yoyo::start();

    for i in 0..args.len() {
        let arg = &args[i];
        if arg.contains("--path") && i != args.len() - 1 {
            context.set_cwd(args[i + 1].to_string());
        }
    }

    if context.cwd.is_empty() {
        context.set_cwd(std::env::current_dir().unwrap().to_str().unwrap().to_string());
    }

    let res = if args[1] == "init" {
        init(&context, args[1..].iter().collect::<Vec<&String>>())
    } else if args[1] == "compile" {
        compile(&context, args[1..].iter().collect::<Vec<&String>>())
    } else if args[1] == "repl" {
        repl(&context, args[1..].iter().collect::<Vec<&String>>())
    } else {
        println!("{USAGE}");
        yoyo::stop();
        return;
    };

    yoyo::stop();

    match res {
        Ok(_) => return,
        Err(err) => eprintln!("{err}"),
    }
}

fn init(context: &Context, args: Vec<&String>) -> YoyoResult {
    if args.len() == 1 {
        return yoyo_error!("Expected <name>");
    }

    // Name
    let name = args[1].to_string();
    let mut parent = context.path_buf();
    parent.push(&name);
    if parent.exists() {
        return yoyo_error!("{name} already exists");
    }
    let mut project_path = parent.clone();
    parent.push(yoyo::YOYO_PATH);

    // Create <name>/.yoyo
    yoyo::create_dir(&parent)?;
    extract_zip_file(project_path.clone())?;

    // Create .yoyo/core/main.cpp
    parent.push("core");
    yoyo::create_dir(&parent)?;
    
    parent.push("main.cpp");
    let mut main_cpp = yoyo::create_file(&parent)?;
    get_result!(main_cpp.write(MAIN_CPP))?;

    // Create a main.py file
    project_path.push("main.py");
    let mut file = yoyo::create_file(&project_path)?;
    get_result!(file.write(b"from yoyo import print\n\nprint('Hello World!')\n"))?;
    Ok(())
}

/// Extract the pixelscript zip file
fn extract_zip_file(parent: PathBuf) -> YoyoResult {
    let dir = yoyo_dir!(parent);
    let reader = Cursor::new(PIXELSCRIPT_ZIP);
    // let file = get_result!(std::fs::File::open(dir.join(PIXEL_SCRIPT_ZIP_PATH)))?;
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

fn compile(context: &Context, args: Vec<&String>) -> YoyoResult {
    if args.len() == 1 {
        return yoyo_error!("Expected <main_file>.");
    }

    let main_file = args[1].to_string();
    let mut parent = context.path_buf();
    parent.push(&main_file);
    // Open and read contents
    let contents = yoyo::read_file(&parent)?;
    if contents.len() == 0 {
        return yoyo_error!("{main_file} is empty.");
    }

    // Compile pixelscript first.

    Ok(())
}

fn repl(context: &Context, args: Vec<&String>) -> YoyoResult {
    if args.len() == 1 {
        return yoyo_error!("Expected language.");
    }

    let binding = args[1].to_string().to_lowercase();
    let language = &binding.trim();
    let runtime = match *language {
        "python" | "py" => pxs_Runtime::pxs_Python,
        "lua" => pxs_Runtime::pxs_Lua,
        "javascript" | "js" => pxs_Runtime::pxs_JavaScript,
        _ => return yoyo_error!("Unsupported language: {language}.")
    };

    loop {
        let mut cstring = CStringSafe::new();
        print!("> ");
        std::io::stdout().flush().unwrap();
        
        // Get input
        let input = yoyo::get_input();
        let input = input.trim();
        if input.to_lowercase() == "quit" {
            break;
        } else if input.len() == 0 {
            continue;
        }

        // Eval for the runtime
        let result = pxs_exec(runtime.clone(), cstring.new_string(input), cstring.new_string("<repl>"));
        
        if pxs_varis(result, pxs_VarType::pxs_Exception) {
            let message = own_string!(pxs_getstring(result));
            pxs_freevar(result);
            println!("{message}");
            pxs_clear();
            yoyo::add_modules();
            continue;
        }
        pxs_freevar(result);
    }

    Ok(())
}